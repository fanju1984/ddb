%{
    /*
    ** 这个文件夹中中所有语句是为了判断insert的条件
    ** 如 id > 3 and sex = 'F' ,满足条件返回ture, 反正返回flase
    **若 id = 4, sex = 'M', 返回 false
    */
     package exprParser
     import "strconv"
     func setResult(l yyLexer, v bool) {
          l.(*lex).result = v
     }

%}

%union{
    num	   int
    str	   string
    b      bool
}
%token AND
%token <str> IDENT
%token <num> NUMBER
%token <str> STRING
%type <b> exprs
%type <b> expr
%type <str> a_op
%start main

%%

main: exprs
    {
	setResult(yylex, $1)
    }

exprs: expr
     {
     	$$ = $1
     }
     | exprs AND expr
     {
     	$$ = $$ && $3
     }

expr: IDENT a_op NUMBER
     {
     	n, _ := strconv.Atoi($1)
     	switch $2 {
        case ">":
     	   $$ = n > $3
     	case "<":
           $$ = n < $3
        case "=":
           $$ = n == $3
        case "<>":
           $$ = n != $3
        case ">=":
           $$ = n >= $3
        case "<=":
           $$ = n <= $3
	}
     }
    | IDENT a_op STRING
    {
	switch $2 {
        case ">":
           $$ = $1 > $3
        case "<":
           $$ = $1 < $3
        case "=":
           $$ = $1 == $3
        case "<>":
           $$ = $1 != $3
        case ">=":
           $$ = $1 >= $3
        case "<=":
           $$ = $1 <= $3
	}
    }

a_op: '>' {$$ = ">"}
    | '=' {$$ = "="}
    | '<' {$$ = "<"}
    | '>' '=' {$$ = ">="}
    | '<' '=' {$$ = "<="}
    | '<' '>' {$$ = "<>"}


%%