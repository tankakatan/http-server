//
//  parser.c
//  http server
//
//  Created by Denis on 29/05/2017.
//  Copyright © 2017 Denis. All rights reserved.
//


#include "parser.h"

#include <regex.h>
#include <stdlib.h>


int match_pattern (const char *pattern, const char *string) {
  regex_t regex = {};
  int match = 0;
  if (regcomp (&regex, pattern, REG_EXTENDED) != 0) {
    perror ("Regex compile error");
    return match;
  }
  
  int regex_match = regexec (&regex, string, 0, NULL, 0);
  switch (regex_match) {
    case 0            : match = 1; break;
    case REG_NOMATCH  : match = 0; break;
    default           : {
      char error_message[64];
      regerror (regex_match, &regex, error_message, sizeof (error_message));
      printf ("Regex matching error: %s\n", error_message);
      break;
    }
  }
  
  regfree (&regex);
  return match;
}
