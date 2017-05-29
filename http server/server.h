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
int display_client_info (const struct sockaddr_in *, const socklen_t);
int handle_client (const int, const struct sockaddr_in *, const socklen_t);


#endif /* server_h */
