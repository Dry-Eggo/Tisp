#pragma once

#include <variant>
#include <vector>
#include <string>
#include <cstdint>

namespace Tisp {
    namespace Value {
	enum class ValueKind {
	    Number,
	    String,
	};

	struct Value;
	typedef std::variant<
	std::string,
	int64_t> ValueData; 
	struct Value {
	    ValueKind kind;
	    ValueData data;
	};
    } // Value
} // Tisp
