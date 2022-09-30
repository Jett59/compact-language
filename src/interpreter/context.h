#ifndef COMPACT_INTERPRETER_CONTEXT_H
#define COMPACT_INTERPRETER_CONTEXT_H

#include "value.h"
#include <map>
#include <string>

namespace compact {
struct Context {
  std::map<std::string, std::shared_ptr<Value>> variables;
};
} // namespace compact

#endif