CFLAGS+=-std=c99 -Wall -g
LDFLAGS+=-ledit

SRCS := src/mpc.h \
	src/mpc.c \
	src/util.h \
	src/util.c \
	src/status.h \
	src/status.c \
	src/grammar.h \
	src/grammar_txt.c \
	src/ast.h \
	src/ast.c \
	src/parser.h \
	src/parser.c \
	src/eval.h \
	src/eval.c \
	src/lisp.c

DIST_EXE=lisp

$(DIST_EXE): $(SRCS)
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

src/grammar_txt.c: src/grammar.txt
	cd src; xxd -i grammar.txt > grammar_txt.c

.PHONY: mpc
mpc:
	ln -sf ../thirdparty/mpc/mpc.h src
	ln -sf ../thirdparty/mpc/mpc.c src

.PHONY: clean
clean:
	rm -fr src/*.o $(DIST_EXE) src/grammar_txt.c
