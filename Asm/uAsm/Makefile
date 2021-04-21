#
#	Commands
#

# Compilers
CC=gcc

# Misc
ECHO=@echo

#
#	Misc Variables
#

# C Flags
CLIBS=-lm -lncurses
CFLAGS=-Wall -std=c99 -Wextra -pedantic
CDEBUGFLAGS=-ggdb

#
#	Main Target
#
main: tokenizer


tokenizer: lex.yy.c
	$(CC) -o tokenizer lex.yy.c -lfl

lex.yy.c: tokenizer.l
	flex tokenizer.l 

.PHONY: clean realclean tokenize parse

clean:
	-rm *.o
	-rm *.obj
	-rm lex.yy.c

realclean: clean
	-rm tokenizer
