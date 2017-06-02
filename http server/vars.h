//
//  vars.h
//  http server
//
//  Created by Denis on 27/05/2017.
//  Copyright © 2017 Denis. All rights reserved.
//

#ifndef vars_h
#define vars_h

#define DEFAULT_PORT 1337
#define DEFAULT_QUEUE_LENGTH 1
#define INITIAL_BUFFER_SIZE 64


const char *http_end = "\r\n\r\n";
const char *url_path_pattern = "(GET|POST|PUT|DELETE|OPTION)[ ]+((/[a-zA-Z0-9_.-]*?)+)[ ]+HTTP/";
const char *url_path_format = "(GET|POST|PUT|DELETE|OPTION)[ ]+/%s[ ]+HTTP/";
const char *file_names[1] = {"page",};
const char *http_404_f = "404.html";
const char *http_page_f = "page.html";
const char *http_headers_format = "HTTP/1.x 200 OK\r\n"
  "Content-Type: text/html; charset=UTF-8\r\n\r\n"
  "%s\r\n\r\n";


#endif /* vars_h */
