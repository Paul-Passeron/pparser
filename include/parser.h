/**
 * parser.h
 * Copyright (C) 2024 Paul Passeron
 * PARSER header file
 * Paul Passeron <paul.passeron2@gmail.com>
 */

#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "string_view.h"

typedef struct parser_t parser_t;

typedef void *(*parser_rule)(lexer_t *l, parser_t self);

typedef enum parser_kind_t { UNIT, CHAIN, CHOICE } parser_kind_t;

typedef struct parsers_t {
  parser_t *parsers;
  size_t count;
} parsers_t;

typedef union parser_as_t {
  parsers_t multi;
} parser_as_t;

struct parser_t {
  parser_kind_t kind;
  parsers_t children;
  parser_rule rule;
  string_view_t error_message;
};

parser_t new_unit_parser(parser_rule rule, string_view_t error);

// should copy the children (dynamic allocation)
parser_t new_chain_parser(parser_t *children, size_t count,
                          string_view_t error);

// should copy the children (dynamic allocation)
parser_t new_choice_parser(parser_t *children, size_t count,
                           string_view_t error);

void *chain_rule(lexer_t *l, parser_t self);

void *choice_rule(lexer_t *l, parser_t self);

#endif // PARSER_H
