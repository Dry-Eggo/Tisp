#include "lexer.hpp"
#include <cassert>
#include <cstdio>
#include <iostream>
#include <memory>
#include <parser.hpp>
#include <stdlib.h>

namespace Tisp {
namespace Language {
using std::make_unique;

Token Parser::peek() {
  if (pos + 1 >= source.size()) {
    // last Token is always EOF
    return source.at(source.size() - 1);
  }
  return source.at(pos + 1);
}
Token Parser::now() {
  if (pos >= source.size()) {
    return source.at(source.size() - 1);
  }
  return source.at(pos);
}
Token Parser::before() {
  if (pos - 1 >= source.size()) {
    return source.at(source.size() - 1);
  }
  return source.at(pos - 1);
}
void Parser::advance() { pos++; }
Node Parser::parse() {
  std::unique_ptr<NodeBody> body = std::make_unique<NodeBody>();
  Node program;
  while (now().kind != TokenKind::TEOF) {
    switch (now().kind) {
    case Tisp::Language::TokenKind::KEYWORD: {
      if (now().data == "func") {
        Node func_stmt = parse_func();
      }
      if (now().data == "let") {
        advance();
        auto span = now().span;
        const char *name = strdup(now().data.c_str());
        advance();
        expect(TokenKind::EQ);
        auto expr = parse_expr();
        expect(TokenKind::SEMI);
        std::unique_ptr<NodeStmt> stmt = std::make_unique<NodeStmt>(
            span, StmtKind::Assignment,
            std::make_unique<NodeAssignment>(
                NodeAssignment(name, std::move(expr))));
        if (!stmt) {
          printf("Stmt if null\n");
        }
        body->stmts.push_back(std::move(stmt));
      } else {
        std::cout << "Invalid Top-level item: '" << now().data << "' \n";
        exit(1);
      }
    } break;
    default:
      auto expr = parse_expr();
      expect(TokenKind::SEMI);
      auto exprs = std::make_unique<NodeExprStmt>(std::move(expr));
      auto stmt = std::make_unique<NodeStmt>(now().span, StmtKind::Expr,
                                             std::move(exprs));
      body->stmts.push_back(std::move(stmt));
      break;
    }
  }
  program.stmt =
      std::make_unique<NodeStmt>(now().span, StmtKind::Body, std::move(body));
  return program;
}

Node Parser::parse_func() {
  Node n;
  advance();
  std::unique_ptr<NodeFunction> fn;
  if (match(TokenKind::NAME)) {
    fn->name = strdup(now().data.c_str());
    advance();
  } else {
    std::cout << "Todo Error Expected a name\n";
    exit(1);
  }
  expect(TokenKind::COLON);
  fn->body = parse_body();
  n.stmt = std::make_unique<NodeStmt>();
  n.stmt->stmt = std::move(fn);
  return n;
}

std::unique_ptr<NodeBody> Parser::parse_body() {
  std::unique_ptr<NodeBody> b = std::make_unique<NodeBody>();
  // TODO:
  while (now().kind != TokenKind::KEYWORD && now().data == "end") {
    switch (now().kind) {
    default: {
      std::unique_ptr<NodeStmt> stmt = std::make_unique<NodeStmt>();
      expect(TokenKind::SEMI);
      stmt->kind = StmtKind::Expr;
    } break;
    }
  }
  return b;
}

std::unique_ptr<NodeExpr> Parser::parse_expr() {
  switch (now().kind) {
  case TokenKind::NAME: {
    const char *name = strdup(now().data.c_str());
    advance();
    if (match(TokenKind::OPEN_PAREN)) {
      advance();
      std::unique_ptr<NodeExpr> callee =
          std::make_unique<NodeIdent>(NodeIdent(name));
      std::vector<Exprptr> args;
      while (!match(TokenKind::CLOSE_PAREN)) {
        args.push_back(parse_expr());
        if (match(TokenKind::COMMA)) {
          advance();
          continue;
        }
      }
      expect(TokenKind::CLOSE_PAREN);
      Exprptr call =
          std::make_unique<NodeCall>(std::move(callee), std::move(args));
      return call;
    }
    return std::make_unique<NodeIdent>(NodeIdent(name));
  } break;
  case TokenKind::NUMBER: {
    int64_t num = std::stoi(now().data.c_str());
    advance();
    return std::make_unique<NodeInt>(num);
  } break;
  case TokenKind::STRING: {
    const char *s = strdup(now().data.c_str());
    advance();
    return std::make_unique<NodeString>(s);
  } break;
  default:
    std::cout << "Invalid Expr\n";
    exit(1);
  }
}
void Parser::expect(TokenKind k) {
  if (!match(k)) {
    std::cout << "Todo Error: Unexpected Token: " << now().data << "\n";
    exit(1);
  }
  advance();
}
bool Parser::match(TokenKind k) { return (now().kind == k); }
void Parser::expect_kw(const char *w) {
  if (match(TokenKind::KEYWORD) && now().data == w) {
    advance();
  }
  std::cout << "Todo Error: Unexpected Token\n";
  exit(1);
}
} // namespace Language
} // namespace Tisp
