/*
    和scanner文件共同构成此法解析器
	lexer将最终的token返回给yacc解析
	其中lex的statement用来接收yacc语法解析的最终结果
 */

package dataManager

import (
	"errors"
	"fmt"
	"io"
)

// 数据库的关键字， 每一个的都是再yacc中的一个token
var keywords = map[string]int{
	"allocate":     ALLOCATE,
	"and":          AND,
	"by":           BY,
	"char":         CHAR,
	"create":       CREATE,
	"database":     DATABASE,
	"delete":       DELETE,
	"double":       DOUBLE,
	"drop":         DROP,
	"float":        FLOAT,
	"foreign":      UNUSED,
	"fragment":     FRAGMENT,
	"from":         FROM,
	"horizontally": HORIZONTALLY,
	"in":           IN,
	"insert":       INSERT,
	"int":          INT,
	"into":         INTO,
	"key":          KEY,
	"load":         LOAD,
	"nchar":        NCHAR,
	"not":          NOT,
	"null":         NULL,
	"number":       NUMBER,
	"on":           UNUSED,
	"only":         UNUSED,
	"or":           OR,
	"partition":    PARTITION,
	"primary":      PRIMARY,
	"schema":       SCHEMA,
	"table":        TABLE,
	"text":         UNUSED,
	"to":           TO,
	"unique":       UNUSED,
	"update":       UPDATE,
	"values":       VALUES,
	"varchar":      VARCHAR,
	"vertically":   VERTICALLY,
	"where":        WHERE,
}

// keywordStrings contains the reverse mapping of token to keyword strings
var keywordStrings = map[int]string{}

// ok, reverse
func init() {
	for str, id := range keywords {
		if id == UNUSED {
			continue
		}
		keywordStrings[id] = str
	}
}

// KeywordString returns the string(value, the upper) corresponding to the given keyword
func KeywordString(id int) string {
	str, ok := keywordStrings[id]
	if !ok {
		return ""
	}
	return str
}

// Find the value(int) of the keywords
func WordKeyInt(key string) int {
	if keywordID, found := keywords[key]; found {
		return keywordID
	}
	return 0
}

func newLex(r io.Reader) *lex {
	return &lex{
		input: NewScanner(r),
	}
}

// put the rune we read previously back to our buffered reader

// SO Let we Statement as result
type lex struct {
	input     *Scanner
	statement Statement
	err       error
}

//  implement the lex and error for goyacc
//  the lex stop when return 0
func (l *lex) Lex(lval *yySymType) int {
	// this is like a state-transfer machine
SCAN:
	tok, lit := l.input.Scan()
	switch tok {
	case EOF:
		fmt.Println("Lex is finished.")
		return 0 // stop lex
	case KEYWORD:
		return WordKeyInt(lit.(string))
	case IDENT: // 我们需要将这个存储起来以方便在yacc分析中使用,比如表名，列名之类
		lval.str = lit.(string) // STRING是yacc中的一个token,它的值我们通过%type <str> STRING存在再str中
		return IDENT            // 返回 token STRING
	case NAMEWITHDOT:
		lval.str = lit.(string)
		return NAMEWITHDOT
	case NUMBER:
		lval.num = lit.(string)
		return NUMBER // 返回 token NUMBER
	case STRING:
		lval.str = lit.(string)
		return STRING
	case Character:
		return lit.(int)
	case WS:
		// Skip and start
		goto SCAN
	case StringError:
		l.Error("String must end with '.\n")
		fmt.Println(l.err)
		//os.Exit(1)
	default:
		l.err = fmt.Errorf("Token error near %v.\n", lit)
		//fmt.Println(l.err)
		//os.Exit(1)
		return LEX_ERROR
	}
	return 0
}

func (l *lex) Error(s string) {
	// may be there will be a position report
	// waiting...
	l.err = errors.New(s)
}

// the func is the whole point we want
func Parse(r io.Reader) (Statement, error) {
	l := newLex(r)
	_ = yyParse(l)
	return l.statement, l.err
}
