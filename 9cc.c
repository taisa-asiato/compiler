#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

// トークンの型を表す値
enum {
	TK_NUM = 256,	// 整数トークン
	TK_EOF,		// 入力の終わりを表すトークン
};


enum {
	ND_NUM = 256,	// 整数ノードを示す
};

// トークンの型
typedef struct {
	int ty;		//トークンの型
	int val;	//tyがTK_NUMの場合, その数値
	char * input;	// トークン文字列（エラーメッセージ用)
} Token;

typedef struct Node {
	int ty;
	struct Node * lhs;
	struct Node * rhs;
	int val;
} Node;

//トークナイズした結果のトークン列はこの配列に保存する
// 100 以上のトークンは来ないものとする
Token tokens[100];
int pos = 0;

void tokenize( char * p );
Node * new_node( int ty, Node *lhs, Node * rhs );
Node * new_node_num( int val );
Node * expr();
Node * mul();
Node * term();
void error( int i );
void gen( Node * node );

// スタックマシン
void gen( Node * node )
{
	if ( node->ty = ND_NUM ) {
		printf( "	push	%d\n", node->val );
		return;
	}

	gen( node->lhs );
	gen( node->rhs );

	printf( "	pop	rdi\n" );
	printf( "	pop 	rax\n" );

	switch ( node->ty ) {
		case '+':
			printf( "	add	rax, rdi\n" );
			break;
		case '-':
			printf( "	sub	rax, rdi\n" );
			break;
		case '*':
			printf( "	mul	rdi\n" );
			break:
		case '/':
			printf( "	mov	rdx, 0\n" );
			printf( " 	div	rdx\n" );
			break;
	}
	printf( "	push	 rax\n" ); 
}

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

// 新しくNodeを追加する
// 追加されるNodeは,演算子
Node * new_node( int ty, Node *lhs, Node * rhs )
{
	Node * node = malloc( sizeof( Node ) );
	node->ty = ty;
	node->lhs = lhs;
	node->rhs = rhs;
	return node;
}


// 新しくNode(数値)追加する
Node * new_node_num( int val )
{
	Node * node = malloc( sizeof( Node ) );
	node->ty = ND_NUM;
	node->val = val;
	return node;
}

// 生成規則, expr
Node * expr()
{
	Node * lhs = mul();
	if ( tokens[pos].ty == '+' ) {
		pos++;
		return new_node( '+', lhs, expr() );
	}
	if ( tokens[pos].ty == '-' ) {
		pos++;
		return new_node( '-', lhs, expr() );
	}

	return lhs;
}

// 生成規則, mul
Node * mul()
{
	Node * lhs = term();
	if ( tokens[pos].ty == '*' ) {
		pos++;
		return new_node( '*', lhs, mul() );
	}
	if ( tokens[pos].ty == '/' ) {
		pos++;
		return new_node( '/', lhs, mul() );
	}

	return lhs;
}


// 生成規則, term
Node * term()
{
	if ( tokens[pos].ty == TK_NUM ) {
		return new_node_num( tokens[pos++].val );
	}
	if ( tokens[pos].ty == '(' ) {
		pos++;
		Node * node =  expr();
		if ( tokens[pos].ty != ')' ){
			error( "開き括弧に対する綴じ括弧がありません: %s\n", tokens[pos].input );
		}
		pos++;
		return node;

	}
	error( "数値でも開き括弧でもないトークンです: %s\n", tokens[pos].input );
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
	printf( "	mov	rax, %d\n", tokens[0].val );

	int i = 1;
	while ( tokens[i].ty != TK_EOF ) {
		if ( tokens[i].ty == '+' ) {
			i++;
			if ( tokens[i].ty != TK_NUM ) { error( i ); }
			printf( "	add 	rax, %d\n", tokens[i].val );
			i++;
			continue;
		}

		if ( tokens[i].ty == '-' ) {
			i++;
			if ( tokens[i].ty != TK_NUM ) { error( i ); }
			printf( "	sub	rax, %d\n", tokens[i].val );
			i++;
			continue;
		}
		error( i );
	}


	printf( "	ret\n");
	return 0;
}

