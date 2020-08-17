#include "parser.h"
#include "mpc.h"
#include "util.h"

struct lisp_parser {
  mpc_parser_t *Number;
  mpc_parser_t *Symbol;
  mpc_parser_t *Atom;
  mpc_parser_t *List;
  mpc_parser_t *Expr;
  mpc_parser_t *Lisp;
};

lisp_parser_t *create_lisp_parser(const char *input, status_t *status) {
  status->code = 0;
  mpc_parser_t *Number = mpc_new("number");
  mpc_parser_t *Symbol = mpc_new("symbol");
  mpc_parser_t *Atom = mpc_new("atom");
  mpc_parser_t *List = mpc_new("list");
  mpc_parser_t *Expr = mpc_new("expr");
  mpc_parser_t *Lisp = mpc_new("lisp");

  mpc_err_t *ret = mpca_lang(MPCA_LANG_DEFAULT, input,
                             Number, Symbol, Atom, List, Expr, Lisp);

  if (NULL != ret) {
    status->code = 1;
    char *err = mpc_err_string(ret);
    sprintf(status->message, "parse grammar error: %s", err);
    free(err);
    mpc_delete(Number);
    mpc_delete(Symbol);
    mpc_delete(Atom);
    mpc_delete(List);
    mpc_delete(Expr);
    mpc_delete(Lisp);
    mpc_err_delete(ret);
    return NULL;
  }

  lisp_parser_t *p = (lisp_parser_t *)malloc(sizeof(struct lisp_parser));

  p->Number = Number;
  p->Symbol = Symbol;
  p->Atom = Atom;
  p->List = List;
  p->Expr = Expr;
  p->Lisp = Lisp;

  return p;
}

void cleanup_lisp_parser(lisp_parser_t *parser) {
  if (NULL == parser) {
    return;
  }
  mpc_cleanup(6, parser->Number, parser->Symbol, parser->Atom, parser->List,
              parser->Expr, parser->Lisp);
  free(parser);
}

static ast_t *construct_ast(mpc_ast_t *ast, status_t *status);

ast_t *lisp_parse(lisp_parser_t *parser, const char *input, status_t *status) {
  status->code = 0;
  mpc_result_t r;
  if (mpc_parse("<stdin>", input, parser->Lisp, &r)) {
    ast_t *ast = construct_ast(r.output, status);
    mpc_ast_delete(r.output);
    return ast;
  } else {
    status->code = 1;
    char *err = mpc_err_string(r.error);
    sprintf(status->message, "error: %s", err);
    free(err);
    mpc_err_delete(r.error);
    return NULL;
  }
}

static ast_expr_t *construct_ast_expr(mpc_ast_t *ast, status_t *status) {
  if (strstr(ast->tag, "expr")) {
    if (ast->children_num == 0) {
      int tag;
      if (strstr(ast->tag, "number")) {
        tag = TOK_NUMBER;
      } else if (strstr(ast->tag, "symbol")) {
        tag = TOK_SYMBOL;
      } else {
        status->code = 1;
        sprintf(status->message, "invalid tag: %s, contents: %s", ast->tag, ast->contents);
        return NULL;
      }
      ast_atom_t *atom = (ast_atom_t *)malloc(sizeof(ast_atom_t));
      atom->tag = tag;
      int len = strlen(ast->contents);
      atom->cap = len + 1;
      atom->content = (char *)malloc(atom->cap);
      memcpy(atom->content, ast->contents, len);
      atom->content[len] = '\0';
      ast_expr_t *expr = (ast_expr_t *)malloc(sizeof(ast_expr_t));
      expr->type = TOK_ATOM;
      expr->atom = atom;
      status->code = 0;
      return expr;
    }
    ast_list_t *list = (ast_list_t *)malloc(sizeof(ast_list_t));
    list->len = 0;
    list->cap = ast->children_num;
    list->exprs = (ast_expr_t **)malloc(sizeof(ast_expr_t *) * list->cap);
    for (int i = 0; i < ast->children_num; i++) {
      ast_expr_t *expr = construct_ast_expr(ast->children[i], status);
      if (!is_status_ok(status)) {
        cleanup_ast_list(list);
        return NULL;
      }
      if (NULL != expr) {
        list->exprs[list->len] = expr;
        list->len++;
      }
    }
    ast_expr_t *expr = (ast_expr_t *)malloc(sizeof(ast_expr_t));
    expr->type = TOK_LIST;
    expr->list = list;
    status->code = 0;
    return expr;
  }
  for (int i = 0; i < ast->children_num; i++) {
    ast_expr_t *expr = construct_ast_expr(ast->children[i], status);
    if (!is_status_ok(status)) {
      return NULL;
    }
    if (NULL != expr) {
      return expr;
    }
  }
  return NULL;
}

static ast_t *construct_ast(mpc_ast_t *ast, status_t *status) {
  ast_expr_t *expr = construct_ast_expr(ast, status);
  if (!is_status_ok(status)) {
    return NULL;
  }
  ast_t *os = (ast_t *)malloc(sizeof(ast_t));
  if (NULL != expr) {
    os->len = 1;
    os->cap = 1;
    os->exprs = (ast_expr_t **)malloc(sizeof(ast_expr_t *) * os->cap);
    os->exprs[0] = expr;
  } else {
    os->len = 0;
    os->exprs = NULL;
  }
  return os;
}
