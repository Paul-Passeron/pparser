/**
 * string_view.h
 * Copyright (C) 2024 Paul Passeron
 * STRING_VIEW header file
 * Paul Passeron <paul.passeron2@gmail.com>
 */

#ifndef STRING_VIEW_H
#define STRING_VIEW_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

typedef struct string_view_t {
  char *contents;
  size_t length;
} string_view_t;

#define SV(s)                                                                  \
  (string_view_t) { (s), sizeof(s) - 1 }

#define SF "%.*s"

#define SA(s) ((int)(s).length), (s).contents

void sv_consume(string_view_t *s);

char *sv_to_cstr(string_view_t s);

// TODO: custom implementation that does not rely on sv_to_sctr.

bool sv_matches_exact(string_view_t pattern, string_view_t string,
                      string_view_t *rest);

int length_until(string_view_t s, char c);

string_view_t from_file(FILE *f);

bool sv_eq(string_view_t a, string_view_t b);

#endif // STRING_VIEW_H
