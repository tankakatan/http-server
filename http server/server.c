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

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <strings.h>
#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>


/*  Bind address and port, setup listener, run main loop  */

int start () {
  
  struct sockaddr_in server;
  bzero (&server, sizeof (server));
  
  server.sin_family       = AF_INET;
  server.sin_addr.s_addr  = INADDR_ANY;
  server.sin_port         = htons (DEFAULT_PORT);
  
  int server_sock = socket (PF_INET, SOCK_STREAM, 0);
  
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
  
  while (1) {
    
    struct sockaddr_in client;
    socklen_t client_addr_len;
    bzero (&client, sizeof (client));
    
    int client_sock = accept (server_sock, (struct sockaddr *)&client, &client_addr_len);
    if (client_sock <= 0) {
      perror ("Accept error");
      close (client_sock);
      close (server_sock);
      abort ();
    }
    
    if (display_client_info (&client, client_addr_len) < 0) {
      perror ("Display client info error");
      close (client_sock);
      close (server_sock);
      abort ();
    }
    
    if (handle_client (client_sock, &client, client_addr_len) < 0) {
      perror ("Handle client error");
      close (client_sock);
      close (server_sock);
      abort ();
    }
    
    close (client_sock);
    break;
  }

  close (server_sock);
  return 0;
}


/*  Receive and handle client request, send server response   */

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
  for (int i = 0; i < file_names_amount; i++) {
    size_t pattern_size = strlen (url_path_format) - 2 + strlen (file_names[i]);
    char *pattern       = malloc (pattern_size);
    if (sprintf (pattern, url_path_format, file_names[i]) < 0) {
      perror ("File name pattern compiling error");
      return -1;
    }
    
    int is_matching = match_pattern (pattern, buffer.data);
    printf ("¿Has client requested file name \"%s\"? – %s\n",
            file_names[i], is_matching ? "Yes" : "No");
    
    free (pattern);
  }

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
    
  } else {
    printf ("%sDetected connection with undefined client address length %d"
            "(should be %lu)\n\n", timestring, client_addr_length, sizeof (struct sockaddr_in));
  }

  return 0;
}
