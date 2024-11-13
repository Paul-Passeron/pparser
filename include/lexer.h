/**
 * lexer.h
 * Copyright (C) 2024 Paul Passeron
 * LEXER header file
 * Paul Passeron <paul.passeron2@gmail.com>
 */

#ifndef LEXER_H
#define LEXER_H

#include "string_view.h"
#include <stdbool.h>
#include <stdio.h>

typedef struct location_t {
  const char *filename;
  int line;
  int col;
  bool is_expanded;
} location_t;

typedef struct token_t {
  location_t location;
  string_view_t lexeme;
  int kind;
} token_t;

typedef enum lexer_rule_kind_t {
  GOOD,
  BAD,
  SKIP,
} lexer_rule_kind_t;

typedef union lexer_rule_as_t {
  int good;
  string_view_t error;
} lexer_rule_as_t;

typedef struct lexer_rule_t {
  string_view_t regexp;
  lexer_rule_kind_t kind;
  lexer_rule_as_t as;
} lexer_rule_t;

typedef struct lexer_rules_t {
  lexer_rule_t *data;
  size_t count;
  size_t size;
} lexer_rules_t;

typedef struct lexer_t {
  location_t current_loc;
  string_view_t remaining;
  lexer_rules_t rules;
  size_t eaten;
} lexer_t;

void print_location_t(FILE *f, location_t loc);

void add_rule_to_lexer(lexer_t *l, string_view_t regexp, int value);

void add_bad_rule_to_lexer(lexer_t *l, string_view_t regexp,
                           string_view_t error);

void add_skip_rule_to_lexer(lexer_t *l, string_view_t regexp);

bool is_done(lexer_t *l);

void lexer_skip(lexer_t *l);

bool is_next(lexer_t *l);

token_t next(lexer_t *l);

void print_error(FILE *f, lexer_t *l, string_view_t error_message);

string_view_t location_to_sv(location_t loc);

lexer_rules_t new_rules(void);

bool is_error_tok(token_t tok);

token_t error_token(void);

#endif // LEXER_H
