/**
 * pparser.c
 * Copyright (C) 2024 Paul Passeron
 * PPARSER source file
 * Paul Passeron <paul.passeron2@gmail.com>
 */

#include "../include/pparser_lexer.h"
#include "../include/string_view.h"
#include <stdio.h>
#include <stdlib.h>

int main(void) {
  ppl_t ppl = new_ppl();
  FILE *f = fopen("unilang.ppars", "r");
  if (!f) {
    printf("NOOO");
    perror("Could not open file ");
  }
  string_view_t s = from_file(f);
  ppl.l.remaining = s;
  fclose(f);
  ppl.l.current_loc = (location_t){"unilang.ppars", 1, 1, 0};
  while (!is_next(&ppl.l)) {
    token_t tok = pparser_lexer_next(&ppl);
    dump_token(tok);
    printf("\n");
    fflush(stdout);
  }

  free(s.contents);
  return 0;
}
