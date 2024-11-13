/**
 * pparser.c
 * Copyright (C) 2024 Paul Passeron
 * PPARSER source file
 * Paul Passeron <paul.passeron2@gmail.com>
 */

#include "../include/ast.h"
#include "../include/pparser_lexer.h"
#include "../include/pparser_parser.h"
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

  // ppl_t cpy = ppl;
  // while (!is_next(&cpy.l)) {
  //   token_t tok = pparser_lexer_next(&cpy);
  //   dump_token(tok);
  //   printf("\n");
  //   fflush(stdout);
  // }
  ast_program_t prog = program_parser(&ppl);

  printf("Prog count: %ld\n", prog.rules_count);
  free(s.contents);
  return 0;
}
