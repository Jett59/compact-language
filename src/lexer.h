#ifndef COMPACT_LEXER_H
#define COMPACT_LEXER_H

#include <fstream>

#include "location.hh"

#if !defined(yyFlexLexerOnce)
#include "FlexLexer.h"
#endif

#undef YY_DECL
#define YY_DECL compact::Parser::symbol_type compact::Lexer::next()

#include "parser.hh"

namespace compact {
class Lexer : public yyFlexLexer {
private:
  location currentLocation;

public:
  Lexer(std::istream &input, std::string &fileName)
      : yyFlexLexer(&input) {
        currentLocation.begin.filename = &fileName;
  }
  Parser::symbol_type next();
  location getLocation() { return currentLocation; }

private:
  void updateLocation(const char *token);
};
} // namespace compact

#endif