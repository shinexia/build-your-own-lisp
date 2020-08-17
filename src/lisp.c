#include <editline/history.h>
#include <editline/readline.h>
#include <stdio.h>
#include "eval.h"
#include "grammar.h"
#include "mpc.h"
#include "parser.h"
#include "status.h"

int main(int argc, char **argv) {
  status_t status;
  lisp_parser_t *parser = create_lisp_parser((const char *)grammar_txt, &status);

  if (!is_status_ok(&status)) {
    fprintf(stdout, "ERROR: code=%d, message=%s\n", status.code, status.message);
    exit(1);
  }

  puts("lisp Version 0.0.1");
  puts("Press Ctrl+c to Exit\n");

  while (1) {
    char *input = readline("lisp>");
    add_history(input);
    if (!strcmp(input, "exit")) {
      fprintf(stdout, "exit\n");
      exit(0);
    }
    ast_t *ast = lisp_parse(parser, input, &status);
    if (is_status_ok(&status)) {
      long result = eval_ast(ast, &status);
      if (!is_status_ok(&status)) {
        fprintf(stdout, "ERROR: code=%d, message=%s\n", status.code, status.message);
      } else {
        print_ast(ast);
        fprintf(stdout, "=%li\n", result);
      }
    } else {
      fprintf(stdout, "ERROR: %s", status.message);
    }
    cleanup_ast(ast);
    free(input);
  }
  cleanup_lisp_parser(parser);
}
