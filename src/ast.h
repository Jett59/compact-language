#ifndef COMPACT_AST_H
#define COMPACT_AST_H

#include "error.h"
#include "interpreter/context.h"
#include "interpreter/value.h"
#include "location.h"
#include <cmath>
#include <memory>
#include <string>

namespace compact {
enum class AstNodeType { VARIABLE_REFERENCE, NUMBER, BINARY_EXPRESSION };

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
      return context.variables[name];
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
          std::vector<Value> values;
          if (static_cast<int>(leftNumber) == leftNumber &&
              static_cast<int>(rightNumber) == rightNumber) {
            for (int i = static_cast<int>(leftNumber);
                 i <= static_cast<int>(rightNumber); i++) {
              values.push_back(Value(static_cast<double>(i)));
            }
          } else {
            throw CompactError(getLocation(), "Range must be between two integers");
          }
          return Value(values);
        }
        case BinaryOperator::POWER: {
          return Value(std::pow(leftNumber, rightNumber));
        }
        case BinaryOperator::NOT_EQUAL: {
          return Value(leftNumber != rightNumber);
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
} // namespace compact

#endif