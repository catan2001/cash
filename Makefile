#MakeFile for CASH!

.PHONY: all
all: cash.o
	gcc cash.o -o cash
cash.o: cash.c
	gcc -c cash.c
.PHONY: clean
clean: cash.o cash
	rm cash.o
	rm cash
.PHONY: run
run: cash
	./cash
