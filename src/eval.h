#ifndef EVAL_H
#define EVAL_H

#include "ast.h"
#include "status.h"

char* read_symbol(ast_expr_t* expr, status_t* status);
long read_number(ast_expr_t* atom, status_t* status);

long eval_ast_list(ast_list_t* list, status_t* status);
long eval_ast_expr(ast_expr_t* expr, status_t* status);

long eval_ast(ast_t* ast, status_t* status);

#endif  // !EVAL_H
