#pragma once
#include <lexer.hpp>
namespace Tisp {
    namespace Language {
	enum class NodeKind  {
	    Assignment,
	    Function,
	    Int,
	    String,
	    Ident,
	};
	struct Node;
	struct NodeInt {
	    Value::Value  value;
	};
	struct NodeString {
	    Value::Value  value;
	};
	struct NodeIdent {
	    std::string name;
	};
	struct NodeBody {
	    std::vector<Node> body;
	};
	struct NodeFunction {
	    std::string name;
	    NodeBody    body;
	};
	struct NodeExpr {
	    std::variant<NodeInt, NodeString, NodeIdent, NodeFunction> expr;
	};
	struct NodeAssignment {
	    std::string name;
	    NodeExpr    expr;
	};
	struct NodeExprStmt {
	    NodeExpr expr;
	};
	struct NodeStmt {
	    std::variant<NodeAssignment, NodeExprStmt> stmt;
	};
	struct NodeProgram {
	    std::string module;
	    NodeBody    body;
	};
	struct Node {
	    NodeKind kind;
	    Span     span;
	    NodeStmt stmt;
	};
	// Parser
	struct Parser {
	    Tokens source;
	    int    pos;
	    Parser(Tokens source): source(source), pos(0) {}
	    Token now();
	    Token before();
	    Token peek();
	    void advance();

	    
	};
    }
}
