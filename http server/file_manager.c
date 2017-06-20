//
//  file_manager.c
//  http server
//
//  Created by Denis on 02/06/2017.
//  Copyright © 2017 Denis. All rights reserved.
//

#include "file_manager.h"
#include "vars.h"

#include <unistd.h>
#include <strings.h>

int get_file_content (buffer_t *buffer, const char *fname) {
  
//  char pwd[128];
//  printf ("pwd? %s\n", getcwd (pwd, sizeof (pwd)));
//  printf ("pwd: %s %s\n", pwd, fname);
  
  FILE *file = fopen (fname, "r");
  if (file == NULL) {
    perror ("File open error");
    return -1;
  }
  
  char *cursor = NULL;
  size_t bytes = 0, chars = 0;
  do {
    chars   = (buffer->size - buffer->used) / sizeof (char);
    if (chars < BUFFER_THRESHOLD) {
      extend_buffer (buffer);
      chars = (buffer->size - buffer->used) / sizeof (char);
    }
    cursor  = buffer->data + buffer->used;
    bytes   = fread (cursor, sizeof (char), chars, file);
    
//    printf ("file reading: *cursor: %s, chars: %lu, read: %lu\n", cursor, chars, bytes);
    
    if (bytes <= 0) {
      break;
    }
    buffer->used += bytes;

  } while (1);
  
  fclose (file);
  return 0;
}
