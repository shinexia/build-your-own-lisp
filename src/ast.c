#include "ast.h"
#include <stdio.h>
#include <stdlib.h>

void print_ast_atom(ast_atom_t *atom) { fprintf(stdout, "%s", atom->content); }

void print_ast_list(ast_list_t *list) {
  fprintf(stdout, "(");
  for (int i = 0; i < list->len; i++) {
    if (i > 0) {
      fprintf(stdout, " ");
    }
    print_ast_expr(list->exprs[i]);
  }
  fprintf(stdout, ")");
}

void print_ast_expr(ast_expr_t *expr) {
  if (expr->type == TOK_ATOM) {
    print_ast_atom(expr->atom);
  } else if (expr->type == TOK_LIST) {
    print_ast_list(expr->list);
  } else {
    fprintf(stderr, "invalid type: %d", expr->type);
  }
}

void print_ast(ast_t *ast) {
  if (NULL == ast) {
    fprintf(stdout, "NULL\n");
    return;
  }
  if (0 == ast->len) {
    fprintf(stdout, "ast.len=%d\n", ast->len);
    return;
  }
  for (int i = 0; i < ast->len; i++) {
    print_ast_expr(ast->exprs[i]);
    fprintf(stdout, "\n");
  }
}

void cleanup_ast_atom(ast_atom_t *atom) {
  if (NULL == atom) {
    return;
  }
  free(atom->content);
  free(atom);
}

void cleanup_ast_list(ast_list_t *list) {
  if (NULL == list) {
    return;
  }
  for (int i = 0; i < list->len; i++) {
    cleanup_ast_expr(list->exprs[i]);
  }
  free(list->exprs);
  free(list);
}

void cleanup_ast_expr(ast_expr_t *expr) {
  if (NULL == expr) {
    return;
  }
  if (expr->type == TOK_LIST) {
    cleanup_ast_list(expr->list);
  } else if (expr->type == TOK_ATOM) {
    cleanup_ast_atom(expr->atom);
  } else {
    fprintf(stderr, "invalid expr type: %d\n", expr->type);
  }
  free(expr);
}

void cleanup_ast(ast_t *ast) {
  if (NULL == ast) {
    return;
  }
  for (int i = 0; i < ast->len; i++) {
    cleanup_ast_expr(ast->exprs[i]);
  }
  free(ast->exprs);
  free(ast);
}
