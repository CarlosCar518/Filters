test.exe: test.o
	gcc -g $? -o test

test.o: test.c
	gcc -g -c $? -o test.o

run:
	@.\test