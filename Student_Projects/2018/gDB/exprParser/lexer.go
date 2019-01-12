/*
** 这个文件夹中中所有语句是为了判断insert的条件
** 如 id > 3 and sex = 'F' ,满足条件返回ture, 反正返回flase
**若 id = 4, sex = 'M', 返回 false
*/
package exprParser

import (
	"errors"
	"fmt"
	"io"
	"os"
)

func newLex(r io.Reader, tableName string, inserMap map[string]string) *lex {
	return &lex{
		input:     NewScanner(r),
		tableName: tableName,
		insertMap: inserMap,
	}
}

type lex struct {
	input     *Scanner
	result    bool
	err       error
	tableName string
	insertMap map[string]string
}

//  implement the lex and error for goyacc
//  the lex stop when return 0
func (l *lex) Lex(lval *yySymType) int {
	// this is like a state-transfer machine
SCAN:
	tok, lit := l.input.Scan()
	switch tok {
	case EOF:
		//fmt.Println("Lex is finished.") // 这里如果打印会出现很多个Lex
		return 0 // stop lex
	case IDENT:
		// FIXME 这里假定读取出来的IDENT都是真确的属性
		lval.str = l.insertMap[lit.(string)]
		return IDENT
	case NUMBER:
		lval.num = lit.(int)
		//fmt.Println(lval.num)
		return NUMBER
	case STRING:
		lval.str = lit.(string)
		return STRING
	case AND:
		return AND
	case Character:
		return lit.(int)
	case WS:
		// Skip and start
		goto SCAN
	case StringError:
		l.Error("String must end with '.\n")
		fmt.Println(l.err)
		os.Exit(1)
	default:
		l.err = fmt.Errorf("Token error near %v.\n", lit)
		fmt.Print(l.err)
		os.Exit(1)
	}
	return 0
}

func (l *lex) Error(s string) {
	// may be there will be a position report
	// waiting...
	l.err = errors.New(s)
}

// the func is the whole point we want
func Parse(r io.Reader, tableName string, insertValue map[string]string) (bool, error) {
	l := newLex(r, tableName, insertValue)
	_ = yyParse(l)
	return l.result, l.err
}
