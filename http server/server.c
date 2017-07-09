//
//  server.c
//  http server
//
//  Created by Denis on 27/05/2017.
//  Copyright © 2017 Denis. All rights reserved.
//

#include "server.h"
#include "vars.h"
#include "buffer.h"
#include "parser.h"
#include "file_manager.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <strings.h>
#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>


/*  Bind address and port, set up listener, run main loop  */

int start () {
  
  struct sockaddr_in server;
  bzero (&server, sizeof (server));
  
  server.sin_family       = AF_INET;
  server.sin_addr.s_addr  = INADDR_ANY;
  server.sin_port         = htons (DEFAULT_PORT);
  
  int server_sock = socket (PF_INET, SOCK_STREAM, 0);
  
#ifdef SO_REUSEPORT
  int reuse = 1;
  if (setsockopt(server_sock, SOL_SOCKET, SO_REUSEPORT, (const char*)&reuse, sizeof (reuse)) < 0) {
    perror("setsockopt(SO_REUSEPORT) failed");
  }
#endif
  
  if (bind (server_sock, (struct sockaddr *)&server, sizeof (server)) < 0) {
    perror ("Bind error");
    close (server_sock);
    abort ();
  }
  
  if (listen (server_sock, DEFAULT_QUEUE_LENGTH)< 0) {
    perror ("Listen error");
    close (server_sock);
    abort ();
  }
  
  printf ("Setting child signal handler\n");
  
  struct sigaction action;
  bzero (&action, sizeof (struct sigaction));
  action.sa_flags = 0;
  action.sa_sigaction = &child_status_did_change;
  action.sa_mask = SA_NODEFER;  // If several children will stop in one time
  sigaction (SIGCHLD, &action, 0);
  sigaction (SIGINT, &action, 0);
  
  while (1) {
    
    struct sockaddr_in client;
    socklen_t client_addr_len = sizeof (struct sockaddr_in);
    bzero (&client, sizeof (client));
    
    int client_sock = accept (server_sock, (struct sockaddr *)&client, &client_addr_len);
    if (client_sock <= 0) {
      perror ("Accept error");
      close (client_sock);
      break;
    }
    
    int child_process;
    if ((child_process = fork ()) == 0) {
      
      int exit_status = run_client_process (client_sock, &client, client_addr_len);
      close (client_sock);
      exit (exit_status);
      
    } else if (child_process < 0) {
      perror ("Fork error:");
      close (client_sock);
      break;
    }
    
    printf ("Awaiting for a new connection\n");
  }

  close (server_sock);
  abort ();
}


/*  Run client handling process   */

int run_client_process (int client_sock, struct sockaddr_in *client, const socklen_t client_addr_len) {
  printf ("Handling client in a separate process:\n\n");
  
  if (display_client_info (client, client_addr_len) < 0) {
    perror ("Display client info error");
    return EXIT_FAILURE;
  }
  
  if (handle_client (client_sock, client, client_addr_len) < 0) {
    perror ("Handle client error");
    return EXIT_FAILURE;
  }
  
  printf("The client was successfully handled\n\n");
  return EXIT_SUCCESS;
}


/*  Handle system signals  */

void child_status_did_change (int signal, struct __siginfo *signal_info, void *something) {
  switch (signal) {
    case SIGCHLD: {
      int returned_value = 0;
      while (wait (&returned_value) > 0) {
        printf ("Awaited value is returned %d\n", returned_value);
      };
      break;
    }

    case SIGINT: {
      printf ("Bye!\n");
      exit (EXIT_SUCCESS);
    }
      
    default: {
      printf ("Received signal %d\n\n", signal);
      break;
    }
  }
}


/*  Receive and handle client request  */

int handle_client (const int client_socket, const struct sockaddr_in *client_addr,
                   const socklen_t client_addr_length) {

  buffer_t buffer;
  init_buffer (&buffer, INITIAL_BUFFER_SIZE);
  
  size_t bytes_read = 0;
  do {
    bytes_read = recv (client_socket, buffer.data + buffer.used, buffer.size - buffer.used, 0);
  
    if (bytes_read > 0) {
      buffer.used += bytes_read;
    
    } else { break; }
    
    if (strstr (buffer.data, http_end) != NULL) {
      break;
    
    } else if (buffer.size <= buffer.used) {
      extend_buffer (&buffer);
    }
    
  } while (1);
  
  printf ("Client request (%lu bytes of %lu available):\n%s\n",
          buffer.used, buffer.size, buffer.data);
  
  int file_names_amount = sizeof (file_names) / sizeof (char *);
  int is_matching = 0;
  for (int i = 0; i < file_names_amount; i++) {
    size_t pattern_size = strlen (url_path_format) - 2 + strlen (file_names[i]);
    char *pattern       = malloc (pattern_size);
    if (sprintf (pattern, url_path_format, file_names[i]) < 0) {
      perror ("File name pattern compiling error");
      free (buffer.data);
      free (pattern);
      return -1;
    }
    
    is_matching = match_pattern (pattern, buffer.data);
    if (is_matching) {
      if (read_and_send (client_socket, index_page_f) < 0) {
        perror ("Send response error");
        free (buffer.data);
        free (pattern);
        return -1;
      }
      free (buffer.data);
      free (pattern);
      break;
    }
    free (pattern);
  }
  
  if (!is_matching) {
    if (read_and_send (client_socket, not_found_page_f) < 0) {
      perror ("Send response error");
      free (buffer.data);
      return -1;
    }
  }
  
  return 0;
}


