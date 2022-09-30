#include "value.h"
#include "ast.h"

namespace compact {
Value Function::operator()(std::vector<Value> parameters) const {
  for (size_t i = 0; i < parameterNames.size(); i++) {
    context->variables[parameterNames[i]] =
        std::make_shared<Value>(parameters[i]);
  }
  return body->evaluate(*context);
}
} // namespace compact
