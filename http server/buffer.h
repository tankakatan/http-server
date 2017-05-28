//
//  buffer.h
//  packet dump
//
//  Created by Denis on 26/03/2017.
//  Copyright © 2017 Denis. All rights reserved.
//


#ifndef buffer_h
#define buffer_h

#include <stdio.h>


#define BUFFER_MULTIPLIER 2


typedef struct {
  
  size_t size;
  size_t used;
  char * data;
  
} buffer_t;


void init_buffer (buffer_t *buffer, size_t length);
void extend_buffer (buffer_t *buffer);


#endif /* buffer_h */
