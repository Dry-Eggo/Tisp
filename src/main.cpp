#include <iostream>
#include <lexer.hpp>
#include <parser.hpp>
#include <value.hpp>
#include <vm.hpp>
extern void print_usage(const char *program) {
  std::cout << "Usage: " << program << " <filename>\n";
}

int main(int argc, char **argv) {
  if (argc < 2) {
    print_usage(argv[0]);
    exit(1);
  }
  std::string filename = argv[1];
  Tisp::Language::Lexer Lexer = Tisp::Language::Lexer(filename);
  ErrorManager error_manager  = ErrorManager(Lexer.source);
  Lexer.error_manager = &error_manager;
  Tisp::Language::Tokens tokens = Lexer.parse();
  Tisp::Language::Parser parser = Tisp::Language::Parser(tokens, &error_manager);
  auto p = parser.parse();
  Tisp::Runtime::Vm vm = Tisp::Runtime::Vm(std::move(p), &error_manager);
  vm.execute();
  error_manager.reportAll();
}
