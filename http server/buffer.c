//
//  buffer.c
//  packet dump
//
//  Created by Denis on 26/03/2017.
//  Copyright © 2017 Denis. All rights reserved.
//


#include "buffer.h"
#include <stdlib.h>


void init_buffer (buffer_t *buffer, size_t size) {
  buffer->data = malloc (size);
  buffer->size = size;
  buffer->used = 0;
}


void extend_buffer (buffer_t *buffer) {
  size_t new_size = buffer->size * BUFFER_MULTIPLIER;
  char * new_data = malloc (new_size);
  
  for (int i = 0; i < buffer->size; i++) {
    new_data[i] = buffer->data[i];
  }
  
  buffer->data = new_data;
  buffer->size = new_size;
}
