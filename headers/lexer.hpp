#pragma once

#include <string>
#include <vector>
#include <string.h>

namespace Tisp {
    namespace Language {
	enum class TokenKind {
	    NAME,
	    KEYWORD,
	    NUMBER,
	    STRING,
	    EQ,
	    OPEN_PAREN,
	    CLOSE_PAREN,
	    COMMA,
	    COLON,
	    SEMI,
	    DOT,
	    TEOF,
	};
	struct Span {
	    const char* filename;
	    int         line;
	    int         cols;
	    int         cole;
	    Span(const char* f, int l, int sc, int ec) : filename(strdup(f)), line(l), cols(sc), cole(ec) {}
	    Span() {}
	};
	struct Token {
	    TokenKind   kind;
	    std::string data;
	    Span        span;
	    Token(TokenKind k, std::string lexme, Span s): kind(k), data(lexme), span(s) {}
	};
	typedef std::vector<Token> Tokens;
	struct Lexer {
	    std::string filename;
	    std::string source;
	    
	    int         pos    = 0;
	    int         column = 1;
	    int         line   = 1;
	    
	    Lexer(std::string filename);
	    char now();
	    char advance();
	    char before();
	    char peek();
	    Tokens parse();
	};
    } // Language
} // Tisp
