//
//  vars.c
//  http server
//
//  Created by Denis on 16/06/2017.
//  Copyright © 2017 Denis. All rights reserved.
//


#include <stdio.h>
#include "vars.h"

#define INDEX_PAGE "index\\.html"

const char *http_end            = "\r\n\r\n";
const char *url_path_pattern    = "(GET|POST|PUT|DELETE|OPTION)[ ]+((/[a-zA-Z0-9_.-]*?)+)[ ]+HTTP/";
const char *url_path_format     = "(GET|POST|PUT|DELETE|OPTION)[ ]+/%s[ ]+HTTP/";
const char *not_found_page_f    = "404.html";
const char *index_page_f        = INDEX_PAGE;
const char *file_names[1]       = { INDEX_PAGE };
const char *http_headers_format = "HTTP/1.1 200 OK\r\n"
  "Content-Type: text/html; charset=UTF-8\r\n"
  "Content-Length: %d\r\n\r\n"
  "%s\r\n\r\n";
