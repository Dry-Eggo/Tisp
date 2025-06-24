#include <lexer.hpp>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

namespace Tisp {
    namespace Language {
	Lexer::Lexer(std::string filename) {
	    this->filename = filename;
	    if (!fs::exists(filename)) {
		std::cout << "Unable to open '" << filename << "' : No Such file or directory\n";
		exit(1);
	    }
	    std::fstream      f = std::fstream(filename, std::ios::in);
	    std::stringstream s;
	    s << f.rdbuf();
	    source              = s.str();
	}
	char Lexer::now() {
	    if (pos >= source.size()) {
		return EOF;
	    }
	    return source.at(pos);
	}
	char Lexer::peek() {
	    if (pos + 1 >= source.size()) {
		return EOF;
	    }
	    return source.at(pos + 1);
	}
	char Lexer::before() {
	    if (pos - 1 < 0) {
		return EOF;
	    }
	    return source.at(pos - 1);
	}
	char Lexer::advance() {
	    char ch = now();
	    if (ch == '\n') {
		line++;
		column = 1;
	    } else {
		column++;
	    }
	    pos++;
	    return ch;
	}
	Tokens Lexer::parse() {
	    Tokens tokens;
	    while(now() != EOF) {
		if (isspace(now())) {
		    advance();
		    continue;
		}
		if (isalpha(now()) || now() == '_') {
		    std::string buf;
		    int sc = column;
		    while (now() != EOF && (isalnum(now()) || now() == '_')) {
			buf += advance();
		    }
		    std::cout << "Name: " << buf << "\n";
		    tokens.push_back(Token(TokenKind::NAME, buf, Span(filename, line, sc, column-1)));
		    continue;
		}
		int sc = column;
		switch (now()) {
		case '=':
		    advance();
		    tokens.push_back(Token(TokenKind::EQ, "=", Span(filename, line, sc, column-1)));		    
		    break;
		case '(':
		    advance();
		    tokens.push_back(Token(TokenKind::OPEN_PAREN, "(", Span(filename, line, sc, column-1)));		    
		    break;
		case ')':
		    advance();
		    tokens.push_back(Token(TokenKind::CLOSE_PAREN, ")", Span(filename, line, sc, column-1)));		    
		    break;
		default:
		    std::cout << "Error:" << filename << "(" << line << ":" << column << "): Unexpected char: '" << now() << "'\n";
		    exit(1);
		}
	    }
	    return tokens;
	}
    }
}
