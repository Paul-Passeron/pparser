/**
 * pparser_parser.c
 * Copyright (C) 2024 Paul Passeron
 * PPARSER_PARSER source file
 * Paul Passeron <paul.passeron2@gmail.com>
 */

#include "../include/pparser_parser.h"
#include <stdio.h>
#include <stdlib.h>

ast_elem_t elem_parser(ppl_t *l) {
  ast_elem_t res = {0};
  token_t tok = pparser_lexer_next(l);
  if (is_error_tok(tok)) {
    print_error(stdout, &l->l, SV("Could not parse_element"));
    exit(1);
  }

  if (tok.kind == RULE) {
    res.kind = EK_RULE;
    res.rule = tok.lexeme;
    sv_consume(&res.rule);
    return res;
  }

  if (tok.kind == STRLIT) {
    res.kind = EK_LEXEME;
    res.rule = tok.lexeme;
    // remove quotes
    sv_consume(&res.rule);
    res.rule.length--;
    return res;
  }

  if (tok.kind == OPEN_BRA) {
    tok = pparser_lexer_next(l);
    if (is_error_tok(tok)) {
      print_error(stdout, &l->l, SV("Could not parse_element"));
      exit(1);
    }
    if (tok.kind != CODE) {
      print_error(stdout, &l->l,
                  SV("Could not parse_element: expected identifier after open "
                     "bracket"));
      exit(1);
    }
    res.kind = EK_KIND;
    res.rule = tok.lexeme;

    tok = pparser_lexer_next(l);
    if (is_error_tok(tok)) {
      print_error(stdout, &l->l, SV("Could not parse_element"));
      exit(1);
    }
    if (tok.kind != CLOSE_BRA) {
      print_error(stdout, &l->l,
                  SV("Could not parse_element: missing close bracket"));
      exit(1);
    }
    return res;
  }
  print_error(stdout, &l->l, SV("Could not parse_element: end"));

  exit(1);
}

ast_rule_candidate_t candidate_parser(ppl_t *l) {
  ast_rule_candidate_t res = {0};
  token_t tok = pparser_peek(l);
  ast_elem_t elems[1024] = {0};
  int elems_count = 0;
  while (tok.kind != BIG_ARROW) {
    ast_elem_t elem = elem_parser(l);
    elems[elems_count++] = elem;
    tok = pparser_peek(l);
    if (is_error_tok(tok)) {
      printf("Could not parse rule candidate.\n");
      exit(1);
    }
  }
  tok = pparser_lexer_next(l);
  if (is_error_tok(tok)) {
    printf("Could not parse rule candidate.\n");
    exit(1);
  }
  if (tok.kind != BIG_ARROW) {
    printf("Expected '=>' after rule candidate\n");
    exit(1);
  }

  ast_elem_t *res_elems = malloc(sizeof(ast_elem_t) * elems_count);
  for (int i = 0; i < elems_count; i++) {
    res_elems[i] = elems[i];
  }
  res.elems = res_elems;

  res.elems_count = elems_count;

  tok = pparser_lexer_next(l);

  if (is_error_tok(tok)) {
    print_error(stdout, &l->l,
                SV("Could not parse rule candidate: missing action"));
    exit(1);
  }
  if (tok.kind != OPEN_BRA) {
    print_error(stdout, &l->l,
                SV("Could not parse rule candidate: missing action"));
    exit(1);
  }
  tok = pparser_lexer_next(l);
  if (is_error_tok(tok)) {
    print_error(stdout, &l->l,
                SV("Could not parse rule candidate: missing action"));
    exit(1);
  }
  if (tok.kind != CODE) {
    print_error(stdout, &l->l,
                SV("Could not parse rule candidate: missing action"));
    exit(1);
  }
  res.action = tok.lexeme;
  tok = pparser_lexer_next(l);
  if (is_error_tok(tok)) {
    print_error(stdout, &l->l,
                SV("Could not parse rule candidate: missing action"));
    exit(1);
  }
  if (tok.kind != CLOSE_BRA) {
    print_error(stdout, &l->l,
                SV("Could not parse rule candidate: missing close bracket "
                   "after rule candidate"));
    exit(1);
  }

  return res;
}

