#include <stdio.h>
#include <stdlib.h>

int main( int argc, char **argv )
{
	if ( argc != 2 ) {
		fprintf( stderr, "wrong argument number\n" );
		return 1;
	}

	printf( ".intel_syntax noprefix\n" );
	printf( ".global main\n" );
	printf( "main:\n" );
	printf( "	MOV	rax, %d\n", atoi( argv[1] ) );
	printf( "	RET\n");
	return 0;
}

