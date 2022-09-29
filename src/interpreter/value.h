#ifndef COMPACT_INTERPRETER_VALUE_H
#define COMPACT_INTERPRETER_VALUE_H

#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace compact {
enum class ValueType { NUMBER, STRING, BOOLEAN, LIST, NONE };
struct None {};

struct Value;

template <ValueType type> struct ValueAlternatives { using Type = None; };

template <> struct ValueAlternatives<ValueType::NUMBER> {
  using Type = double;
};
template <> struct ValueAlternatives<ValueType::STRING> {
  using Type = std::string;
};
template <> struct ValueAlternatives<ValueType::BOOLEAN> { using Type = bool; };
template <> struct ValueAlternatives<ValueType::LIST> {
  using Type = std::vector<Value>;
};

struct Value {
  std::variant<None, double, std::string, bool, std::vector<Value>> value;

  template <ValueType type> bool is() const {
    switch (type) {
    case ValueType::NUMBER:
      return std::holds_alternative<double>(value);
    case ValueType::STRING:
      return std::holds_alternative<std::string>(value);
    case ValueType::BOOLEAN:
      return std::holds_alternative<bool>(value);
    case ValueType::LIST:
      return std::holds_alternative<std::vector<Value>>(value);
    case ValueType::NONE:
      return std::holds_alternative<None>(value);
    }
  }

  template <ValueType type> auto get() const {
    return std::get<typename ValueAlternatives<type>::Type>(value);
  }
};
} // namespace compact

#endif