ast_rule_t rule_parser(ppl_t *l) {
  ast_rule_t res = {0};
  token_t tok = pparser_lexer_next(l);
  if (is_error_tok(tok)) {
    print_error(stdout, &l->l, SV("Expected identifier for rule name"));
    exit(1);
  }
  if (tok.kind != IDENTIFIER) {
    // printf("TOK.KIND IS %s\n", human_token_kind(tok.kind));
    print_error(stdout, &l->l, SV("Expected identifier for rule name"));
    exit(1);
  }
  res.name = tok.lexeme;
  tok = pparser_lexer_next(l);
  if (is_error_tok(tok)) {
    printf("Could not parse rule.\n");
    exit(1);
  }
  if (tok.kind != COLON) {
    printf("Expected colon after rule name\n");
    exit(1);
  }
  tok = pparser_peek(l);
  if (is_error_tok(tok)) {
    printf("Could not parse rule.\n");
    exit(1);
  }
  if (tok.kind == DIRECTIVE) {
    res.is_list = 1;
    if (!sv_eq(tok.lexeme, SV("@list"))) {
      printf("This is: " SF "\n", SA(tok.lexeme));
      printf("Only @list is supported for now: \n");
      exit(1);
    }

    (void)pparser_lexer_next(l);
    tok = pparser_lexer_next(l);
    if (is_error_tok(tok)) {
      printf("no more\n");
      exit(1);
    }
    if (!sv_eq(tok.lexeme, SV("elem"))) {
      printf("No elem in @list");
      exit(1);
    }
    tok = pparser_lexer_next(l);
    if (is_error_tok(tok)) {
      printf("no more\n");
      exit(1);
    }
    if (tok.kind != EQ) {
      printf("Expected = after elem in @list\n");
      exit(1);
    }
    ast_elem_t elem = elem_parser(l);
    res.l.elem = elem;
    ppl_t old = *l;
    tok = pparser_lexer_next(l);
    int sep = 1;
    if (!sv_eq(tok.lexeme, SV("separator"))) {
      sep = 0;
      *l = old;
    }
    if (sep) {
      tok = pparser_lexer_next(l);
      if (is_error_tok(tok)) {
        printf("no more\n");
        exit(1);
      }
      if (tok.kind != EQ) {
        printf("Expected = after separator in @list\n");
        exit(1);
      }
      res.l.sep = elem_parser(l);
    }
    res.l.has_sep = sep;
    return res;
  }
  ast_rule_candidate_t candidates[1024] = {0};

  int n_candidates = 0;
  if (tok.kind != VERT) {
    ast_rule_candidate_t candidate = candidate_parser(l);

    candidates[n_candidates++] = candidate;
  }

  while (tok.kind == VERT) {
    tok = pparser_lexer_next(l);

    ast_rule_candidate_t candidate = candidate_parser(l);
    candidates[n_candidates++] = candidate;
    tok = pparser_peek(l);
    if (is_error_tok(tok)) {
      break;
    }
  }
  ast_rule_candidate_t *res_candidates =
      malloc(sizeof(ast_rule_candidate_t) * n_candidates);
  for (int i = 0; i < n_candidates; i++) {
    res_candidates[i] = candidates[i];
  }
  res.candidates = res_candidates;

  res.candidates_count = n_candidates;
  return res;
}

ast_program_t program_parser(ppl_t *l) {
  ast_program_t res = {0};

  token_t tok = pparser_peek(l);
  if (tok.kind == OPEN_BRA) {
    tok = pparser_lexer_next(l);
    tok = pparser_lexer_next(l);
    if (tok.kind != CODE) {
      printf("Expected preambule code after open bracket\n");
      exit(1);
    }
    string_view_t preambule = tok.lexeme;
    tok = pparser_lexer_next(l);
    if (tok.kind != CLOSE_BRA) {
      printf("Expected close bracket after preambule code\n");
      exit(1);
    }
    res.preambule = preambule;
  }
  size_t rules_cap = 16;
  size_t rules_count = 0;
  ast_rule_t *rules = malloc(rules_cap * sizeof(ast_rule_t));
  while (!is_next(&l->l)) {
    ast_rule_t r = rule_parser(l);

    if (rules_cap == rules_count) {
      rules = realloc(rules, rules_cap * 2 * sizeof(ast_rule_t));
      rules_cap *= 2;
    }
    rules[rules_count++] = r;
  }
  rules = realloc(rules, rules_count * sizeof(ast_rule_t));
  res.rules = rules;
  res.rules_count = rules_count;

  return res;
}