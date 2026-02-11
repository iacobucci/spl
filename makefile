MAKEFLAGS += --silent

main.out: main.c
	gcc -g main.c json.c parser.c rule.c utils.c -o main.out
	./main.out

compile:
	gcc -g main.c json.c parser.c rule.c utils.c -o main.out

dev:
	make main.out
	ls *.c | entr make main.out

.PHONY: dev compile
