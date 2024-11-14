#include "include/lexer.h"
#include <stdlib.h>
// PREAMBULE:

// Toylang parser, generated using PPARSER.
#include "toylang_lexer.h"

token_t *parse_token_lexeme(lexer_t *l, int *worked, string_view_t lexeme);
token_t *parse_token_kind(lexer_t *l, int *worked, int kind);
// RULE let
void *parse_let(lexer_t *l, int *worked);

// RULE leaf
void *parse_leaf(lexer_t *l, int *worked);

// RULE expr
void *parse_expr(lexer_t *l, int *worked);
