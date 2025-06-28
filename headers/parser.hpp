#pragma once
#include "value.hpp"
#include <cstdint>
#include <lexer.hpp>
#include <memory>
#include <vector>
namespace Tisp {
namespace Language {
enum class StmtKind {
  Assignment,
  Function,
  Program,
  Body,
  Expr,
  Nop,
};
enum class ExprKind {
  Int,
  String,
  Call,
  Ident,
  Nop,
};
struct Node;
struct NodeExpr;
struct NodeStmt;
struct NodeAssignment;
struct NodeFunction;
struct NodeCall;
struct NodeBody;
struct NodeExprStmt;
struct NodeInt;
struct NodeString;
struct NodeIdent;

using Exprptr = std::unique_ptr<NodeExpr>;
using Stmtptr = std::unique_ptr<NodeStmt>;

struct NodeExpr {
  Span span;
  virtual ~NodeExpr() = default;
};

struct NodeAssignment {
  const char *name;
  Exprptr expr;
  NodeAssignment(const char *name, Exprptr expr)
      : name(std::move(name)), expr(std::move(expr)) {}
};

struct NodeIdent : NodeExpr {
  const char *identifier;
  NodeIdent(const char *name) : identifier(name) {}
};

struct NodeInt : NodeExpr {
  int64_t value;
  NodeInt(int64_t value) : value(value) {}
};

struct NodeString : NodeExpr {
  const char *value;
  NodeString(const char *name) : value(name) {}
};

struct NodeFunction {
  const char *name;
  std::unique_ptr<NodeBody> body;
  NodeFunction(const char *name, std::unique_ptr<NodeBody> body)
      : name(std::move(name)), body(std::move(body)) {}
};

struct NodeExprStmt {
  Exprptr expr;
  explicit NodeExprStmt(Exprptr expr) : expr(std::move(expr)) {}
};

struct NodeBody {
  std::vector<Stmtptr> stmts;
};

struct NodeNop {};
struct NodeCall: NodeExpr {
  Exprptr callee;
  std::vector<Exprptr> args;
  NodeCall(Exprptr callee, std::vector<Exprptr> args)
      : callee(std::move(callee)), args(std::move(args)) {}
};
struct NodeStmt {
  StmtKind kind;
  Span span;
  using StmtVariant =
      std::variant<std::unique_ptr<NodeAssignment>,
                   std::unique_ptr<NodeExprStmt>, std::unique_ptr<NodeFunction>,
                   std::unique_ptr<NodeBody>, std::unique_ptr<NodeCall>,
                   std::unique_ptr<NodeNop>>;
  StmtVariant stmt;
  NodeStmt() : kind(StmtKind::Nop), stmt(std::make_unique<NodeNop>()) {}
  NodeStmt(Span span, StmtKind kind, StmtVariant stmt)
      : kind(kind), span(span), stmt(std::move(stmt)) {}
  static Stmtptr make_nop(Span s = {}) {
    return std::make_unique<NodeStmt>(s, StmtKind::Nop,
                                      std::make_unique<NodeNop>());
  }
};

struct Node {
  Span span;
  Stmtptr stmt;
};

// Parser
struct Parser {
  Tokens source;
  int pos;
  Token now();
  Token before();
  Token peek();
  void advance();
  Node parse();
  std::unique_ptr<NodeExpr> parse_expr();
  Node parse_func();
  std::unique_ptr<NodeBody> parse_body();
  void expect(TokenKind k);
  void expect_kw(const char *);
  bool match(TokenKind k);
  Parser(Tokens source) : source(source), pos(0) {}
};
} // namespace Language
} // namespace Tisp
