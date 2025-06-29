#include "value.hpp"
#include <builtins.hpp>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <assert.h>
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
    Value::ValuePtr exec(Vm *vm, std::vector<Value::ValuePtr> args) {
	assert(args.size() == 1);
	auto arg = args.at(0);
	assert(arg->kind == Value::ValueKind::String);

	auto cmd = std::get<std::string>(arg->data);
    return std::make_shared<Value::Value>(Value::Value(Value::ValueKind::Number,
	system(cmd.c_str())));
    }
    Value::ValuePtr print(Vm *vm, std::vector<Value::ValuePtr> args) {
	return std::make_shared<Value::Value>(
	Value::Value(Value::ValueKind::Number, 1));
    }
} // namespace Tisp::Runtime::Builtin
