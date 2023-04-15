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
    { "if",       FSM::Parser::token::yytokentype::TOKEN_KEYWORD_IF       },
    { "else",     FSM::Parser::token::yytokentype::TOKEN_KEYWORD_ELSE     },
    { "for",      FSM::Parser::token::yytokentype::TOKEN_KEYWORD_FOR      },
    { "while",    FSM::Parser::token::yytokentype::TOKEN_KEYWORD_WHILE    },
    { "do",       FSM::Parser::token::yytokentype::TOKEN_KEYWORD_DO       },
    { "switch",   FSM::Parser::token::yytokentype::TOKEN_KEYWORD_SWITCH   },
    { "case",     FSM::Parser::token::yytokentype::TOKEN_KEYWORD_CASE     },
    { "default",  FSM::Parser::token::yytokentype::TOKEN_KEYWORD_DEFAULT  },
    { "break",    FSM::Parser::token::yytokentype::TOKEN_KEYWORD_BREAK    },
    { "continue", FSM::Parser::token::yytokentype::TOKEN_KEYWORD_CONTINUE },
    { "return",   FSM::Parser::token::yytokentype::TOKEN_KEYWORD_RETURN   },
    { "sizeof",   FSM::Parser::token::yytokentype::TOKEN_KEYWORD_SIZEOF   },

    { "auto",     FSM::Parser::token::yytokentype::TOKEN_KEYWORD_AUTO     },
    { "register", FSM::Parser::token::yytokentype::TOKEN_KEYWORD_REGISTER },
    { "static",   FSM::Parser::token::yytokentype::TOKEN_KEYWORD_STATIC   },
    { "extern",   FSM::Parser::token::yytokentype::TOKEN_KEYWORD_EXTERN   },
    { "typedef",  FSM::Parser::token::yytokentype::TOKEN_KEYWORD_TYPEDEF  },

    { "void",     FSM::Parser::token::yytokentype::TOKEN_KEYWORD_VOID     },
    { "char",     FSM::Parser::token::yytokentype::TOKEN_KEYWORD_CHAR     },
    { "short",    FSM::Parser::token::yytokentype::TOKEN_KEYWORD_SHORT    },
    { "int",      FSM::Parser::token::yytokentype::TOKEN_KEYWORD_INT      },
    { "long",     FSM::Parser::token::yytokentype::TOKEN_KEYWORD_LONG     },
    { "float",    FSM::Parser::token::yytokentype::TOKEN_KEYWORD_FLOAT    },
    { "double",   FSM::Parser::token::yytokentype::TOKEN_KEYWORD_DOUBLE   },
    { "signed",   FSM::Parser::token::yytokentype::TOKEN_KEYWORD_SIGNED   },
    { "unsigned", FSM::Parser::token::yytokentype::TOKEN_KEYWORD_UNSIGNED },

    { "struct",   FSM::Parser::token::yytokentype::TOKEN_KEYWORD_STRUCT   },
    { "union",    FSM::Parser::token::yytokentype::TOKEN_KEYWORD_UNION    },

    { "const",    FSM::Parser::token::yytokentype::TOKEN_KEYWORD_CONST    },
    { "volatile", FSM::Parser::token::yytokentype::TOKEN_KEYWORD_VOLATILE },
    { "struct",   FSM::Parser::token::yytokentype::TOKEN_KEYWORD_STRUCT   },
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

  /* char */
  '.' {
    return FSM::Parser::make_CHAR(yytext[1], currentLocation());
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

  /* special operators */
  "*=" {
    return FSM::Parser::symbol_type(FSM::Parser::token::yytokentype::TOKEN_MULTIPLY_ASSIGN, currentLocation());
  }
  "/=" {
    return FSM::Parser::symbol_type(FSM::Parser::token::yytokentype::TOKEN_DIVIDE_ASSIGN, currentLocation());
  }
  "%=" {
    return FSM::Parser::symbol_type(FSM::Parser::token::yytokentype::TOKEN_MODULO_ASSIGN, currentLocation());
  }
  "+=" {
    return FSM::Parser::symbol_type(FSM::Parser::token::yytokentype::TOKEN_ADD_ASSIGN, currentLocation());
  }
  "-=" {
    return FSM::Parser::symbol_type(FSM::Parser::token::yytokentype::TOKEN_SUB_ASSIGN, currentLocation());
  }
  "<<=" {
    return FSM::Parser::symbol_type(FSM::Parser::token::yytokentype::TOKEN_SHIFT_LEFT_ASSIGN, currentLocation());
  }
  ">>=" {
    return FSM::Parser::symbol_type(FSM::Parser::token::yytokentype::TOKEN_SHIFT_RIGHT_ASSIGN, currentLocation());
  }
  "&=" {
    return FSM::Parser::symbol_type(FSM::Parser::token::yytokentype::TOKEN_XOR_ASSIGN, currentLocation());
  }
  "^=" {
    return FSM::Parser::symbol_type(FSM::Parser::token::yytokentype::TOKEN_AND_ASSIGN, currentLocation());
  }
  "|=" {
    return FSM::Parser::symbol_type(FSM::Parser::token::yytokentype::TOKEN_OR_ASSIGN, currentLocation());
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
  /*
  "<" {
    return FSM::Parser::symbol_type(FSM::Parser::token::yytokentype::TOKEN_LOWER, currentLocation());
  }
  ">" {
    return FSM::Parser::symbol_type(FSM::Parser::token::yytokentype::TOKEN_LOWER_EQUALS, currentLocation());
  }
  */
  "<=" {
    return FSM::Parser::symbol_type(FSM::Parser::token::yytokentype::TOKEN_GREATER, currentLocation());
  }
  ">=" {
    return FSM::Parser::symbol_type(FSM::Parser::token::yytokentype::TOKEN_GREATER_EQUALS, currentLocation());
  }

  "->" {
    return FSM::Parser::symbol_type(FSM::Parser::token::yytokentype::TOKEN_POINTER, currentLocation());
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

  "<<" {
    return FSM::Parser::symbol_type(FSM::Parser::token::yytokentype::TOKEN_SHIFT_LEFT, currentLocation());
  }
  ">>" {
    return FSM::Parser::symbol_type(FSM::Parser::token::yytokentype::TOKEN_SHIFT_RIGHT, currentLocation());
  }

  "..." {
    return FSM::Parser::symbol_type(FSM::Parser::token::yytokentype::TOKEN_ELLIPSIS, currentLocation());
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
  \n        { yy_pop_state();
              first.line++;
              first.column = 0;
            }
  [^\n]+    { /* ignored */ }
}

<BLOCK_COMMENT>
{
  "*/"      { yy_pop_state(); }
  \n        { /* ignored */
              first.line++;
              first.column = 0;
            }
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
