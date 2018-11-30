#include <stdio.h>
#include <stdlib.h>

int main( int argc, char **argv )
{
	if ( argc != 2 ) {
		fprintf( stderr, "wrong argument number\n" );
		return 1;
	}

	char * p = argv[1];

	printf( ".intel_syntax noprefix\n" );
	printf( ".global main\n" );
	printf( "main:\n" );
	printf( "	MOV	rax, %ld\n", strtol( p, &p, 10 ) );

	while ( *p ) {
		if ( *p == '+' ) {
			p++;
			printf( "	ADD 	rax, %ld\n", strtol( p, &p, 10 ) );
			continue;
		}

		if ( *p == '-' ) {
			p++;
			printf( "	SUB	rax, %ld\n", strtol( p, &p, 10 ) );
			continue;
		}

		fprintf( stdout, "予期せぬ文字です: '%c'\n", *p );
		return 1;
	}


	printf( "	RET\n");
	return 0;
}

