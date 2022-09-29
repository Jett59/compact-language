#ifndef COMPACT_LOCATION_H
#define COMPACT_LOCATION_H

#include "location.hh"
#include <string>

namespace compact {
struct Location {
  size_t line;
  size_t column;
  std::string file;

  Location(const compact::location &location)
      : line(location.begin.line), column(location.begin.column),
        file(location.begin.filename ? *location.begin.filename : "<Unknown>") {
  }
};
} // namespace compact

#endif