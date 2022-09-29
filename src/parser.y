%require "3.8"
%language "c++"
%define api.token.constructor
%define api.value.type variant
%define api.value.automove
%define parse.lac full

%define api.namespace { compact }

%define api.parser.class  { Parser }

%code requires {
    #include "ast.h"
    #include <memory>

    namespace compact {
        class Lexer;
    }
}

%debug

%locations

%define parse.error verbose

%{

#include <cstdint>
#include <iostream>
#include <vector>
#include "error.h"
#include "ast.h"
#include "lexer.h"

compact::Parser::symbol_type yylex(compact::Lexer& lexer) {
    return lexer.next();
}

using std::make_unique;

%}

%lex-param { compact::Lexer& lexer }
%parse-param { compact::Lexer& lexer }
%parse-param { std::string fileName }
%parse-param {std::unique_ptr<compact::AstNode> *ast}

%initial-action {
    // Set the file name on the initial location (goes into compilation-unit).
    @$.initialize(&fileName);
}

%token <std::string> IDENTIFIER "identifier"
%token <double> NUMBER "number"

%token LEFT_PAREN "(" RIGHT_PAREN ")"
%token COLON ":" SEMICOLON ";"
%token COMMA "," DOT "."
%token PLUS "+" MINUS "-" STAR "*" SLASH "/" PERCENT "%"
%token TILDE "~" CARET "^" BANG "!"

%token END 0 "EOF"

%type <std::unique_ptr<AstNode>> expression

%left "!"
%left "+" "-"
%left "*" "/" "%"
%left "^"
%left "~"

%start compilation-unit

%%

compilation-unit: expression {
    *ast = $1;
}

expression:
IDENTIFIER {
    $$ = make_unique<VariableReferenceNode>(@1, $1);
}
| NUMBER {
    $$ = make_unique<NumberNode>(@1, $1);
}
| "(" expression ")" {
    $$ = $2;
}
| expression "+" expression {
    $$ = make_unique<BinaryExpressionNode>(@2, BinaryOperator::ADD, $1, $3);
}
| expression "-" expression {
    $$ = make_unique<BinaryExpressionNode>(@2, BinaryOperator::SUBTRACT, $1, $3);
}
| expression "*" expression {
    $$ = make_unique<BinaryExpressionNode>(@2, BinaryOperator::MULTIPLY, $1, $3);
}
| expression "/" expression {
$$ = make_unique<BinaryExpressionNode>(@2, BinaryOperator::DIVIDE, $1, $3);
}
| expression "%" expression {
    $$ = make_unique<BinaryExpressionNode>(@2, BinaryOperator::MODULO, $1, $3);
}
| expression "~" expression {
    $$ = make_unique<BinaryExpressionNode>(@2, BinaryOperator::RANGE, $1, $3);
}
| expression "^" expression {
    $$ = make_unique<BinaryExpressionNode>(@2, BinaryOperator::POWER, $1, $3);
}
| expression "!" expression {
    $$ = make_unique<BinaryExpressionNode>(@2, BinaryOperator::NOT_EQUAL, $1, $3);
}

%%

void compact::Parser::error(const compact::location& location, const std::string& message) {
    compact::error(fileName, location.begin.line, location.begin.column, message);
}
