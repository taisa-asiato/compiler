#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

// トークンの型を表す値
enum {
	TK_NUM = 256,	// 整数トークン
	TK_EOF,		// 入力の終わりを表すトークン
};

// トークンの型
typedef struct {
	int ty;		//トークンの型
	int val;	//tyがTK_NUMの場合, その数値
	char * input;	// トークン文字列（エラーメッセージ用)
} Token;

//トークナイズした結果のトークン列はこの配列に保存する
// 100 以上のトークンは来ないものとする
Token tokens[100];

/* pがさしている文字列をトークンに分割する */
void tokenize( char * p ) 
{
	int i = 0;
	while ( *p ) {
		// 空白文字列をスキップ
		if ( isspace( *p ) ) {
			p++;
			continue;
		}

		// + or - 文字の場合, tyにはTK_NUMをセットしない
		if ( *p == '+' || *p == '-' ) {
			tokens[i].ty = *p;
			tokens[i].input = p;
			i++;
			p++;
			continue;
		}

		// *pが数字の場合
		if ( isdigit( *p ) ) {
			tokens[i].ty = TK_NUM;
			tokens[i].input = p;
			//　数字の場合, pの数字が続くアドレスまでをtokens[i].valに格納する
			// &pには，続く文字列が数字でない場合のエンドアドレスが格納される.
			tokens[i].val = strtol( p, &p, 10 );
			i++;
			continue;
		}

		fprintf( stderr, "トークナイズできません. :%s\n", p );
		exit( 1 );
	}
	tokens[i].ty = TK_EOF;
	tokens[i].input = p;
}


// エラー報告用の関数
void error( int i )
{
	fprintf( stderr, "予期せぬトークンです: %s\n", tokens[i].input );
	exit( 1 );
}

int main( int argc, char **argv )
{
	if ( argc != 2 ) {
		fprintf( stderr, "引数の個数が正しくありません.\n" );
		return 1;
	}


	// トークナイズする
	tokenize( argv[1] );


	// アセンブリ前半部分を出力する.
	printf( ".intel_syntax noprefix\n" );
	printf( ".global main\n" );
	printf( "main:\n" );

	// 式の最初は数値のでなければならないので,それをチェック
	if ( tokens[0].ty != TK_NUM ) { error( 0 ); }
	printf( "	MOV	rax, %d\n", tokens[0].val );

	int i = 1;
	while ( tokens[i].ty != TK_EOF ) {
		if ( tokens[i].ty == '+' ) {
			i++;
			if ( tokens[i].ty != TK_NUM ) { error( i ); }
			printf( "	ADD 	rax, %d\n", tokens[i].val );
			i++;
			continue;
		}

		if ( tokens[i].ty == '-' ) {
			i++;
			if ( tokens[i].ty != TK_NUM ) { error( i ); }
			printf( "	SUB	rax, %d\n", tokens[i].val );
			i++;
			continue;
		}
		error( i );
	}


	printf( "	RET\n");
	return 0;
}

