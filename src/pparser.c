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

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr,
            "Usage: %s <input_file.ppars> [output_c_dir] [output_h_dir] "
            "[output_prefix]\n",
            argv[0]);
    return 1;
  }

  char *input_file = argv[1];
  char *output_c_dir =
      (argc > 2) ? argv[2] : "."; // Default to current directory
  char *output_h_dir =
      (argc > 3) ? argv[3] : "."; // Default to current directory
  char *output_prefix =
      (argc > 4) ? argv[4] : "output"; // Default prefix is "output"

  ppl_t ppl = new_ppl();
  FILE *f = fopen(input_file, "r");
  if (!f) {
    perror("Could not open input file");
    return 1;
  }

  string_view_t s = from_file(f);

  ppl.l.remaining = s;
  fclose(f);
  ppl.l.current_loc = (location_t){input_file, 1, 1, 0};

  // ppl_t cpy = ppl;
  // token_t tok = pparser_lexer_next(&cpy);
  // while (!is_error_tok(tok)) {
  //   dump_token(tok);
  //   printf("\n");
  //   tok = pparser_lexer_next(&cpy);
  // }

  ast_program_t prog = program_parser(&ppl);

  char output_c[256] = {0};
  snprintf(output_c, sizeof(output_c), "%s/%s.c", output_c_dir, output_prefix);

  char output_h[256] = {0};
  snprintf(output_h, sizeof(output_h), "%s/%s.h", output_h_dir, output_prefix);

  FILE *out_c = fopen(output_c, "w");
  if (!out_c) {
    perror("Could not open output C file");
    return 1;
  }

  FILE *out_h = fopen(output_h, "w");
  if (!out_h) {
    perror("Could not open output header file");
    fclose(out_c); // Close the C file if the header file fails to open
    return 1;
  }

  printf("[INFO] generating parser header at %s\n", output_prefix);

  generate_parser_src(out_c, (string_view_t){output_h, strlen(output_h)}, prog);
  generate_parser_header(out_h, prog);

  printf("[INFO] successfully generated parser at %s\n", output_prefix);

  fclose(out_c);
  fclose(out_h);

  char command[1024] = {0};

  snprintf(command, sizeof(command), "clang-format -i %s", output_c);
  printf("[INFO] formatting generated parser source at %s\n", output_c);
  printf("[CMD] %s\n", command);

  int res = system(command);
  if (res == 0) {
    printf("[INFO] successfully formatted parser source at %s\n", output_c);
  }

  snprintf(command, sizeof(command), "clang-format -i %s", output_h);
  printf("[INFO] formatting generated parser header at %s\n", output_h);
  printf("[CMD] %s\n", command);

  res = system(command);
  if (res == 0) {
    printf("[INFO] successfully formatted parser header at %s\n", output_h);
  }

  free(s.contents);

  return 0;
}