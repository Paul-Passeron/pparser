/**
 * generator.c
 * Copyright (C) 2024 Paul Passeron
 * GENERATOR source file
 * Paul Passeron <paul.passeron2@gmail.com>
 */

#include "../include/generator.h"
#include <stdlib.h>
#include <string.h>

void generate_parser_header(FILE *f, ast_program_t prog) {
  string_view_t rules[1024] = {0};
  fprintf(f, "#ifndef PPARSER_H\n");
  fprintf(f, "#define PPARSER_H\n");
  fprintf(f, "#include \"lexer.h\"\n");
  fprintf(f, "#include <stdlib.h>\n");
  fprintf(f, "// PREAMBULE:\n" SF "\n", SA(prog.preambule));
  fprintf(f, "token_t *parse_token_lexeme(lexer_t *l, int *worked, "
             "string_view_t lexeme);\n");
  fprintf(f, "token_t *parse_token_kind(lexer_t *l, int *worked, "
             "int kind);\n");
  // creating the context
  for (size_t i = 0; i < prog.rules_count; i++) {
    fprintf(f, "// RULE " SF "\n", SA(prog.rules[i].name));
    rules[i] = prog.rules[i].name;
    for (size_t j = 0; j < i; j++) {
      if (sv_eq(rules[j], rules[i])) {
        printf("[ERROR] duplicate rule \'" SF "\'\n", SA(rules[i]));
        exit(1);
      }
    }
    fprintf(f, "void *parse_" SF "(lexer_t *l, int *worked);\n\n",
            SA(rules[i]));
  }
  fprintf(f, "#endif // PPARSER_H\n");
}

