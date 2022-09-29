#ifndef COMPACT_ERROR_H
#define COMPACT_ERROR_H

#include "location.h"
#include   <exception>
#include <string>

namespace compact {
void error(const std::string &fileName, size_t line, size_t column,
           const std::string &msg);

struct CompactError : public std::exception {
  Location location;
  std::string message;
  CompactError(Location location, std::string message)
      : location(location), message(message) {}
  const char *what() const noexcept override { return message.c_str(); }
};

static inline void error(const CompactError &error) {
  compact::error(error.location.file, error.location.line, error.location.column,
              error.message);
}

void warn(const std::string &fileName, size_t line, size_t column,
          const std::string &msg);
static inline void warn(const Location &location, const std::string &message) {
  compact::warn(location.file, location.line, location.column, message);
}
}

#endif