.PHONY: bench

compile: main.c
	gcc -march=native -O1 main.c

bench: compile
	./a.out