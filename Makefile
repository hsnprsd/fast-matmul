.PHONY: bench

ITER=0

all: compile
	./a.out

compile: ${ITER}.c
	gcc -march=native -O2 ${ITER}.c
