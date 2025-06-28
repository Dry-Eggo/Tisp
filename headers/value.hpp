#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace Tisp {
namespace Value {
enum class ValueKind {
  Number,
  String,
  Object,
};

struct Value;
using ValuePtr = std::shared_ptr<Value>;
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
};
} // namespace Value
} // namespace Tisp
