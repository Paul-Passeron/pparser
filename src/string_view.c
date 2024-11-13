/**
 * string_view.c
 * Copyright (C) 2024 Paul Passeron
 * STRING_VIEW source file
 * Paul Passeron <paul.passeron2@gmail.com>
 */

#include "../include/string_view.h"
#include "../include/regexp.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void sv_consume(string_view_t *s) {
  if (s->length <= 1)
    return;
  s->contents++;
  s->length--;
}

char *sv_to_cstr(string_view_t s) {
  char *res = malloc(s.length + 1);
  for (size_t i = 0; i < s.length; i++) {
    res[i] = s.contents[i];
  }
  res[s.length] = 0;
  return res;
}

bool sv_matches_exact(string_view_t pattern, string_view_t string,
                      string_view_t *rest) {
  char pat[1024] = {0};
  memcpy(pat, pattern.contents, pattern.length);
  char str_backup[1024] = {0};
  char *str = str_backup;
  bool should_free = false;
  if (string.length > 1024) {
    str = malloc(string.length + 1);
    should_free = true;
  }
  memcpy(str, string.contents, string.length);
  str[string.length] = 0;
  char *r;
  bool res = matches_exact(pat, str, &r);
  int l = strlen(r);
  *rest = (string_view_t){string.contents + (r - str), l};
  if (should_free) {
    free(str);
  }
  return res;
}

int length_until(string_view_t s, char c) {
  for (size_t i = 0; i < s.length; i++) {
    if (s.contents[i] == c)
      return i;
  }
  return -1;
}

string_view_t from_file(FILE *f) {
  long current = ftell(f);
  fseek(f, 0, SEEK_END);
  long size = ftell(f);
  fseek(f, 0, SEEK_SET);
  char *str = malloc(size + 1);
  fread(str, 1, size, f);
  fseek(f, current, SEEK_SET);
  return (string_view_t){str, size};
}

bool sv_eq(string_view_t a, string_view_t b) {
  if (a.length != b.length)
    return false;
  return !memcmp(a.contents, b.contents, a.length);
}