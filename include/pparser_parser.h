/**
 * pparser_parser.h
 * Copyright (C) 2024 Paul Passeron
 * PPARSER_PARSER header file
 * Paul Passeron <paul.passeron2@gmail.com>
 */

// File created by the new_file tool !

#ifndef PPARSER_PARSER_H
#define PPARSER_PARSER_H
#include "../include/ast.h"
#include "../include/pparser_lexer.h"

ast_program_t program_parser(ppl_t *l);

#endif // PPARSER_PARSER_H
