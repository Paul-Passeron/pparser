/**
 * pparser_lexer.c
 * Copyright (C) 2024 Paul Passeron
 * PPARSER_LEXER source file
 * Paul Passeron <paul.passeron2@gmail.com>
 */

#include "../include/pparser_lexer.h"
#include "../include/string_view.h"

lexer_t new_pparser_lexer() {
  lexer_t l = {0};
  l.rules = new_rules();
  add_rule_to_lexer(&l, SV(":"), COLON);
  add_rule_to_lexer(&l, SV("|"), VERT);
  add_rule_to_lexer(&l, SV("\'*\'"), STRLIT);
  add_bad_rule_to_lexer(&l, SV("\'"), SV("Unmatched string literal start."));
  add_rule_to_lexer(&l, SV("@[a-zA-Z_-_]([a-zA-Z_-_0-9])"), DIRECTIVE);
  add_rule_to_lexer(&l, SV("[a-zA-Z_-_]([a-zA-Z_-_0-9])"), IDENTIFIER);
  add_rule_to_lexer(&l, SV("%[a-zA-Z_-_]([a-zA-Z_-_0-9])"), RULE);
  add_rule_to_lexer(&l, SV("[0-9]([0-9])"), INTLIT);
  add_rule_to_lexer(&l, SV("%{"), OPEN_MARKER);
  add_rule_to_lexer(&l, SV("%}"), CLOSE_MARKER);
  add_rule_to_lexer(&l, SV("=>"), BIG_ARROW);
  add_rule_to_lexer(&l, SV("="), EQ);
  add_rule_to_lexer(&l, SV("{"), OPEN_BRA);
  add_rule_to_lexer(&l, SV("}"), CLOSE_BRA);
  add_skip_rule_to_lexer(&l, SV("//*\n"));
  add_skip_rule_to_lexer(&l, SV(" "));
  add_skip_rule_to_lexer(&l, SV("\t"));
  add_skip_rule_to_lexer(&l, SV("\n"));
  add_skip_rule_to_lexer(&l, SV("\b"));
  add_skip_rule_to_lexer(&l, SV("/\\**\\*/"));
  add_bad_rule_to_lexer(&l, SV("/\\*"), SV("Unmatched multi-line comment."));
  add_bad_rule_to_lexer(&l, SV("*"), SV("No rule."));
  return l;
}

ppl_t new_ppl(void) { return (ppl_t){new_pparser_lexer(), 0}; }

token_t pparser_lexer_next(ppl_t *l) {
  if (l->depth == 0) {
    token_t tok = next(&l->l);
    if (tok.kind == OPEN_BRA) {
      l->depth++;
    }
    return tok;
  }
  lexer_t cpy = l->l;
  bool done = false;
  int length = 0;
  bool skip = false;
  while (!is_done(&l->l)) {
    if (l->l.remaining.contents[0] == '%') {
      skip = true;
    }
    if (!skip) {

      if (l->l.remaining.contents[0] == '{') {
        l->depth++;
      }
      if (l->l.remaining.contents[0] == '}') {
        l->depth--;
        if (l->depth == 0) {
          done = true;
          break;
        }
      }
    } else {
      skip = false;
    }
    length++;
    char c = l->l.remaining.contents[0];
    if (c == '\n') {
      l->l.current_loc.line++;
      l->l.current_loc.col = 1;
    } else {
      l->l.current_loc.col++;
    }
    sv_consume(&l->l.remaining);
  }
  if (!done) {
    print_error(stdout, &cpy, SV("Could not read code snippet."));
    return error_token();
  }
  return (token_t){cpy.current_loc, {cpy.remaining.contents, length}, CODE};
}

const char *human_token_kind(int kind) {
  switch (kind) {

  case IDENTIFIER:
    return "IDENTIFIER";
  case RULE:
    return "RULE";
  case BIG_ARROW:
    return "BIG_ARROW";
  case STRLIT:
    return "STRLIT";
  case INTLIT:
    return "INTLIT";
  case OPEN_BRA:
    return "OPEN_BRA";
  case CLOSE_BRA:
    return "CLOSE_BRA";
  case OPEN_MARKER:
    return "OPEN_MARKER";
  case CLOSE_MARKER:
    return "CLOSE_MARKER";
  case COLON:
    return "COLON";
  case VERT:
    return "VERT";
  case CODE:
    return "CODE";
  default:
    return "ERROR";
  }
}

void dump_token(token_t token) {
  print_location_t(stdout, token.location);
  printf("\'" SF "\' %s", SA(token.lexeme), human_token_kind(token.kind));
}

token_t pparser_peek(ppl_t *l) {
  ppl_t cpy = *l;
  return pparser_lexer_next(&cpy);
}