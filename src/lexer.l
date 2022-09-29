%option nodefault
%option noyywrap
%option nounistd
%option never-interactive

%{
    #include <cstdint>
#include "parser.hh"
#include "location.hh"
#include "lexer.h"

#define YY_USER_ACTION updateLocation(yytext);

#define MAKE(TYPE)  Parser::make_ ## TYPE (currentLocation)
#define MAKE_PARAMS(TYPE, ...) Parser::make_ ## TYPE (__VA_ARGS__, currentLocation)

#define yyterminate() MAKE(END)

%}

%option c++
%option yyclass="Lexer"

%%

"\n"|" "|"\t"|"\r"|"\v"|"\f" ;

[a-zA-Z$_][a-zA-Z0-9$_]* { return MAKE_PARAMS(IDENTIFIER, yytext); }
[0-9]+(\.[0-9]*)? { return MAKE_PARAMS(NUMBER, strtod(yytext, nullptr)); }

"(" { return MAKE(LEFT_PAREN); }
")" { return MAKE(RIGHT_PAREN); }

"+" { return MAKE(PLUS); }
"-" { return MAKE(MINUS); }
"*" { return MAKE(STAR); }
"/" { return MAKE(SLASH); }
"%" { return MAKE(PERCENT); }

<<EOF>> return yyterminate();

. {
    std::cerr << "Unknown character " << yytext << std::endl;
    return yyterminate();
}

%%

void compact::Lexer::updateLocation(const char* token) {
    auto fileName = currentLocation.begin.filename;
    currentLocation.begin = currentLocation.end;
size_t tokenLength = strlen(token);
    for (size_t i = 0; i < tokenLength; i ++) {
        char c = token[i];
        if (c == '\n') {
            currentLocation.end.line++;
            currentLocation.end.column = 1;
        }else {
            currentLocation.end.column++;
        }
    }
    currentLocation.begin.filename = currentLocation.end.filename = fileName;
}
