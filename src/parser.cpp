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
	    NodeBody* body = new NodeBody();
	    Node program;
	    while (now().kind != TokenKind::TEOF) {
		switch (now().kind) {
		case Tisp::Language::TokenKind::KEYWORD: {
		    if (now().data == "func") {
			Node func_stmt = parse_func();
		    } else
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
			new NodeAssignment(
			NodeAssignment(name, std::move(expr), span)));
			if (!stmt) {
			    printf("Stmt if null\n");
			}
			body->stmts.push_back(std::move(stmt));
		    } else {
			auto expr = parse_expr();
			auto exprs = new NodeExprStmt(std::move(expr));
			auto stmt = std::make_unique<NodeStmt>(now().span, StmtKind::Expr,
			exprs);
			body->stmts.push_back(std::move(stmt));
		    }
		} break;
	    default:
		auto expr = parse_expr();
		expect(TokenKind::SEMI);
		auto exprs = new NodeExprStmt(std::move(expr));
		body->stmts.push_back(std::make_unique<NodeStmt>(now().span, StmtKind::Expr, exprs));
		break;
	    }
	}
	program.stmt = std::make_unique<NodeStmt>(now().span, StmtKind::Body, body);
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
	    std::stringstream s;
	    s << "Expected a name\n";
	    error_manager->report(Diagnostic(DiagnosticType::Error, now().span, s.str(), ""), true);
	}
	expect(TokenKind::COLON);
	fn->body = parse_body();
	n.stmt = std::make_unique<NodeStmt>();
	n.stmt->stmt = fn.get();
	return n;
    }

    std::unique_ptr<NodeBody> Parser::parse_body() {
	std::unique_ptr<NodeBody> b = std::make_unique<NodeBody>();
    // TODO:
	while (!(now().kind == TokenKind::KEYWORD && now().data == "end" || now().data == "else")) {
	    switch (now().kind) {
	    case Tisp::Language::TokenKind::KEYWORD: {
		if (now().data == "func") {
		    Node func_stmt = parse_func();
		} else
		if (now().data == "let") {
		    advance();
		    auto span = now().span;
		    const char *name = strdup(now().data.c_str());
		    advance();
		    expect(TokenKind::EQ);
		    auto expr = parse_expr();
		    expect(TokenKind::SEMI);
		    std::unique_ptr<NodeStmt> stmt = std::make_unique<NodeStmt>(span, StmtKind::Assignment, new NodeAssignment(name, std::move(expr), span));
		    if (!stmt) {
			printf("Stmt is null\n");
		    }
		    b->stmts.push_back(std::move(stmt));
		} else {
		    std::stringstream s;
		    s << "Invalid Statememt: '" << now().data << "' \n";
		    error_manager->report(Diagnostic(DiagnosticType::Error, now().span, s.str(), ""), true);		    
		    exit(1);
		}
	    } break;
	    default: {
		auto expr = parse_expr();
		expect(TokenKind::SEMI);
		auto exprs = new NodeExprStmt(std::move(expr));
		auto stmt = std::make_unique<NodeStmt>(now().span, StmtKind::Expr, exprs);
		b->stmts.push_back(std::move(stmt));
	    } break;
	}
    }
    return b;
}

Exprptr Parser::parse_expr() {
    return parse_logical_or();
}

Exprptr Parser::parse_logical_or() {
    Span    span = now().span;
    Exprptr lhs  = parse_logical_and();
    while (match(TokenKind::OR)) {
	auto op = BinaryOp::Or;
	advance();
	Exprptr rhs  = parse_logical_or();
	Exprptr node = std::make_unique<NodeBin>(op, std::move(lhs), std::move(rhs), span);
	lhs          = std::move(node);
    }
    return lhs;
}

Exprptr Parser::parse_logical_and() {
    Span    span = now().span;
    Exprptr lhs = parse_additive();
    while (match(TokenKind::AND)) {
	auto op = BinaryOp::And;
	advance();
	Exprptr rhs  = parse_logical_or();
	Exprptr node = std::make_unique<NodeBin>(op, std::move(lhs), std::move(rhs), span);
	lhs          = std::move(node);
    }
    return lhs;
}

