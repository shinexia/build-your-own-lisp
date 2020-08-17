#ifndef AST_H
#define AST_H

#define TOK_ATOM 1
#define TOK_LIST 2
#define TOK_NUMBER 3
#define TOK_SYMBOL 4

struct ast_expr;

typedef struct ast_atom {
  int tag;
  int cap;
  char *content;
} ast_atom_t;

typedef struct ast_list {
  int len;
  int cap;
  struct ast_expr **exprs;
} ast_list_t;

typedef struct ast_expr {
  int type;
  union {
    struct ast_atom *atom;
    struct ast_list *list;
  };
} ast_expr_t;

typedef struct ast {
  int len;
  int cap;
  ast_expr_t **exprs;
} ast_t;

void print_ast_atom(ast_atom_t *atom);
void print_ast_list(ast_list_t *list);
void print_ast_expr(ast_expr_t *expr);
void print_ast(ast_t *ast);

void cleanup_ast_expr(ast_expr_t *expr);
void cleanup_ast_atom(ast_atom_t *atom);
void cleanup_ast_list(ast_list_t *list);
void cleanup_ast(ast_t *ast);

#endif  // !AST_H
