#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "status.h"

struct lisp_parser;

typedef struct lisp_parser lisp_parser_t;

lisp_parser_t* create_lisp_parser(const char* input, status_t* status);

void cleanup_lisp_parser(lisp_parser_t* parser);

ast_t* lisp_parse(lisp_parser_t* parser, const char* input, status_t* status);

#endif  // !PARSER_H
