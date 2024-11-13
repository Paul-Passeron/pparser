/**
 * regexp.c
 * Copyright (C) 2024 Paul Passeron
 * REGEXP source file
 * Paul Passeron <paul.passeron2@gmail.com>
 */

#include "../include/regexp.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void match_single(const char *pattern, const char *string, bool *worked,
                  int *pattern_advanced) {
  *pattern_advanced = 1;
  char to_test = *pattern;
  pattern++;
  if (to_test == '\\') {
    // handle escaping
    switch (*pattern) {
    case '*':
      to_test = '*';
      break;
    case '[':
      to_test = '[';
      break;
    case ']':
      to_test = ']';
      break;
    case ')':
      to_test = ')';
      break;
    case '(':
      to_test = '(';
      break;
    case '\\':
      to_test = '\\';
      break;
    case '?':
      to_test = '?';
      break;
    case 'n':
      to_test = '\n';
      break;
    case 't':
      to_test = '\t';
      break;
    case 'r':
      to_test = '\r';
      break;
    case 'b':
      to_test = '\b';
      break;
    case '0':
      to_test = 0;
      break;
    default:
      break;
    }
    *pattern_advanced = 2;
  }
  *worked = *string == to_test;
}

char get_actualchar(const char *str, int *length) {
  *length = 1;
  if (!*str)
    return 0;
  char ch = *str;
  str++;
  if (ch == '\\') {
    *length = 2;
    switch (*str) {
    case '*':
      ch = '*';
      break;
    case '[':
      ch = '[';
      break;
    case ']':
      ch = ']';
      break;
    case ')':
      ch = ')';
      break;
    case '(':
      ch = '(';
      break;
    case '\\':
      ch = '\\';
      break;
    case '?':
      ch = '?';
      break;
    case 'n':
      ch = '\n';
      break;
    case 't':
      ch = '\t';
      break;
    case 'r':
      ch = '\r';
      break;
    case 'b':
      ch = '\b';
      break;
    case '0':
      ch = 0;
      break;
    default:
      break;
    }
  }
  return ch;
}

void regexp(const char *pattern, const char *string, bool *pattern_finished,
            int *string_matched) {
  const char *src_string = string;
  *pattern_finished = false;
  *string_matched = 0;
  while (*pattern && *string) {
    char c = *pattern;
    if (c == '(') {
      pattern++;
      const char *pat = pattern;
      while (*pattern && *pattern != ')') {
        pattern++;
      }
      char *sub_pat = (malloc(pattern - pat + 1));
      memcpy(sub_pat, pat, pattern - pat + 1);
      sub_pat[pattern - pat] = 0;
      pattern++;
      while (true) {
        bool finished = false;
        int strmatched = 0;
        regexp(sub_pat, string, &finished, &strmatched);
        if (!finished || !*string) {
          break;
        }
        string += strmatched;
        *string_matched += strmatched;
      }
      free(sub_pat);

      continue;
    } else if (c == '?') {
      pattern++;
      string++;
      *string_matched += 1;
    } else if (c == '*') {
      if (!*(pattern + 1)) {
        *pattern_finished = true;
        *string_matched = strlen(src_string);
        return;
      }
      uint32_t string_length = strlen(string);
      for (size_t j = 0; j < string_length; j++) {
        bool finished = false;
        int string_match = 0;
        regexp(pattern + 1, string + j, &finished, &string_match);
        if (finished) {
          *pattern_finished = true;
          *string_matched += string_match + j;
          return;
        }
        if (!*(string + j + 1)) {
          *pattern_finished = false;
          return;
        }
      }
    } else if (c == '[') {
      bool found = false;
      pattern++;
      if (!*pattern || *pattern == ']') {
        exit(1);
      }
      do {
        if (found) {
          pattern++;
          continue;
        }
        int l = 0;
        char start = get_actualchar(pattern, &l);
        pattern += l;
        char delim = *pattern;
        if (!delim) {
          exit(1);
        }
        if (start != delim && start == '-') {
          if (*string == '-') {
            pattern--;
            found = true;
            *string_matched += 1;
            string++;
            continue;
          }
          return;
        }
        if (delim != '-') {
          exit(1);
        }
        pattern++;
        char end = get_actualchar(pattern, &l);
        pattern += l;
        if (!end || end == ']') {
          exit(1);
        }
        char s = *string;
        if (s >= start && s <= end) {
          found = true;
          *string_matched += 1;
          string++;
          if (!*pattern) {
            *pattern_finished = true;
          }
        }
      } while (*pattern && *pattern != ']');
      if (!found) {
        return;
      }
      pattern++;
    } else {
      bool worked;
      int amount;
      match_single(pattern, string, &worked, &amount);
      if (worked) {
        string++;
        *string_matched += 1;
        pattern += amount;
      } else {
        return;
      }
    }
  }
  while (*pattern == '(') {
    while (*pattern && *pattern != ')') {
      pattern++;
    }
    pattern++;
  }
  if (!*pattern) {
    *pattern_finished = true;
  }
}

bool matches_exact(const char *pattern, char *string, char **rest) {
  *rest = string;
  bool res;
  int l;
  regexp(pattern, string, &res, &l);
  if (res) {
    *rest = string + l;
  }
  return res;
}
