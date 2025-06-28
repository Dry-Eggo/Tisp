#pragma once
#include <value.hpp>
#include <vector>
#include <vm.hpp>
namespace Tisp::Runtime {
namespace Builtin {
Value::ValuePtr println(Vm *vm, std::vector<Value::ValuePtr> args);
} // namespace Builtin
} // namespace Tisp::Runtime
