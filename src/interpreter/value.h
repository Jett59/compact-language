#ifndef COMPACT_INTERPRETER_VALUE_H
#define COMPACT_INTERPRETER_VALUE_H

#include <iostream>
#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace compact {
enum class ValueType { NUMBER, STRING, BOOLEAN, LIST, FUNCTION, NONE };
struct None {};

class AstNode;
struct Context;
struct Value;

struct Function {
  std::vector<std::string> parameterNames;
  AstNode *body;
  std::shared_ptr<Context> context;

  Value operator()(std::vector<Value> parameters) const;
};

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
template <> struct ValueAlternatives<ValueType::FUNCTION> {
  using Type = Function;
};

struct Value {
  std::variant<None, double, std::string, bool, std::vector<Value>, Function> value;

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
    case ValueType::FUNCTION:
      return std::holds_alternative<Function>(value);
    case ValueType::NONE:
      return std::holds_alternative<None>(value);
    }
  }

  template <ValueType type> const auto &get() const {
    return std::get<typename ValueAlternatives<type>::Type>(value);
  }
};

static inline void printValue(const Value &value) {
  if (value.is<ValueType::NUMBER>()) {
    std::cout << value.get<ValueType::NUMBER>();
  } else if (value.is<ValueType::STRING>()) {
    std::cout << value.get<ValueType::STRING>();
  } else if (value.is<ValueType::BOOLEAN>()) {
    std::cout << (value.get<ValueType::BOOLEAN>() ? "true" : "false");
  } else if (value.is<ValueType::LIST>()) {
    std::cout << "[";
    for (auto &item : value.get<ValueType::LIST>()) {
      printValue(item);
      std::cout << ", ";
    }
    std::cout << "]";
  } else if (value.is<ValueType::FUNCTION>()) {
    std::cout << "<function>";
  }else if (value.is<ValueType::NONE>()) {
    std::cout << "none";
  }
}
} // namespace compact

#endif