#include "lexer.h"
#include "parser.hh"
#include <fstream>
#include <iostream>

using namespace compact;

static void printValue(const Value &value) {
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
  }
}

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
    try {
      Context context;
      Value programOutput = ast->evaluate(context);
      printValue(programOutput);
      std::cout << std::endl;
    } catch (const CompactError &e) {
      error(e);
      return -1;
    }
  } else {
    std::cout << "Parse failed" << std::endl;
    return -1;
  }
}
