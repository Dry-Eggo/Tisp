#pragma once

#include <string>
#include <vector>
#include <string.h>
#include <error.hpp>

namespace Tisp {
    namespace Language {
	enum class TokenKind {
	    NAME,
	    KEYWORD,
	    NUMBER,
	    STRING,
	    // punctuations
	    EQ,
	    OPEN_PAREN,
	    CLOSE_PAREN,
	    COMMA,
	    COLON,
	    SEMI,
	    DOT,
	    // operators
	    ADD,      // +
	    SUB,      // -
	    MUL,      // *
	    DIV,      // /
	    SHR,      // >>
	    SHL,      // <<
	    ADDEQ,    // +=
	    SUBEQ,    // -=
	    MULEQ,    // *=
	    DIVEQ,    // /=
	    INC,      // ++
	    DEC,      // --
	    OR,       // ||
	    AND,      // &&
	    BAND,     // &
	    BOR,      // |
	    TEOF,
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
	    ErrorManager*  error_manager;
	    
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
