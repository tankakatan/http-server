//
//  file_manager.c
//  http server
//
//  Created by Denis on 02/06/2017.
//  Copyright © 2017 Denis. All rights reserved.
//

#include "file_manager.h"


int get_file_content (buffer_t *buffer, const char *fname) {
  FILE *file = fopen (fname, "r");
  if (file == NULL) {
    perror ("File open error");
    return -1;
  }
  
  char *cursor = NULL;
  size_t bytes = 0, chars = 0;
  do {
    cursor  = buffer->data + buffer->used;
    chars   = (buffer->size - buffer->used) / sizeof (char);
    bytes   = fread (cursor, sizeof (char), chars, file);
//    printf ("file reading: *cursor: %s, chars: %lu, read: %lu\n", cursor, chars, bytes);
    
    if (bytes <= 0) { break; }
    buffer->used += bytes;
    if (buffer->size <= buffer->used) {
      extend_buffer (buffer);
    }
  } while (1);
  
  fclose (file);
  return 0;
}
