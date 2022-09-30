#ifndef COMPACT_AST_H
#define COMPACT_AST_H

#include "error.h"
#include "interpreter/context.h"
#include "interpreter/value.h"
#include "location.h"
#include <algorithm>
#include <cmath>
#include <memory>
#include <string>

namespace compact {
enum class AstNodeType {
  VARIABLE_REFERENCE,
  NUMBER,
  BINARY_EXPRESSION,
  FUNCTION_EXPRESSION,
  FILTER_EXPRESSION
};

class AstNode {
  Location location;
  AstNodeType nodeType;

public:
  AstNode(Location location, AstNodeType type)
      : location(location), nodeType(nodeType) {}
  virtual ~AstNode() {}

  const Location &getLocation() const { return location; }
  AstNodeType getNodeType() const { return nodeType; }

  virtual Value evaluate(Context &context) = 0;
};

class VariableReferenceNode : public AstNode {
  std::string name;

public:
  VariableReferenceNode(Location location, std::string name)
      : AstNode(location, AstNodeType::VARIABLE_REFERENCE), name(name) {}

  const std::string &getName() const { return name; }

  Value evaluate(Context &context) override {
    using std::literals::string_literals::operator""s;
    if (context.variables.contains(name)) {
      return *context.variables[name];
    } else {
      throw CompactError(getLocation(),
                         "Variable "s + name + " is not defined"s);
    }
  }
};

class NumberNode : public AstNode {
  double value;

public:
  NumberNode(Location location, double value)
      : AstNode(location, AstNodeType::NUMBER), value(value) {}

  double getValue() const { return value; }

  Value evaluate(Context &context) override { return Value(value); }
};

enum class BinaryOperator {
  ADD,
  SUBTRACT,
  MULTIPLY,
  DIVIDE,
  MODULO,
  RANGE,
  POWER,
  NOT_EQUAL
};

class BinaryExpressionNode : public AstNode {
  std::unique_ptr<AstNode> left;
  std::unique_ptr<AstNode> right;
  BinaryOperator op;

public:
  BinaryExpressionNode(Location location, BinaryOperator op,
                       std::unique_ptr<AstNode> left,
                       std::unique_ptr<AstNode> right)
      : AstNode(location, AstNodeType::BINARY_EXPRESSION), op(op),
        left(std::move(left)), right(std::move(right)) {}

  const AstNode &getLeft() const { return *left; }
  const AstNode &getRight() const { return *right; }
  BinaryOperator getOperator() const { return op; }

  Value evaluate(const Value &leftValue, const Value &rightValue,
                 Context &context) {
    if (leftValue.is<ValueType::NUMBER>() &&
        rightValue.is<ValueType::NUMBER>()) {
      double leftNumber = leftValue.get<ValueType::NUMBER>();
      double rightNumber = rightValue.get<ValueType::NUMBER>();
      switch (op) {
      case BinaryOperator::ADD:
        return Value(leftNumber + rightNumber);
      case BinaryOperator::SUBTRACT:
        return Value(leftNumber - rightNumber);
      case BinaryOperator::MULTIPLY:
        return Value(leftNumber * rightNumber);
      case BinaryOperator::DIVIDE:
        return Value(leftNumber / rightNumber);
      case BinaryOperator::MODULO:
        return Value(std::fmod(leftNumber, rightNumber));
      case BinaryOperator::RANGE: {
        if (static_cast<uintmax_t>(leftNumber) == leftNumber &&
            static_cast<uintmax_t>(rightNumber) == rightNumber) {
              uintmax_t leftInt = static_cast<uintmax_t>(leftNumber);
              uintmax_t rightInt = static_cast<uintmax_t>(rightNumber);
          std::vector<Value> values;
          values.reserve(rightInt - leftInt);
          for (auto i = leftInt; i < rightInt; i++) {
            values.push_back(Value(static_cast<double>(i)));
          }
          return Value(values);
        } else {
          throw CompactError(getLocation(),
                             "Range must be between two integers");
        }
      }
      case BinaryOperator::POWER: {
        return Value(std::pow(leftNumber, rightNumber));
      }
      case BinaryOperator::NOT_EQUAL: {
        return Value(static_cast<bool>(leftNumber != rightNumber));
      }
      default:
        throw CompactError(getLocation(), "Unknown binary operator");
      }
    } else if (leftValue.is<ValueType::LIST>()) {
      auto leftValues = leftValue.get<ValueType::LIST>();
      std::vector<Value> resultValues;
      for (auto &leftValue : leftValues) {
        resultValues.push_back(evaluate(leftValue, rightValue, context));
      }
      return Value(resultValues);
    } else if (rightValue.is<ValueType::LIST>()) {
      auto rightValues = rightValue.get<ValueType::LIST>();
      std::vector<Value> resultValues;
      for (auto &rightValue : rightValues) {
        resultValues.push_back(evaluate(leftValue, rightValue, context));
      }
      return Value(resultValues);
    } else {
      throw CompactError(getLocation(), "Invalid operands for binary operator");
    }
  }

