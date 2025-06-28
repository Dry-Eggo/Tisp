#include "value.hpp"
#include <builtins.hpp>
#include <cstdint>
#include <cstdio>
#include <iostream>
namespace Tisp::Runtime::Builtin {
Value::ValuePtr println(Vm *vm, std::vector<Value::ValuePtr> args) {
  for (auto value : args) {
    if (value && value->kind == Value::ValueKind::Number) {
      std::cout << std::get<int64_t>(value->data) << " ";
    } else if (value && value->kind == Value::ValueKind::String) {
      std::cout << std::get<std::string>(value->data) << " ";
    } else if (value && value->kind == Value::ValueKind::Object) {
      auto pair =
          std::get<std::pair<const char *, Value::ValuePtr>>(value->data);
      auto v = pair.second;
      if (v->kind == Value::ValueKind::Number) {
        std::cout << std::get<int64_t>(v->data) << " ";
      } else if (v->kind == Value::ValueKind::String) {
        std::cout << std::get<std::string>(v->data) << " ";
      }
    }
  }
  std::cout << "\n";
  return std::make_shared<Value::Value>(
      Value::Value(Value::ValueKind::Number, 1));
}
} // namespace Tisp::Runtime::Builtin
