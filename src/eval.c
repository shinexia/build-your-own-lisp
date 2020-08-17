#include "eval.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "stdio.h"

typedef long (*op_func)(long x, long y, status_t* status);

static long op_add(long x, long y, status_t* status) {
  status->code = 0;
  return x + y;
}

static long op_sub(long x, long y, status_t* status) {
  status->code = 0;
  return x - y;
}

static long op_mul(long x, long y, status_t* status) {
  status->code = 0;
  return x * y;
}

static long op_div(long x, long y, status_t* status) {
  if (y == 0) {
    status->code = 1;
    sprintf(status->message, "div by zero");
    return 0;
  }
  status->code = 0;
  return x / y;
}

static op_func get_op_func(const char* op, status_t* status) {
  status->code = 0;
  if (0 == strcmp(op, "+")) {
    return op_add;
  } else if (0 == strcmp(op, "-")) {
    return op_sub;
  } else if (0 == strcmp(op, "*")) {
    return op_mul;
  } else if (0 == strcmp(op, "/")) {
    return op_div;
  } else {
    status->code = 1;
    sprintf(status->message, "invalid op: %s", op);
    return NULL;
  }
}

char* read_symbol(ast_expr_t* expr, status_t* status) {
  if (TOK_ATOM != expr->type) {
    status->code = 1;
    sprintf(status->message, "expect a atom, but a list");
    return NULL;
  }
  ast_atom_t* atom = expr->atom;
  if (TOK_SYMBOL != atom->tag) {
    status->code = 1;
    sprintf(status->message, "expect a symbol, but tag=%d", atom->tag);
    return NULL;
  }
  return atom->content;
}

long read_number(ast_expr_t* expr, status_t* status) {
  if (TOK_ATOM == expr->type) {
    ast_atom_t* atom = expr->atom;
    if (TOK_NUMBER == atom->tag) {
      errno = 0;
      long n = atol(atom->content);
      if (errno) {
        status->code = 1;
        sprintf(status->message, "parse number failed, errno: %d, contents: %s", errno, atom->content);
        return 0;
      }
      return n;
    }
    status->code = 0;
    sprintf(status->message, "must be a number, but: %s", atom->content);
    return 0;
  } else if (TOK_LIST == expr->type) {
    return eval_ast_list(expr->list, status);
  } else {
    status->code = 1;
    sprintf(status->message, "invalid type: %d", expr->type);
    return 0;
  }
}

long eval_ast_list(ast_list_t* list, status_t* status) {
  if (list->len <= 0) {
    status->code = 1;
    sprintf(status->message, "empty list, len=%d", list->len);
    return 0;
  }
  if (list->len == 1) {
    return read_number(list->exprs[0], status);
  }
  const char* op = read_symbol(list->exprs[0], status);
  if (!is_status_ok(status)) {
    return 0;
  }
  if (list->len == 2) {
    if (0 == strcmp(op, "+")) {
      return read_number(list->exprs[1], status);
    } else if (0 == strcmp(op, "-")) {
      return -read_number(list->exprs[1], status);
    } else {
      status->code = 1;
      sprintf(status->message, "op: %s does not support single target", op);
      return 0;
    }
  }
  op_func func = get_op_func(op, status);
  if (!is_status_ok(status)) {
    return 0;
  }
  long init = read_number(list->exprs[1], status);
  if (!is_status_ok(status)) {
    return 0;
  }
  for (int i = 2; i < list->len; i++) {
    long next = read_number(list->exprs[i], status);
    if (!is_status_ok(status)) {
      return 0;
    }
    init = func(init, next, status);
    if (!is_status_ok(status)) {
      return 0;
    }
  }
  return init;
}

long eval_ast_expr(ast_expr_t* expr, status_t* status) {
  if (TOK_ATOM == expr->type) {
    return read_number(expr, status);
  } else if (TOK_LIST == expr->type) {
    return eval_ast_list(expr->list, status);
  } else {
    status->code = 1;
    sprintf(status->message, "invalid type: %d", expr->type);
    return 0;
  }
}

long eval_ast(ast_t* ast, status_t* status) {
  if (ast->len == 0) {
    status->code = 1;
    sprintf(status->message, "empty exprs");
    return 0;
  }
  if (ast->len != 1) {
    status->code = 1;
    sprintf(status->message, "mult exprs not support, len=%d", ast->len);
    return 0;
  }
  return eval_ast_expr(ast->exprs[0], status);
}
