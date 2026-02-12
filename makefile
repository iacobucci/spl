MAKEFLAGS += --silent

SRCS := $(wildcard *.c)

main.out: $(SRCS)
	make compile
	./main.out

compile:
	gcc -g $(SRCS) -o main.out

dev:
	make main.out
	ls *.c *.h | entr make main.out

.PHONY: dev compile main.out
