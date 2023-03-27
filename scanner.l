%option noinput nounput
%option noyywrap

%option c++
%option yyclass="Scanner"
%option prefix="FSM_"
%option yylineno

%{
  #include <unordered_map>

  #include "scanner.h"
  #include "parser.hpp"
  #include "position.hh"
  #include "location.hh"

  using namespace std;

  // terminate macro
  #define yyterminate() Parser::make_END(location());

  // track current scanner location
  #define YY_USER_ACTION \
    do \
    { \
      last.line    = first.line; \
      last.column  = first.column; \
      first.line   = lineno(); \
      first.column += yyleng; \
    } \
    while (0);

  static std::unordered_map<std::string, FSM::Parser::token::yytokentype> KEY_WORDS =
  {
    { "if",    FSM::Parser::token::yytokentype::TOKEN_KEYWORD_IF    },
    { "else",  FSM::Parser::token::yytokentype::TOKEN_KEYWORD_ELSE  },
    { "for",   FSM::Parser::token::yytokentype::TOKEN_KEYWORD_FOR   },
    { "while", FSM::Parser::token::yytokentype::TOKEN_KEYWORD_WHILE },
    { "do",    FSM::Parser::token::yytokentype::TOKEN_KEYWORD_DO    },
  };   

  static struct
  {
    uint line;
    uint column;   
  } first, last;
  
  // get current token location (line, column)
  static FSM::location currentLocation()
  {
    return FSM::location(FSM::position(YY_NULLPTR,first.line,first.column));
  } 
%}

/* stateList */
%s CONDITIONS EVENTS
%s LINE_COMMENT BLOCK_COMMENT
%s STRING

%%

<INITIAL>
{
  /* ignore white spaces */
  [\t ] {
  }

  /* LF */
  \n {
    first.line++;
    first.column = 0;
  }

  "//" {
    yy_push_state(LINE_COMMENT);
  }
  "/*" {
    yy_push_state(BLOCK_COMMENT);
  }

  /* FSM begin/end */
  "#fsm" {
    return FSM::Parser::symbol_type(FSM::Parser::token::yytokentype::TOKEN_KEYWORD_FSM, currentLocation());
  }
  "#end" {
    return FSM::Parser::symbol_type(FSM::Parser::token::yytokentype::TOKEN_KEYWORD_END, currentLocation());
  }

  /* keyword or identifier */
  [A-Za-z_][A-Za-z_0-9]* {
    auto keyword = KEY_WORDS.find(yytext);
    if (keyword != KEY_WORDS.end())
    {
//fprintf(stderr,"%s, %d: KEY_WORD %s\n",__FILE__,__LINE__,yytext);
      return FSM::Parser::symbol_type(keyword->second, currentLocation());
    }
    else
    {
//fprintf(stderr,"%s, %d: IDENTIFIER %s\n",__FILE__,__LINE__,yytext);
      return FSM::Parser::make_IDENTIFIER(FSM::Identifier(yytext), currentLocation()); 
    }
  }

  /* string */
  "\"" {
    yy_push_state(STRING);
  }

  /* number */
  [0-9]*"."[0-9]+ {
    double number = std::stod(yytext, 0);
    return FSM::Parser::make_FLOAT(number, currentLocation());
  }
  [0-9]+"."[0-9]* {
    double number = std::stod(yytext, 0);
    return FSM::Parser::make_INTEGER(number, currentLocation());
  }
  [0-9]+ {
    uint64_t number = std::stol(yytext, 0, 10);
    return FSM::Parser::make_INTEGER(number, currentLocation());
  }

  /* operators */
  "=" {
    return FSM::Parser::symbol_type(FSM::Parser::token::yytokentype::TOKEN_ASSIGN, currentLocation());
  }

  "+" {
    return FSM::Parser::symbol_type(FSM::Parser::token::yytokentype::TOKEN_PLUS, currentLocation());
  }
  "-" {
    return FSM::Parser::symbol_type(FSM::Parser::token::yytokentype::TOKEN_MINUS, currentLocation());
  }
  "*" {
    return FSM::Parser::symbol_type(FSM::Parser::token::yytokentype::TOKEN_MULTIPLY, currentLocation());
  }
  "/" {
    return FSM::Parser::symbol_type(FSM::Parser::token::yytokentype::TOKEN_DIVIDE, currentLocation());
  }

  "++" {
    return FSM::Parser::symbol_type(FSM::Parser::token::yytokentype::TOKEN_INCREMENT, currentLocation());
  }
  "--" {
    return FSM::Parser::symbol_type(FSM::Parser::token::yytokentype::TOKEN_DECREMENT, currentLocation());
  }
  "==" {
    return FSM::Parser::symbol_type(FSM::Parser::token::yytokentype::TOKEN_EQUALS, currentLocation());
  }
  "!=" {
    return FSM::Parser::symbol_type(FSM::Parser::token::yytokentype::TOKEN_NOT_EQUALS, currentLocation());
  }
  "<" {
    return FSM::Parser::symbol_type(FSM::Parser::token::yytokentype::TOKEN_LOWER, currentLocation());
  }
  ">" {
    return FSM::Parser::symbol_type(FSM::Parser::token::yytokentype::TOKEN_LOWER_EQUALS, currentLocation());
  }
  "<=" {
    return FSM::Parser::symbol_type(FSM::Parser::token::yytokentype::TOKEN_GREATER, currentLocation());
  }
  ">=" {
    return FSM::Parser::symbol_type(FSM::Parser::token::yytokentype::TOKEN_GREATER_EQUALS, currentLocation());
  }

  "->" {
    return FSM::Parser::symbol_type(FSM::Parser::token::yytokentype::TOKEN_DEREFERENCE, currentLocation());
  }

  "!" {
    return FSM::Parser::symbol_type(FSM::Parser::token::yytokentype::TOKEN_NOT, currentLocation());
  }
  "&&" {
    return FSM::Parser::symbol_type(FSM::Parser::token::yytokentype::TOKEN_AND, currentLocation());
  }
  "||" {
    return FSM::Parser::symbol_type(FSM::Parser::token::yytokentype::TOKEN_OR, currentLocation());
  }
  "^" {
    return FSM::Parser::symbol_type(FSM::Parser::token::yytokentype::TOKEN_XOR, currentLocation());
  }

  /* any character */
  . {
    return FSM::Parser::symbol_type(static_cast<FSM::Parser::token::yytokentype>(yytext[0]), currentLocation());
  }

  /* end of file */
  <<EOF>> {
    return FSM::Parser::symbol_type(FSM::Parser::token::yytokentype::TOKEN_EOF, currentLocation());
  }
}

<LINE_COMMENT>
{
  \n        { yy_pop_state(); }
  [^\n]+    { /* ignored */ }
}

<BLOCK_COMMENT>
{
  "*/"      { yy_pop_state(); }
  \n        { /* ignored */ }
  .         { /* ignored */ }
}

<STRING>
{
  "\""      { yy_pop_state();
              yytext[strlen(yytext)-1] = '\0';
              return FSM::Parser::make_STRING(yytext, currentLocation()); 
            }
  [^"]      { yymore(); }
}

%%

namespace FSM
{

void Scanner::setLineNumber(uint lineNb)
{
  yylineno = lineNb;
}

std::string Scanner::getFileName()
{
return "";
}

uint Scanner::getLine()
{
    return last.line;
}

uint Scanner::getColumn()
{
    return last.column;
}

}  // namespace FSM
