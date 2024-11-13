/**
 * pparser_lexer.h
 * Copyright (C) 2024 Paul Passeron
 * PPARSER_LEXER header file
 * Paul Passeron <paul.passeron2@gmail.com>
 */

// File created by the new_file tool !

#ifndef PPARSER_LEXER_H
#define PPARSER_LEXER_H

#include "lexer.h"

typedef enum token_kind_t {
  IDENTIFIER,
  RULE,
  BIG_ARROW,
  STRLIT,
  INTLIT,
  OPEN_BRA,
  CLOSE_BRA,
  OPEN_MARKER,  // %{
  CLOSE_MARKER, // %}
  COLON,
  VERT, // |
  CODE,
} token_kind_t;

typedef struct ppl_t {
  lexer_t l;
  int depth;
} ppl_t;

lexer_t new_pparser_lexer();

const char *human_token_kind(int kind);
void dump_token(token_t token);
ppl_t new_ppl(void);
token_t pparser_lexer_next(ppl_t *l);
token_t pparser_peek(ppl_t *l);

#endif // PPARSER_LEXER_H
