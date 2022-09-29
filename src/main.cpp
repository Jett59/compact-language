#include "lexer.h"
#include "parser.hh"
#include <fstream>
#include <iostream>

using namespace compact;

static void usage(char *command) {
  std::cout << "Usage: " << command << " <filename>" << std::endl;
}

int main(int argc, char **argv) {
  if (argc < 2) {
    usage(argv[0]);
    return -1;
  }
  std::string fileName = argv[1];
  std::ifstream input(fileName);
  Lexer lexer(input, fileName);
  std::unique_ptr<AstNode> ast;
  Parser parser(lexer, fileName, &ast);
  int result = parser();
  if (result == 0) {
    std::cout << "Parsed successfully" << std::endl;
  } else {
    std::cout << "Parse failed" << std::endl;
  }
}
