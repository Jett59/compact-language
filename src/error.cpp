#include "error.h"
#include <iostream>

namespace compact {
void error(const std::string &fileName, size_t line, size_t column,
           const std::string &message) {
  std::cout << fileName << ":" << line << ":" << column << ":" << std::endl;
  std::cout << "Error: " << message << std::endl;
}
void warn(const std::string &fileName, size_t line, size_t column,
          const std::string &message) {
  std::cout << fileName << ":" << line << ":" << column << ":" << std::endl;
  std::cout << "Warning: " << message << std::endl;
}
} // namespace zips
