#include "value.hpp"
#include <builtins.hpp>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <assert.h>
namespace Tisp::Runtime::Builtin {
    Value::ValuePtr println(Vm *vm, std::vector<Value::ValuePtr> args) {
	print(vm, args);
	std::cout << "\n";
	return Value::Value::make_int(0);
    }
    Value::ValuePtr exec(Vm *vm, std::vector<Value::ValuePtr> args) {
	assert(args.size() == 1);
	auto arg = args.at(0);
	assert(arg->kind == Value::ValueKind::String);

	auto cmd = arg->to_string();
	// Run Command and return the return value of the command
	return Value::Value::make_int(system(cmd.c_str()));
    }
    Value::ValuePtr print(Vm *vm, std::vector<Value::ValuePtr> args) {
	for(auto arg: args) {
	    std::cout << arg->to_string() << " ";
	}
	return Value::Value::make_int(0);
    }
} // namespace Tisp::Runtime::Builtin
