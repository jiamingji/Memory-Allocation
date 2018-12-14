.PHONY: all main test

all: main test

test: test_bestfit test_firstfit

test_bestfit: bestfit_lib.o solution_bestfit.o
	gcc test.c -g $^ -o $@

test_firstfit: firstfit_lib.o solution_firstfit.o
	gcc test.c -g $^ -o $@

bestfit_lib.o:
	gcc -c my_mem.c -g -D BESTFIT -o bestfit_lib.o

firstfit_lib.o:
	gcc -c my_mem.c -g -D FIRSTFIT -o firstfit_lib.o

main: bestfit firstfit

bestfit: bestfit_lib.o
	gcc main.c $^ -o bestfit

firstfit: firstfit_lib.o
	gcc main.c $^ -o firstfit

clean:
	rm -f *_lib.o
	rm -f firstfit
	rm -f bestfit
	rm -f test_bestfit
	rm -f test_firstfit
	rm -f temp_solution_file.txt
	rm -f temp_my_file.txt