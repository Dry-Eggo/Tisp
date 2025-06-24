#include <iostream>
#include <value.hpp>
#include <lexer.hpp>
#include <parser.hpp>
extern void print_usage(const char* program) {
    std::cout << "Usage: " << program << " <filename>\n";
}


int main(int argc, char** argv) {
    if (argc < 2) {
	print_usage(argv[0]);
	exit(1);
    }
    std::string filename = argv[1];
    Tisp::Language::Lexer  Lexer    = Tisp::Language::Lexer(filename);
    Tisp::Language::Tokens tokens   = Lexer.parse();
}
