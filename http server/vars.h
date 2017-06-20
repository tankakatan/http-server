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
#define BUFFER_THRESHOLD 16


extern const char *http_end;
extern const char *url_path_pattern;
extern const char *url_path_format;
extern const char *file_names[1];
extern const char *not_found_page_f;
extern const char *index_page_f;
extern const char *http_headers_format;
extern const char *http_headers;


#endif /* vars_h */
