build:	9cc.c
	gcc -o 9cc 9cc.c

9cc: 9cc.c
	
test: 9cc
	./test.sh

clean:
	rm -f 9cc *.o *- tmp*
