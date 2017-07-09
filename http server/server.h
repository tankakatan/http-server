//
//  server.h
//  http server
//
//  Created by Denis on 27/05/2017.
//  Copyright © 2017 Denis. All rights reserved.
//

#ifndef server_h
#define server_h

#include <stdio.h>


int start ();
int run_client_process (int, struct sockaddr_in *, const socklen_t);
int display_client_info (const struct sockaddr_in *, const socklen_t);
int handle_client (const int, const struct sockaddr_in *, const socklen_t);
int send_http_response (const int, const char *);
int read_and_send (const int, const char *);
void emergency_exit (const char *, int *, void **);
void child_status_did_change (int, struct __siginfo *, void *);


#endif /* server_h */