/*  Send response  */

int send_http_response (const int socket, const char* fname) {
  buffer_t buffer;
  init_buffer (&buffer, INITIAL_BUFFER_SIZE);
  
  if (get_file_content (&buffer, fname) < 0) {
    perror ("Get file content error");
    free (buffer.data);
    return -1;
  }
  
  size_t buffer_used = buffer.used;
  size_t buffer_used_digits = 0;
  while (buffer_used != 0) {
    buffer_used /= 10;
    ++buffer_used_digits;
  }

  size_t format_chars = 4;
  size_t response_size = buffer.used + strlen (http_headers_format) - format_chars + buffer_used_digits;
  char *response = malloc (response_size);
  
  printf ("Total response length is: %lu + %lu + %lu - %lu = %lu\n",
          strlen(buffer.data), strlen(http_headers_format), buffer_used_digits,
          format_chars, response_size);
  
  if (snprintf (response, response_size, http_headers_format, buffer.used, buffer.data) < 0) {
    perror ("Response compile error");
    free (buffer.data);
    free (response);
    return -1;
  }
  
  printf ("Prepared response (%lu bytes):\n%s\n", response_size, response);
  
  ssize_t bytes = 0, sent = 0;
  do {
    if ((bytes = send (socket, response + sent, response_size - sent, 0)) < 0) {
      perror ("Send error");
      free (buffer.data);
      free (response);
      return -1;
    }
    
    if (bytes == 0) { break; }
    sent += bytes;
  } while (1);
  
  printf ("File %s has been sent successfully.\n\n", fname);
  free (response);
  free (buffer.data);
  return 0;
}


/*  Send response 2  */

int read_and_send (const int socket, const char* fname) {
  
  FILE *file = fopen (fname, "r");
  if (file == NULL) {
    perror ("File open error");
    return -1;
  }
  
  buffer_t buffer;
  init_buffer (&buffer, INITIAL_BUFFER_SIZE);
  
  size_t headers_length = strlen (http_headers);
  while (buffer.size < headers_length) {
    extend_buffer (&buffer);
  }
  
  memcpy (buffer.data, http_headers, headers_length);
  buffer.used = headers_length;
  
  char *cursor = NULL;
  long
    chars_sent = 0;
  
  size_t
    chars_read = 0,
    bytes_read = 0,
    bytes_sent = 0,
    reading_complete = 0,
    sending_complete = 0;
  
  do {
    if (!reading_complete) {
      chars_read = (buffer.size - buffer.used) / sizeof (char);
      if (chars_read < BUFFER_THRESHOLD) {
        extend_buffer (&buffer);
        chars_read = (buffer.size - buffer.used) / sizeof (char);
      }
      
      cursor      = buffer.data + buffer.used;
      bytes_read  = fread (cursor, sizeof (char), chars_read, file);
      
      if (bytes_read <= 0) { reading_complete = 1; } else {
//        printf ("Data read: *cursor: %s, chars: %lu, read: %lu\n\n", cursor, chars_read, bytes_read);
        buffer.used += bytes_read;
      }
    }
    
    if (!sending_complete && (buffer.used > 0)) {
      if ((chars_sent = send (socket, buffer.data + bytes_sent, buffer.used - bytes_sent, 0)) < 0) {
        perror ("Send error");
        free (buffer.data);
        fclose (file);
        return -1;
      }
      if (chars_sent == 0) { sending_complete = 1; } else {
        bytes_sent += chars_sent;
//        printf ("Data sent: *cursor: %s, fraction: %lu, total: %lu\n\n",
//                buffer.data + bytes_sent - chars_sent, chars_sent, bytes_sent);
      }
    }
    
  } while ((reading_complete == 0) &&
           (sending_complete == 0));
  
  free (buffer.data);
  fclose (file);
  
  return 0;
}

/*  Display client ip address and destination port  */

int display_client_info (const struct sockaddr_in *client, const socklen_t client_addr_length) {
  time_t timestamp  = time (0);
  char * timestring = ctime (&timestamp);
  
  if (client_addr_length == sizeof (struct sockaddr_in)) {
    char *client_address  = inet_ntoa (client->sin_addr);
    int client_port       = ntohs (client->sin_port);
    printf ("%sDetected connection with %s:%d\n\n", timestring, client_address, client_port);
//    free (client_address);

  } else {
    printf ("%sDetected connection with undefined client address length %d"
            "(should be %lu)\n\n", timestring, client_addr_length, sizeof (struct sockaddr_in));
  }

//  free (timestring);
  return 0;
}
