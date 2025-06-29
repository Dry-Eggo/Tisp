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
	    NodeExpr(Span s): span(s) {}
	    virtual ~NodeExpr() = default;
	};
	
	struct NodeAssignment {
	    Span                     span;
	    const char *             name;
	    Exprptr                  expr;
	    NodeAssignment(const char *name, Exprptr expr, Span s)
	    : name(std::move(name)), expr(std::move(expr)), span(s) {}
	};

	struct NodeIdent : NodeExpr {
	    Span                     span;
	    const char *             identifier;
	    NodeIdent(const char *name, Span s) : identifier(name), span(s), NodeExpr(s) {}
	};

	struct NodeInt : NodeExpr {
	    Span                     span;
	    int64_t                  value;
	    NodeInt(int64_t value, Span s) : value(value), span(s), NodeExpr(s) {}
	};

	struct NodeString : NodeExpr {
	    Span                      span;
	    const char *              value;
	    NodeString(const char *name, Span s) : value(name), span(s), NodeExpr(s) {}
	};
	struct NodeIf     : NodeExpr {
	    Span                      span;
	    Exprptr                   condition;
	    std::unique_ptr<NodeBody> then_body;
	    std::unique_ptr<NodeBody> else_body;
	    NodeIf(Exprptr cond, std::unique_ptr<NodeBody> tb, std::unique_ptr<NodeBody> eb, Span s)
	    : condition(std::move(cond)), then_body(std::move(tb)), else_body(std::move(eb)), span(s), NodeExpr(s) {}

	    NodeIf(Exprptr cond, std::unique_ptr<NodeBody> tb, Span s)
	    : condition(std::move(cond)), then_body(std::move(tb)), span(s), NodeExpr(s) {}
	};

	struct NodeLoop   : NodeExpr {
	    Span                      span;
	    Exprptr                   times;
	    std::unique_ptr<NodeBody> body;
	    NodeLoop(Exprptr t, std::unique_ptr<NodeBody> b, Span s)
	    : times(std::move(t)), body(std::move(b)), span(s), NodeExpr(s) {}
	};
	
	struct NodeFunction {
	    Span                      span;
	    const char *              name;
	    std::unique_ptr<NodeBody> body;
	    NodeFunction(const char *name, std::unique_ptr<NodeBody> body, Span s)
	    : name(std::move(name)), body(std::move(body)), span(s) {}
	};

	struct NodeExprStmt {
	    Exprptr expr;
	    explicit NodeExprStmt(Exprptr expr) : expr(std::move(expr)) {}
	};

	struct NodeBody {
	    Span                 span;
	    std::vector<Stmtptr> stmts;
	};

	struct NodeNop: NodeExpr {
	    NodeNop(): NodeExpr({}) {}
	};
	struct NodeCall: NodeExpr {
	    Span                 span;
	    Exprptr              callee;
	    std::vector<Exprptr> args;
	    NodeCall(Exprptr callee, std::vector<Exprptr> args, Span s)
	    : callee(std::move(callee)), args(std::move(args)), span(s), NodeExpr(s) {}
	};
	enum class BinaryOp {
	    Add,
	    Sub,
	    Mul,
	    Div,
	    Mod,
	    Shl,
	    Shr,
	    And,
	    Or,
	    Band,
	    Bor,
	};
	struct NodeBin: NodeExpr {
	    Span       span;
	    BinaryOp   op;
	    Exprptr    lhs;
	    Exprptr    rhs;
	    NodeBin(BinaryOp op, Exprptr l, Exprptr r, Span s): op(op), lhs(std::move(l)), rhs(std::move(r)), span(s), NodeExpr(s) {}
	};
	
	struct NodeStmt {
	    StmtKind kind;
	    Span span;
	    using StmtVariant =
	    std::variant<NodeAssignment*,
            NodeExprStmt*, NodeFunction*,
            NodeBody*, NodeCall*,
            NodeNop*>;
	    StmtVariant stmt;
	    NodeStmt() : kind(StmtKind::Nop), stmt(new NodeNop()) {}
	    NodeStmt(Span span, StmtKind kind, StmtVariant stmt)
	    : kind(kind), span(span), stmt(std::move(stmt)) {}
	    static Stmtptr make_nop(Span s = {}) {
		return std::make_unique<NodeStmt>(s, StmtKind::Nop,
                new NodeNop());
	    }
	};

	struct Node {
	    Span span;
	    Stmtptr stmt;
	};

	// Parser
	struct Parser {
	    Tokens        source;
	    ErrorManager* error_manager;
	    int           pos;
	    Token now();
	    Token before();
	    Token peek();
	    void advance();
	    Node parse();
	    Exprptr parse_expr();
	    Exprptr parse_term();
	    Exprptr parse_additive();
	    Exprptr parse_atom();
	    Exprptr parse_logical_or();
	    Exprptr parse_logical_and();
	    
	    Node parse_func();
	    std::unique_ptr<NodeBody> parse_body();
	    void expect(TokenKind k);
	    void expect_kw(const char *);
	    bool match(TokenKind k);
	    Parser(Tokens source, ErrorManager* em) : source(source), pos(0), error_manager(em) {}
	};
    } // namespace Language
} // namespace Tisp
