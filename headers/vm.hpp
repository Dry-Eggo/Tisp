#pragma once

#include <memory>
#include <parser.hpp>
#include <unordered_map>
#include <value.hpp>

using namespace Tisp::Language;

namespace Tisp {
    namespace Runtime {	
	struct Env {
	    using namespace::Value;
	    std::unordered_map<std::string, std::shared_ptr<Value::Value>> variables;
	    void set(std::string name, ValuePtr value) {
		variables[name] = value;
	    }
	    ValuePtr get(std::string name) {
		if (variables.count(name) == 0) {
		    return Value::make_error("Variable Not Declared");
		}
	    }
	};
	
	struct Vm {
	    using NativeFn = std::function<Value::ValuePtr(Vm* vm, Value::Args args)>;
	    ErrorManager*                              error_manager;
	    Language::Node                             program;
	    Env                                        env;
	    std::unordered_map<std::string, NativeFn>  builtins;
	    
	    Vm(Language::Node program, ErrorManager* em);
	    void execute();
	    void execute_node(NodeStmt* node);
	    Value::ValuePtr generate_value(NodeExpr* expr);
	    Value::ValuePtr handle_call(NodeCall *call);
	    Value::Args to_values(std::vector<Exprptr> &args);
	};
    } // namespace Runtime
} // namespace Tisp
