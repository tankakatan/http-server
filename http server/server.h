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

void subscribe_to_signals ();
void on_child_did_stop (int);
void on_app_interrupted (int);


#endif /* server_h */