Exprptr Parser::parse_additive() {
    Span    span = now().span;
    Exprptr lhs = parse_term();
    while(match(TokenKind::ADD) || match(TokenKind::SUB)) {
	auto op = (now().kind == TokenKind::ADD)? BinaryOp::Add : BinaryOp::Sub;
	advance();
	Exprptr rhs = parse_expr();
	auto node = std::make_unique<NodeBin>(op, std::move(lhs), std::move(rhs), span);
	lhs = std::move(node);
    }
    return lhs;
}

Exprptr Parser::parse_term() {
    Span    span = now().span;
    Exprptr lhs = parse_atom();
    while(match(TokenKind::MUL) || match(TokenKind::DIV)) {
	auto op = (now().kind == TokenKind::DIV)? BinaryOp::Div : BinaryOp::Mul;
	advance();
	Exprptr rhs = parse_expr();
	auto node = std::make_unique<NodeBin>(op, std::move(lhs), std::move(rhs), span);
	lhs = std::move(node);
    }
    return lhs;
}

std::unique_ptr<NodeExpr> Parser::parse_atom() {
    switch (now().kind) {
    case TokenKind::KEYWORD: {
	if (now().data == "if") {
	    Span if_start    = now().span;
	    advance();
	    Exprptr condition = parse_expr();
	    expect(TokenKind::COLON);
	    std::unique_ptr<NodeBody> then_body = parse_body();
	    if (now().data == "end") {
		expect_kw("end");
		return std::make_unique<NodeIf>(std::move(condition), std::move(then_body), if_start);
	    } else if (now().data == "else") {
		expect_kw("else");
		expect(TokenKind::COLON);
		std::unique_ptr<NodeBody> else_body = parse_body();
		expect_kw("end");
		return std::make_unique<NodeIf>(std::move(condition), std::move(then_body), std::move(else_body), if_start);
	    }
	} else if (now().data == "loop") {
	    Span loop_start = now().span;
	    advance();
	    Exprptr times   = parse_expr();
	    expect(TokenKind::COLON);
	    auto    body    = parse_body();
	    expect_kw("end");
	    return std::make_unique<NodeLoop>(std::move(times), std::move(body), loop_start);
	}
	error_manager->add(Diagnostic(DiagnosticType::Error, now().span, "Invalid Expression", ""));
	return std::make_unique<NodeNop>();
    } break;
    case TokenKind::NAME: {
	const char *name = strdup(now().data.c_str());
	Span        span = now().span;
	advance();
	if (match(TokenKind::OPEN_PAREN)) {
	    advance();
	    std::unique_ptr<NodeExpr> callee =
            std::make_unique<NodeIdent>(NodeIdent(name, span));
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
            std::make_unique<NodeCall>(std::move(callee), std::move(args), span);
	    return call;
	}
	return std::make_unique<NodeIdent>(NodeIdent(name, span));
    } break;
    case TokenKind::NUMBER: {
	int64_t num  = std::stoi(now().data.c_str());
	Span    span = now().span;
	advance();
	return std::make_unique<NodeInt>(num, span);
    } break;
    case TokenKind::STRING: {
	const char *s    = strdup(now().data.c_str());
	Span        span = now().span;
	advance();
	return std::make_unique<NodeString>(s, span);
    } break;
    default:
	std::stringstream s;
	s << "Invalid Expr\n";
	error_manager->report(Diagnostic(DiagnosticType::Error, now().span, s.str(), ""), true);
	exit(1);
    }
}
void Parser::expect(TokenKind k) {
    if (!match(k)) {
	std::stringstream s;
	s << "Unexpected Token: '" << now().data << "'\n";
	error_manager->report(Diagnostic(DiagnosticType::Error, now().span, s.str(), ""), true);
	exit(1);
    }
    advance();
}
bool Parser::match(TokenKind k) { return (now().kind == k); }
void Parser::expect_kw(const char *w) {
    if (match(TokenKind::KEYWORD) && now().data == w) {
	advance();
	return;
    }
    std::stringstream s;
    s << "Expected: '" << w << "'\n";
    error_manager->report(Diagnostic(DiagnosticType::Error, now().span, s.str(), ""), true);
    exit(1);
}
} // namespace Language
} // namespace Tisp
