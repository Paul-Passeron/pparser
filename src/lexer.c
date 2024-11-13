/**
 * lexer.c
 * Copyright (C) 2024 Paul Passeron
 * LEXER source file
 * Paul Passeron <paul.passeron2@gmail.com>
 */

#include "../include/lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_location_t(FILE *f, location_t loc) {
  fprintf(f, "%s:%d:%d: %s", loc.filename, loc.line, loc.col,
          loc.is_expanded ? "[IN MACRO EXPANSION] " : "");
}

void append_rule(lexer_t *l, lexer_rule_t rule) {
  if (l->rules.count >= l->rules.size) {
    lexer_rule_t *new_rules = malloc(sizeof(lexer_rule_t) * l->rules.size * 2);
    for (size_t i = 0; i < l->rules.count; i++) {
      new_rules[i] = l->rules.data[i];
    }
    free(l->rules.data);
    l->rules.data = new_rules;
  }
  l->rules.data[l->rules.count++] = rule;
}

void add_rule_to_lexer(lexer_t *l, string_view_t regexp, int value) {
  lexer_rule_t res;
  res.regexp = regexp;
  res.kind = GOOD;
  res.as.good = value;
  append_rule(l, res);
}

void add_bad_rule_to_lexer(lexer_t *l, string_view_t regexp,
                           string_view_t error) {
  lexer_rule_t res;
  res.regexp = regexp;
  res.kind = BAD;
  res.as.error = error;
  append_rule(l, res);
}

void add_skip_rule_to_lexer(lexer_t *l, string_view_t regexp) {
  lexer_rule_t res = {regexp, SKIP, {0}};
  append_rule(l, res);
}

bool is_done(lexer_t *l) { return l->remaining.length == 0; }

void update_pos(lexer_t *l, int n) {
  if (n < 0)
    return;
  l->eaten += n;
  for (size_t i = 0; i < (size_t)n && i < l->remaining.length; i++) {
    char c = l->remaining.contents[i];
    switch (c) {
    case '\n': {
      l->current_loc.line++;
      l->current_loc.col = 1;
    } break;
    default: {
      l->current_loc.col++;
    } break;
    }
  }
}

void lexer_skip(lexer_t *l) {
  while (true) {
    if (is_done(l))
      break;
    bool could_skip = false;

    for (size_t i = 0; i < l->rules.count; i++) {
      lexer_rule_t rule = l->rules.data[i];
      if (rule.kind != SKIP)
        continue;
      string_view_t nr;
      bool res = sv_matches_exact(rule.regexp, l->remaining, &nr);
      if (res) {
        update_pos(l, l->remaining.length - nr.length);
        l->remaining = nr;
        could_skip = true;

        break;
      }
    }
    if (!could_skip)
      break;
  }
}

bool is_next(lexer_t *l) {
  if (is_done(l))
    return true;
  lexer_t cpy = *l;
  lexer_skip(&cpy);
  return is_done(&cpy);
}

string_view_t location_to_sv(location_t loc) {
  char res[1024] = {0};
  sprintf(res, "%s:%d:%d", loc.filename, loc.line, loc.col);
  string_view_t r = {0};
  int l = strlen(res);
  r.length = l;
  r.contents = malloc(l + 1);
  if (r.contents) {
    strcpy(r.contents, res);
  }
  return r;
}

void print_error(FILE *f, lexer_t *l, string_view_t error_message) {
  int until_end_of_line = length_until(l->remaining, '\n');
  if (until_end_of_line < 0)
    until_end_of_line = l->remaining.length;
  string_view_t loc = location_to_sv(l->current_loc);
  fprintf(f, SF " " SF ": %.*s\n", SA(loc), SA(error_message),
          until_end_of_line, l->remaining.contents);
  free(loc.contents);
  int space_len = error_message.length + loc.length + 3;
  for (int i = 0; i < space_len; i++) {
    fprintf(f, " ");
  }
  for (int i = 0; i < until_end_of_line; i++) {
    fprintf(f, "^");
  }
  fprintf(f, "\n");
}

token_t error_token() { return (token_t){{0}, {0}, -1}; }

token_t next(lexer_t *l) {
  lexer_skip(l);
  fflush(stdout);
  location_t tmp = l->current_loc;
  for (size_t i = 0; i < l->rules.count; i++) {
    lexer_rule_t rule = l->rules.data[i];
    if (rule.kind == SKIP)
      continue;
    string_view_t sv;
    bool res = sv_matches_exact(rule.regexp, l->remaining, &sv);
    if (!res)
      continue;
    if (rule.kind == GOOD) {
      int len = l->remaining.length - sv.length;
      token_t tok = {tmp, {l->remaining.contents, len}, rule.as.good};
      update_pos(l, len);
      l->remaining = sv;
      return tok;
    }
    print_error(stderr, l, rule.as.error);
    return error_token();
  }
  for (size_t i = 0; i < l->rules.size; i++) {
    lexer_rule_t rule = l->rules.data[i];
    if (rule.kind != BAD)
      continue;
  }

  // print_error(stderr, l, SV("[Syntax Error] No rule matching"));
  return error_token();
}

#define RULES_INIT 64

lexer_rules_t new_rules(void) {
  return (lexer_rules_t){malloc(RULES_INIT * sizeof(lexer_rule_t)), 0,
                         RULES_INIT};
}

bool is_error_tok(token_t tok) { return tok.kind < 0; }