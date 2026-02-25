MAKEFLAGS += --silent

SRCS := $(wildcard *.c)

main.out: $(SRCS)
	make compile
	./main.out
	date

compile:
	gcc -g -O2 $(SRCS) -o main.out

dev:
	ls *.c *.h | entr make main.out

sanitize:
	gcc -O0 -g -fsanitize=address $(SRCS) -o main.out
	./main.out

check:
	valgrind --leak-check=full ./main.out

# prlimit

.PHONY: dev compile main.out