  Value evaluate(Context &context) override {
    return evaluate(left->evaluate(context), right->evaluate(context), context);
  }
};

class FunctionExpressionNode : public AstNode {
  std::vector<std::string> parameterNames;
  std::unique_ptr<AstNode> body;

public:
  FunctionExpressionNode(Location location,
                         std::vector<std::string> parameterNames,
                         std::unique_ptr<AstNode> body)
      : AstNode(location, AstNodeType::FUNCTION_EXPRESSION),
        parameterNames(std::move(parameterNames)), body(std::move(body)) {}

  const std::vector<std::string> &getParameterNames() const {
    return parameterNames;
  }
  const AstNode &getBody() const { return *body; }

  Value evaluate(Context &context) override {
    return Value(Function{parameterNames, body.get(),
                          std::make_shared<Context>(context)});
  }
};
class FilterExpressionNode : public AstNode {
  std::unique_ptr<AstNode> left;
  std::unique_ptr<AstNode> right;

public:
  FilterExpressionNode(Location location, std::unique_ptr<AstNode> left,
                       std::unique_ptr<AstNode> right)
      : AstNode(location, AstNodeType::FILTER_EXPRESSION),
        left(std::move(left)), right(std::move(right)) {}

  const AstNode &getLeft() const { return *left; }
  const AstNode &getRight() const { return *right; }

  Value evaluate(Context &context) override {
    auto leftValue = left->evaluate(context);
    auto rightValue = right->evaluate(context);
    if (leftValue.is<ValueType::LIST>() &&
        rightValue.is<ValueType::FUNCTION>()) {
      auto leftValues = leftValue.get<ValueType::LIST>();
      auto rightFunction = rightValue.get<ValueType::FUNCTION>();
      std::vector<Value> resultValues;
      for (auto &leftValue : leftValues) {
        Value filterResult = rightFunction({leftValue});
        if (!(filterResult.is<ValueType::BOOLEAN>() ||
              filterResult.is<ValueType::LIST>())) {
          throw CompactError(
              getLocation(),
              "Filter function must return a boolean or list of booleans");
        }
        if ((filterResult.is<ValueType::BOOLEAN>() &&
             filterResult.get<ValueType::BOOLEAN>()) ||
            (filterResult.is<ValueType::LIST>() &&
             std::all_of(
                 filterResult.get<ValueType::LIST>().begin(),
                 filterResult.get<ValueType::LIST>().end(),
                 [this](const Value &value) {
                   if (!value.is<ValueType::BOOLEAN>()) {
                     throw CompactError(
                         getLocation(),
                         "Filter function must return a boolean or list "
                         "of booleans");
                   }
                   return value.get<ValueType::BOOLEAN>();
                 }))) {
          resultValues.push_back(leftValue);
        }
      }
      return Value(resultValues);
    } else {
      throw CompactError(getLocation(), "Invalid operands for filter operator");
    }
  }
};
} // namespace compact

#endif