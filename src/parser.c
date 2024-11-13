/**
 * parser.c
 * Copyright (C) 2024 Paul Passeron
 * PARSER source file
 * Paul Passeron <paul.passeron2@gmail.com>
 */

#include "../include/parser.h"
#include <stdio.h>
#include <stdlib.h>

parser_t new_unit_parser(parser_rule rule, string_view_t error) {
  return (parser_t){.kind = UNIT,
                    .children = (parsers_t){NULL, 0},
                    .error_message = error,
                    .rule = rule};
}

parser_t new_multi_parser(parser_t *children, size_t count,
                          string_view_t error) {
  parsers_t ch = {malloc(sizeof(parser_t) * count), count};
  for (size_t i = 0; i < count; i++) {
    ch.parsers[i] = children[i];
  }
  return (parser_t){
      .kind = -1,
      .children = ch,
      .error_message = error,
      .rule = NULL,
  };
}

// should copy the children (dynamic allocation)
parser_t new_chain_parser(parser_t *children, size_t count,
                          string_view_t error) {
  parser_t res = new_multi_parser(children, count, error);
  res.kind = CHAIN;
  res.rule = &chain_rule;
  return res;
}

parser_t new_choice_parser(parser_t *children, size_t count,
                           string_view_t error) {
  parser_t res = new_multi_parser(children, count, error);
  res.kind = CHOICE;
  res.rule = &choice_rule;
  return res;
}

// this returns an array of pointer.
// Its actual return type is void**
void *chain_rule(lexer_t *l, parser_t self) {
  lexer_t cpy = *l;
  void **res = malloc(sizeof(void *) * self.children.count);
  for (size_t i = 0; i < self.children.count; i++) {
    parser_t p = self.children.parsers[i];
    res[i] = p.rule(l, p);
    if (res[i] == NULL) {
      print_error(stderr, &cpy, self.error_message);
      print_error(stderr, l, p.error_message);
      return NULL;
    }
  }
  return res;
}

void *choice_rule(lexer_t *l, parser_t self) {
  int current_max = -1;
  lexer_t *current_lexer = malloc(sizeof(lexer_t));
  *current_lexer = *l;
  parser_t *best_parser = malloc(sizeof(parser_t));
  *best_parser = self;
  for (size_t i = 0; i < self.children.count; i++) {
    lexer_t cpy = *l;
    parser_t p = self.children.parsers[i];
    void *res = p.rule(&cpy, p);
    if (res) {
      free(current_lexer);
      free(best_parser);
      return res;
    }
    int dist = l->eaten - cpy.eaten;
    if (dist > current_max) {
      current_max = dist;
      *current_lexer = cpy;
      *best_parser = p;
    }
  }
  print_error(stderr, l, self.error_message);
  if (best_parser != NULL) {
    print_error(stderr, current_lexer, best_parser->error_message);
  }
  free(current_lexer);
  free(best_parser);
  return NULL;
}
