/**
 * pparser.c
 * Copyright (C) 2024 Paul Passeron
 * PPARSER source file
 * Paul Passeron <paul.passeron2@gmail.com>
 */

#include "../include/ast.h"
#include "../include/generator.h"
#include "../include/pparser_lexer.h"
#include "../include/pparser_parser.h"
#include "../include/string_view.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

float test() { return 7.2f; }

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

  char output[] = "output";

  char output_c[256] = {0};
  snprintf(output_c, sizeof(output_c), "%s.c", output);

  char output_h[256] = {0};
  snprintf(output_h, sizeof(output_c), "%s.h", output);

  FILE *out_c = fopen(output_c, "w");
  if (!out_c) {
    printf("NOOO");
    perror("Could not open file ");
  }

  FILE *out_h = fopen(output_h, "w");
  if (!out_h) {
    printf("NOOO");
    perror("Could not open file ");
  }

  printf("[INFO] generating parser header at %s\n", output);

  generate_parser_src(out_c, (string_view_t){output, strlen(output)}, prog);
  generate_parser_header(out_h, prog);

  printf("[INFO] successfully generated parser at %s\n", output);

  fclose(out_c);
  fclose(out_h);

  char command[1024] = {0};

  snprintf(command, sizeof(command), "clang-format -i %s", output_c);

  printf("[INFO] formatting generated parser source at %s\n", output_c);

  printf("[CMD] %s\n", command);

  int res = system(command);
  if (res == 0) {
    printf("[INFO] successfully formatted parser source at %s\n", output);
  }

  snprintf(command, sizeof(command), "clang-format -i %s", output_h);
  printf("[INFO] formatting generated parser header at %s\n", output_h);

  printf("[CMD] %s\n", command);

  res = system(command);

  if (res == 0) {
    printf("[INFO] successfully formatted parser head at %s\n", output);
  }

  free(s.contents);

  return 0;
}
