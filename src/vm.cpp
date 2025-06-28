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

void Vm::execute() {
  auto stmt =
      std::get<std::unique_ptr<NodeBody>>(std::move(program.stmt->stmt));
  for (auto &node : stmt->stmts) {
    execute_node(std::move(node));
  }
}

void Vm::execute_node(std::unique_ptr<NodeStmt> n) {
  switch (n->kind) {
  case StmtKind::Assignment: {
    std::unique_ptr<NodeAssignment> node =
        std::get<std::unique_ptr<NodeAssignment>>(std::move(n->stmt));
    Value::ValuePtr value = generate_value(std::move(node->expr));
    if (value->kind != ValueKind::Object)
      env.variables[node->name] =
          Value::Value::obj_from_Value(node->name, value);
    else
      env.variables[node->name] = value;
  } break;
  case StmtKind::Expr: {
    auto expr = std::get<std::unique_ptr<NodeExprStmt>>(std::move(n->stmt));
    Value::ValuePtr value = generate_value(std::move(expr->expr));
  } break;
  default:
    break;
  }
}
ValuePtr Vm::generate_value(std::unique_ptr<NodeExpr> expr) {
  if (auto nint = dynamic_cast<NodeInt *>(expr.get())) {
    return std::make_shared<Value::Value>(
        Value::Value(ValueKind::Number, nint->value));
  }
  if (auto nstr = dynamic_cast<NodeString *>(expr.get())) {
    return std::make_shared<Value::Value>(
        Value::Value(ValueKind::String, nstr->value));
  }
  if (auto nid = dynamic_cast<NodeIdent *>(expr.get())) {
    return this->env.variables[nid->identifier];
  }
  if (auto ncall = dynamic_cast<NodeCall *>(expr.get())) {
    return handle_call(ncall);
  }
  assert(0 && "Todo: add error value");
  exit(1);
}

ValuePtr Vm::handle_call(NodeCall *call) {
  if (auto nid = dynamic_cast<NodeIdent *>(call->callee.get())) {
    if (strcmp(nid->identifier, "println") == 0) {
      return Tisp::Runtime::Builtin::println(this,
                                             to_values(std::move(call->args)));
    }
  }
  assert(0 && "Todo: add error value");
  exit(1);
}

Vm::Args Vm::to_values(std::vector<Exprptr> args) {
  std::vector<ValuePtr> a;
  for (auto &arg : args) {
    a.push_back(generate_value(std::move(arg)));
  }
  return a;
}