void generate_parser_src(FILE *f, string_view_t name, ast_program_t prog) {
  string_view_t rules[1024] = {0};
  bool must_fail = false;
  fprintf(f, "#include \"../" SF "\"\n", SA(name));
  fprintf(f, "token_t *parse_token_lexeme(lexer_t *l, int *worked, "
             "string_view_t lexeme) {\n");
  fprintf(f, "token_t tok = next(l);\n");
  fprintf(f, "*worked = 0;\n");
  fprintf(f, "if(is_error_tok(tok)) {\nreturn NULL;}\n");
  fprintf(
      f, "if(sv_eq(tok.lexeme, lexeme)) {\ntoken_t *res = "
         "malloc(sizeof(token_t));\n*res = tok;\n*worked = 1;\nreturn res;}\n");
  fprintf(f, "return NULL;\n}\n\n");

  fprintf(f, "token_t *parse_token_kind(lexer_t *l, int *worked, "
             "int kind) {\n");
  fprintf(f, "token_t tok = next(l);\n");
  fprintf(f, "*worked = 0;\n");
  fprintf(f, "if(is_error_tok(tok)) {\nreturn NULL;}\n");
  fprintf(
      f, "if(tok.kind == kind) {\ntoken_t *res = "
         "malloc(sizeof(token_t));\n*res = tok;\n*worked = 1;\nreturn res;}\n");
  fprintf(f, "return NULL;\n}\n\n");
  // creating the context
  for (size_t i = 0; i < prog.rules_count; i++) {

    rules[i] = prog.rules[i].name;
    for (size_t j = 0; j < i; j++) {
      if (sv_eq(rules[j], rules[i])) {
        printf("[ERROR] duplicate rule \'" SF "\'\n", SA(rules[i]));
        exit(1);
      }
    }
    fprintf(f, "// RULE " SF "\n", SA(prog.rules[i].name));
    for (size_t j = 0; j < prog.rules[i].candidates_count; j++) {
      // ast_rule_candidate_t candidate = rule.candidates[j];
      fprintf(f, "void *parse_" SF "_c%ld(lexer_t *l, int *worked);\n\n",
              SA(rules[i]), j);
    }
  }
  for (size_t i = 0; i < prog.rules_count; i++) {
    fprintf(f, "// RULE " SF "\n", SA(rules[i]));
    fprintf(f, "void *parse_" SF "(lexer_t *l, int *worked){\n", SA(rules[i]));

    ast_rule_t rule = prog.rules[i];
    fprintf(f, "*worked = 0;\n");
    fprintf(f, "int rule_worked = 0;\n");
    fprintf(f, "void *rule_res = NULL;\n");
    fprintf(f, "lexer_t rule_cpy = *l;\n");
    for (size_t j = 0; j < rule.candidates_count; j++) {
      if (j > 0) {
        fprintf(f, "rule_cpy = *l;\n");
      }
      fprintf(f, "rule_res = parse_" SF "_c%ld(&rule_cpy, &rule_worked);\n",
              SA(rules[i]), j);
      fprintf(f, "if (rule_worked) {\n*worked = 1;\n*l = rule_cpy;\nreturn "
                 "rule_res;\n}\n");
    }
    fprintf(f, "return NULL;\n}\n\n");
  }
  for (size_t i = 0; i < prog.rules_count; i++) {
    ast_rule_t rule = prog.rules[i];
    for (size_t j = 0; j < rule.candidates_count; j++) {
      ast_rule_candidate_t candidate = rule.candidates[j];
      fprintf(f, "void *parse_" SF "_c%ld(lexer_t *l, int *worked){\n",
              SA(rules[i]), j);
      fprintf(f, "*worked = 0;\n");
      char *used = malloc(sizeof(int) * candidate.elems_count);
      memset(used, 0, sizeof(int) * candidate.elems_count);
      for (size_t k = 0; k < candidate.action.length; k++) {
        char c = candidate.action.contents[k];
        if (c != '%')
          continue;
        c = candidate.action.contents[++k];
        char next = candidate.action.contents[++k];
        if (c == '{') {
          char buffer[1024] = {0};
          int cter = 0;
          c = next;
          next = candidate.action.contents[++k];
          while (c != '%' && next != '}') {
            buffer[cter++] = c;
            c = next;
            next = candidate.action.contents[++k];
          }
          size_t res = atoi(buffer);
          if (res >= candidate.elems_count) {
            printf("Bad element in candidate n°%ld of rule " SF ": %ld\n",
                   j + 1, SA(rule.name), res);
            exit(1);
          }
          used[res] = 1;
        }
      }

      for (size_t k = 0; k < candidate.elems_count; k++) {
        if (used[k]) {
          fprintf(f, "void *elem_%ld = ", k);
        } else {
          fprintf(f, "free(");
        }
        ast_elem_t elem = candidate.elems[k];
        if (elem.kind == EK_KIND) {
          fprintf(f, "parse_token_kind(l, worked, " SF ")", SA(elem.rule));
        } else if (elem.kind == EK_LEXEME) {
          fprintf(f, "parse_token_lexeme(l, worked, SV(\"" SF "\"))",
                  SA(elem.rule));
        } else {
          int found = 0;
          for (size_t l = 0; l < prog.rules_count; l++) {
            if (sv_eq(prog.rules[l].name, elem.rule)) {
              found = 1;
              break;
            }
          }
          if (!found) {
            must_fail = true;
            fprintf(stderr, "Error: unknown rule name '" SF "'\n",
                    SA(elem.rule));
          }
          fprintf(f, "parse_" SF "(l, worked)", SA(elem.rule));
        }
        if (!used[k]) {
          fprintf(f, ")");
        }
        fprintf(f, ";\nif(!*worked){\nreturn NULL;\n}\n");
      }

      for (size_t k = 0; k < candidate.action.length; k++) {
        char c = candidate.action.contents[k];
        if (c != '%') {
          fprintf(f, "%c", c);
          continue;
        }
        c = candidate.action.contents[++k];
        char next = candidate.action.contents[++k];

        if (c == '{') {
          char buffer[1024] = {0};
          int cter = 0;
          c = next;
          next = candidate.action.contents[++k];
          while (c != '%' && next != '}') {
            buffer[cter++] = c;
            c = next;
            next = candidate.action.contents[++k];
          }
          size_t res = atoi(buffer);
          fprintf(f, "elem_%ld", res);
        }
      }

      fprintf(f, "}\n");
    }
  }
  if (must_fail) {
    exit(1);
  }
}
