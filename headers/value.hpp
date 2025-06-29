#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <variant>
#include <vector>
#include <functional>

namespace Tisp {
    namespace Value {
	enum class ValueKind {
	    Number,
	    String,
	    Object,
	    NativeFn,
	    Error,
	};

	struct Value;

	using ValuePtr = std::shared_ptr<Value>;
	using Args = std::vector<ValuePtr>;
	typedef std::variant<std::string, int64_t, std::pair<const char *, ValuePtr>>
	ValueData;

	struct Value {
	    ValueKind kind;
	    ValueData data;
	    Value(ValuePtr p) : kind(p->kind), data(std::move(p->data)) {}
	    Value(ValueKind k, ValueData d) : kind(k), data(std::move(d)) {}
	    static ValuePtr obj_from_Value(const char *name, ValuePtr value) {
		return std::make_shared<Value>(
		Value(ValueKind::Object, std::pair(name, std::move(value))));
	    }
	    
	    static ValuePtr make_error(std::string error_message) {
		return std::make_shared<Value>(ValueKind::Error, std::move(error_message));
	    }
	    
	    static ValuePtr make_string(std::string value) {
		return std::make_shared<Value>(ValueKind::String, std::move(value));
	    }
	    
	    static ValuePtr make_int(int64_t number) {
		return std::make_shared<Value>(ValueKind::Number, number);
	    }

	    bool is_truthy() {
		if (kind == ValueKind::Number) {
		    return (std::get<int64_t>(data) > 0);
		}
		if (kind == ValueKind::Object) {
		    return std::get<std::pair<const char*, ValuePtr>>(data).second->is_truthy();
		}
		return false;
	    }
	    
	    bool is_falsy() {
		if (kind == ValueKind::Number) {
		    return (std::get<int64_t>(data) < 0);
		} if (kind == ValueKind::Object) {
		    return std::get<std::pair<const char*, ValuePtr>>(data).second->is_falsy();
		}
		return false;
	    } 
	    
	    std::string to_string() {
		std::string repr;
		if (kind == ValueKind::String) {
		    
		    repr = std::get<std::string>(data);
		    
		} else if (kind == ValueKind::Number) {

		    repr = std::to_string(std::get<int64_t>(data));
		    
		} else if (kind == ValueKind::Object) {

		    repr = std::get<std::pair<const char*, ValuePtr>>(data).second->to_string();
		    
		}
		return repr;
	    }
	};
    } // namespace Value
} // namespace Tisp
