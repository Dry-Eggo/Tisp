#include "parser.hpp"
#include "value.hpp"
#include <builtins.hpp>
#include <cassert>
#include <cstdio>
#include <memory>
#include <string.h>
#include <vm.hpp>
using namespace Tisp::Language;
using namespace Tisp::Runtime;
using namespace Tisp::Value;
using Tisp::Value::Value;

Vm::Vm(Language::Node program, ErrorManager* em): error_manager(em) {
    this->program = std::move(program);
    this->builtins["println"] = Runtime::Builtin::println;
    this->builtins["print"]   = Runtime::Builtin::print;
    this->builtins["exec"]    = Runtime::Builtin::exec;
}

void Vm::execute() {
    auto stmt = std::get<NodeBody*>(program.stmt->stmt);
    for (auto &node : stmt->stmts) {
	execute_node(node.get());
    }
}

void Vm::execute_node(NodeStmt* n) {
    switch (n->kind) {
    case StmtKind::Assignment: {
	NodeAssignment* node  = std::get<NodeAssignment*>(n->stmt);
	Value::ValuePtr value = generate_value(node->expr.get());
	
	if (value->kind != ValueKind::Object) {
	    env.variables[node->name] = Value::Value::obj_from_Value(node->name, value);
	}
	else
	env.variables[node->name] = value;
    } break;
    case StmtKind::Expr: {
	auto expr = std::get<NodeExprStmt*>(n->stmt);
	Value::ValuePtr value = generate_value(expr->expr.get());
    } break;
default:
    break;
}
}
ValuePtr Vm::generate_value(NodeExpr* expr) {
    if (auto nint = dynamic_cast<NodeInt *>(expr)) {
	return std::make_shared<Value::Value>(
        Value::Value(ValueKind::Number, nint->value));
    }
    if (auto nstr = dynamic_cast<NodeString *>(expr)) {
	return std::make_shared<Value::Value>(
        Value::Value(ValueKind::String, nstr->value));
    }
    if (auto nbin = dynamic_cast<NodeBin *>(expr)) {
	auto lhs  = generate_value(nbin->lhs.get());
	auto rhs  = generate_value((nbin->rhs.get()));
	assert(lhs->kind == Value::ValueKind::Number && rhs->kind == Value::ValueKind::Number);
	switch (nbin->op) {
	case BinaryOp::Add:
	    return std::make_unique<Value::Value>(Value::Value(ValueKind::Number,
	    std::get<int64_t>(lhs->data) + std::get<int64_t>(rhs->data)
	    ));
	case BinaryOp::Sub:
	    return std::make_unique<Value::Value>(Value::Value(ValueKind::Number,
	    std::get<int64_t>(lhs->data) - std::get<int64_t>(rhs->data)
	    ));
	case BinaryOp::Mul:
	    return std::make_unique<Value::Value>(Value::Value(ValueKind::Number,
	    std::get<int64_t>(lhs->data) * std::get<int64_t>(rhs->data)
	    ));
	case BinaryOp::Div:
	    return std::make_unique<Value::Value>(Value::Value(ValueKind::Number,
	    std::get<int64_t>(lhs->data) / std::get<int64_t>(rhs->data)
	    ));
	case BinaryOp::Or:
	    return std::make_unique<Value::Value>(Value::Value(ValueKind::Number,
	    std::get<int64_t>(lhs->data) || std::get<int64_t>(rhs->data)
	    ));
	case BinaryOp::And:
	    return std::make_unique<Value::Value>(Value::Value(ValueKind::Number,
	    std::get<int64_t>(lhs->data) && std::get<int64_t>(rhs->data)
	    ));
	}
    }
    if (auto nid = dynamic_cast<NodeIdent *>(expr)) {
	return this->env.variables[nid->identifier];
    }
    if (auto ncall = dynamic_cast<NodeCall *>(expr)) {
	return handle_call(ncall);
    }
    if (auto nif = dynamic_cast<NodeIf *>(expr)) {
	ValuePtr cond = generate_value(nif->condition.get());
	if (cond->is_truthy()) {
	    for(auto &node: nif->then_body->stmts) {
		execute_node(node.get());
	    }
	} else if (nif->else_body) {
	    for(auto &node: nif->else_body->stmts) {
		execute_node(node.get());
	    }
	}
	return cond;
    } if (auto nloop = dynamic_cast<NodeLoop *>(expr)) {
	auto value   = generate_value(nloop->times.get());
	if (value->kind == ValueKind::Number) {
	    int64_t times = std::get<int64_t>(value->data);
	    for (int i = 0; i < times; i++) {
		for (auto& node: nloop->body->stmts) {
		    execute_node(node.get());
		}
	    }
	}
	return value;
    }
    error_manager->report(Diagnostic(DiagnosticType::Error, expr->span, "Todo: Add Error Value Type", ""), true);
    exit(1);
}


ValuePtr Vm::handle_call(NodeCall *call) {
    if (auto nid = dynamic_cast<NodeIdent *>(call->callee.get())) {
	if (this->builtins.count(nid->identifier)) {
	    return this->builtins[nid->identifier](this,
	    to_values(std::move(call->args)));
	}
    }
    assert(0 && "Todo: add error value");
    exit(1);
}

Args Vm::to_values(std::vector<Exprptr> args) {
    std::vector<ValuePtr> a;
    for (auto &arg : args) {
	a.push_back(generate_value(arg.get()));
    }
    return a;
}
