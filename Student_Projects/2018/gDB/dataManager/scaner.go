/*
	和lexer共同构成词法解析器
	scanner负责解析一开始的SQL解析
	并返回原始的token给lexer
*/
package dataManager

import (
	"bufio"
	"bytes"
	"io"
	"strings"
)

const (
	EOF         = -1
	KEYWORD     = -2
	Character   = -3
	WS          = -4
	StringError = -5
)

// Scanner represents a lexical scanner.
type Scanner struct {
	r *bufio.Reader
}

// NewScanner returns a new instance of Scanner.
func NewScanner(r io.Reader) *Scanner {
	return &Scanner{r: bufio.NewReader(r)}

}

// Scan returns the next token and literal value.
func (s *Scanner) Scan() (int, interface{}) {
	// Read the next rune.
	ch := s.read()

	// If we see whitespace then consume all contiguous whitespace.
	// If we see a letter then consume as an ident or reserved word.
	// If we see a digit then consume as a number.
	if isWhiteSpace(ch) {
		s.unread()
		return s.scanWhiteSpace()
	} else if isLetter(ch) {
		s.unread()
		return s.scanIdent()
	} else if isDigit(ch) {
		s.unread()
		return s.scanNumber()
	}

	// Otherwise read the individual character.
	switch ch {
	case eof:
		return EOF, ""
	case '\'':
		s.unread()
		return s.scanString()
	case '*':
		return Character, int(ch)
	case ',':
		return Character, int(ch)
	case '(':
		return Character, int(ch)
	case ')':
		return Character, int(ch)
	case ';':
		return Character, int(ch)
	case '=':
		return Character, int(ch)
	case '[':
		return Character, int(ch)
	case ']':
		return Character, int(ch)
	case '>':
		return Character, int(ch)
	case '<':
		return Character, int(ch)
	case '&':
		return KEYWORD, "and"
	}
	return 0, string(ch)
}

// scanWhitespace consumes the current rune and all contiguous whitespace.
func (s *Scanner) scanWhiteSpace() (int, string) {
	// Create a buffer and read the current character into it.
	var buf bytes.Buffer
	buf.WriteRune(s.read())

	// Read every subsequent whitespace character into the buffer.
	// Non-whitespace characters and EOF will cause the loop to exit.
	for {
		if ch := s.read(); ch == eof {
			break
		} else if !isWhiteSpace(ch) {
			s.unread()
			break
		} else {
			buf.WriteRune(ch)
		}
	}

	return WS, buf.String()
}

// scanIdent consumes the current rune and all contiguous ident runes.
// if the string is in the keywords, we return the keywords, else we consume it just as string
func (s *Scanner) scanIdent() (tok int, lit string) {
	// Create a buffer and read the current character into it.
	var buf bytes.Buffer
	buf.WriteRune(s.read())

	// Read every subsequent ident character into the buffer.
	// Non-ident characters and EOF will cause the loop to exit.
	for {
		if ch := s.read(); ch == eof {
			break
		} else if !isLetter(ch) && !isDigit(ch) && ch != '_' { // 必须以英文字母开通,可包含数字和_
			if ch == '.' { // 下面的 for循环我们想读入一个fragmentname 如 student.1
				buf.WriteRune(ch)
				for {
					if ch := s.read(); ch == eof {
						break
					} else if !isLetter(ch) && !isDigit(ch) && ch != '_' {
						s.unread()
						break
					} else {
						buf.WriteRune(ch)
					}
				}
				return NAMEWITHDOT, buf.String()
			}
			s.unread()
			break
		} else {
			buf.WriteRune(ch)
		}
	}
	loweredStr := strings.ToLower(buf.String())
	if _, found := keywords[loweredStr]; found {
		return KEYWORD, loweredStr // 如果是关键字, 返回小写, 对应map中的key
	}
	return IDENT, buf.String() // 如果是是标识符, 返回原字符串
}

// scanString is used to scan 'shen yun long' or 'ylshen@ruc.edu.cn'
func (s *Scanner) scanString() (tok int, lit string) {
	var buf bytes.Buffer
	buf.WriteRune(s.read())
	for {
		if ch := s.read(); ch == eof {
			return StringError, buf.String()
		} else if ch == '\'' {
			buf.WriteRune(ch)
			return STRING, buf.String()
		} else { //字符串内部可以是任何内容
			buf.WriteRune(ch)
		}
	}
}

// scanIdent consumes the current rune and all contiguous ident runes.
// NOTE: 0b and 0x prefixes are not supported.
func (s *Scanner) scanNumber() (tok int, lit string) {
	// Create a buffer and read the current character into it.
	var buf bytes.Buffer
	buf.WriteRune(s.read())

	// Read every subsequent ident character into the buffer.
	// Non-ident characters and EOF will cause the loop to exit.
	for {
		if ch := s.read(); ch == eof {
			break
		} else if !isDigit(ch) {
			s.unread()
			break
		} else {
			_, _ = buf.WriteRune(ch)
		}
	}

	return NUMBER, buf.String()
}

// I find that there is no scan function to recognize the string like 'zhangsan';
// So i write it following

// read reads the next rune from the buffered reader.
// Returns the rune(0) if an error occurs (or io.EOF is returned).
func (s *Scanner) read() rune {
	ch, _, err := s.r.ReadRune()
	if err != nil {
		return eof
	}
	return ch
}

// unread places the previously read rune back on the reader.
func (s *Scanner) unread() { _ = s.r.UnreadRune() }

// isWhitespace returns true if the rune is a space, tab, or newline.
func isWhiteSpace(ch rune) bool { return ch == ' ' || ch == '\t' || ch == '\n' }

// isLetter returns true if the rune is a letter.
func isLetter(ch rune) bool { return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') }

// isDigit returns true if the rune is a digit.
func isDigit(ch rune) bool { return (ch >= '0' && ch <= '9') }

// eof represents a marker rune for the end of the reader.
var eof = rune(0)
