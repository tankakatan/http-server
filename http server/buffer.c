//
//  buffer.c
//  packet dump
//
//  Created by Denis on 26/03/2017.
//  Copyright © 2017 Denis. All rights reserved.
//


#include "buffer.h"

#include <stdlib.h>
#include <strings.h>


void init_buffer (buffer_t *buffer, size_t size) {
  buffer->data = malloc (size);
  buffer->size = size;
  buffer->used = 0;

  bzero (buffer->data, size);
}


void extend_buffer (buffer_t *buffer) {
  size_t new_size = buffer->size * BUFFER_MULTIPLIER;
  char new_data[new_size];
  bzero (new_data, new_size);
  memcpy (new_data, buffer->data, buffer->used);
  
  buffer->size = new_size;
  buffer->data = malloc (new_size);
  bzero (buffer->data, new_size);
  memcpy (buffer->data, new_data, new_size);
}
