compress.exe: compress.o
	gcc $? -o compress

compress.o: compress.c
	gcc -c $? -o compress.o

run:
	@.\compress