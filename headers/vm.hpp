#pragma once

#include <memory>
#include <parser.hpp>
#include <unordered_map>
#include <value.hpp>

using namespace Tisp::Language;

namespace Tisp {
namespace Runtime {
struct Env {
  std::unordered_map<std::string, std::shared_ptr<Value::Value>> variables;
};
struct Vm {
  using Args = std::vector<Value::ValuePtr>;
  Language::Node program;
  Env env;
  Vm(Language::Node program) { this->program = std::move(program); }
  void execute();
  void execute_node(std::unique_ptr<NodeStmt> node);
  Value::ValuePtr generate_value(std::unique_ptr<NodeExpr> expr);
  Value::ValuePtr handle_call(NodeCall *call);
  Args to_values(std::vector<Exprptr> args);
};
} // namespace Runtime
} // namespace Tisp
