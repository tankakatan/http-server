//
//  server.c
//  http server
//
//  Created by Denis on 27/05/2017.
//  Copyright © 2017 Denis. All rights reserved.
//


#include "server.h"
#include "vars.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <strings.h>
#include <arpa/inet.h>
#include <time.h>


int start () {
  
  struct sockaddr_in server;
  bzero (&server, sizeof (server));
  
  server.sin_family       = AF_INET;
  server.sin_addr.s_addr  = INADDR_ANY;
  server.sin_port         = htons (DEFAULT_PORT);
  
  int server_sock = socket (PF_INET, SOCK_STREAM, 0);
  
  if (bind (server_sock, (struct sockaddr *)&server, sizeof (server)) < 0) {
    perror ("Bind error");
    abort ();
  }
  
  if (listen (server_sock, DEFAULT_QUEUE_LENGTH)< 0) {
    perror ("Listen error");
    abort ();
  }
  
  struct sockaddr_in client;
  socklen_t client_addr_len;
  bzero (&client, sizeof (client));
  while (1) {
    int client_sock = accept (server_sock, (struct sockaddr *)&client, &client_addr_len);
    if (client_sock <= 0) {
      perror ("Accept error");
      abort ();
    }
    
    time_t timestamp  = time (0);
    char * timestring = ctime (&timestamp);
    if (client_addr_len == sizeof (struct sockaddr_in)) {
      char *client_address  = inet_ntoa (client.sin_addr);
      int client_port       = ntohs (client.sin_port);
      printf ("%sDetected connection with %s:%d\n\n", timestring, client_address, client_port);
    
    } else {
      printf ("%sDetected connection with undefined client address length %d"
              "(should be %lu)\n\n", timestring, client_addr_len, sizeof (struct sockaddr_in));
    }
    
    break;
  }
  
  return 0;
}
