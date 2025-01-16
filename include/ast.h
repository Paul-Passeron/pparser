/**
 * ast.h
 * Copyright (C) 2024 Paul Passeron
 * AST header file
 * Paul Passeron <paul.passeron2@gmail.com>
 */

#ifndef AST_H
#define AST_H

#include "lexer.h"

typedef enum ast_elem_kind_t {
  EK_RULE,
  EK_KIND,
  EK_LEXEME,
} ast_elem_kind_t;

typedef struct ast_elem_t {
  ast_elem_kind_t kind;
  string_view_t rule;
} ast_elem_t;

typedef struct ast_list_t {
  int has_sep;
  ast_elem_t elem;
  ast_elem_t sep;
} ast_list_t;

typedef struct ast_rule_candidate_t {
  ast_elem_t *elems;
  size_t elems_count;
  string_view_t action;
} ast_rule_candidate_t;

typedef struct ast_rule_t {
  string_view_t name;
  bool is_list;
  ast_list_t l;
  ast_rule_candidate_t *candidates;
  size_t candidates_count;
} ast_rule_t;

typedef struct ast_program_t {
  string_view_t preambule;
  ast_rule_t *rules;
  size_t rules_count;
} ast_program_t;

#endif // AST_H
