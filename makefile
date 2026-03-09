MAKEFLAGS += --silent

SRCS := $(wildcard *.c)

# development

main.out: $(SRCS)
	make compile
	./main.out
	date

compile:
	gcc -g -O2 $(SRCS) examples/main.c -o main.out

dev:
	ls *.c *.h **/*.c | entr -c make main.out

# library

OBJS := $(SRCS:.c=.o)

LIB := build/libspl.a

all: $(LIB)

$(LIB): $(OBJS)
	mkdir -p build
	ar rcs $@ $^

clean:
	rm -rf build *.o

# profiling

sanitize:
	gcc -O0 -g -fsanitize=address $(SRCS) -o main.out
	./main.out

check:
	distrobox enter debian -- valgrind --leak-check=full ./main.out

.PHONY: dev compile sanitize compile check all
