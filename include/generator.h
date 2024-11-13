/**
 * generator.h
 * Copyright (C) 2024 Paul Passeron
 * GENERATOR header file
 * Paul Passeron <paul.passeron2@gmail.com>
 */

#ifndef GENERATOR_H
#define GENERATOR_H

#include "ast.h"
void generate_parser_src(FILE *f, string_view_t name, ast_program_t prog);
void generate_parser_header(FILE *f, ast_program_t prog);

#endif // GENERATOR_H
