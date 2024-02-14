/***********************************************************************\
*
* Contents: scanner
* Systems: all
*
\***********************************************************************/

%option noinput nounput
%option noyywrap

%option c++
%option yyclass="Scanner"
%option prefix="FSM_"
%option yylineno

%{
  #include <unordered_map>

  #include "parser.h"
  #include "parser.tab.h"
  #include "location.h"

  #include "scanner.h"

  using namespace std;

  // terminate macro
  #define yyterminate() return TOKEN_EOF

  // track current scanner location
  #define YY_USER_ACTION \
    do \
    { \
      yylloc.last.line    = yylloc.first.line; \
      yylloc.last.column  = yylloc.first.column; \
      yylloc.first.line   = lineno(); \
      yylloc.first.column += yyleng; \
    } \
    while (0);

  static std::unordered_map<std::string, int> KEY_WORDS =
  {
    { "if",       KEYWORD_IF       },
    { "else",     KEYWORD_ELSE     },
    { "for",      KEYWORD_FOR      },
    { "while",    KEYWORD_WHILE    },
    { "do",       KEYWORD_DO       },
    { "switch",   KEYWORD_SWITCH   },
    { "case",     KEYWORD_CASE     },
    { "default",  KEYWORD_DEFAULT  },
    { "break",    KEYWORD_BREAK    },
    { "continue", KEYWORD_CONTINUE },
    { "return",   KEYWORD_RETURN   },
    { "sizeof",   KEYWORD_SIZEOF   },

    { "auto",     KEYWORD_AUTO     },
    { "register", KEYWORD_REGISTER },
    { "static",   KEYWORD_STATIC   },
    { "extern",   KEYWORD_EXTERN   },
    { "typedef",  KEYWORD_TYPEDEF  },

    { "void",     KEYWORD_VOID     },
    { "char",     KEYWORD_CHAR     },
    { "short",    KEYWORD_SHORT    },
    { "int",      KEYWORD_INT      },
    { "long",     KEYWORD_LONG     },
    { "float",    KEYWORD_FLOAT    },
    { "double",   KEYWORD_DOUBLE   },
    { "signed",   KEYWORD_SIGNED   },
    { "unsigned", KEYWORD_UNSIGNED },

    { "struct",   KEYWORD_STRUCT   },
    { "union",    KEYWORD_UNION    },

    { "const",    KEYWORD_CONST    },
    { "volatile", KEYWORD_VOLATILE },
    { "struct",   KEYWORD_STRUCT   },
  };

  // get current token location (line, column)
  Location currentLocation()
  {
    Location location;

    location.first.line   = yylloc.first.line;
    location.first.column = yylloc.first.column;
    location.last.line    = yylloc.last.line;
    location.last.column  = yylloc.last.column;

    return location;
  }
%}

/* scanner state list */
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
    yylloc.first.line++;
    yylloc.first.column = 0;
  }

  "//" {
    yy_push_state(LINE_COMMENT);
  }
  "/*" {
    yy_push_state(BLOCK_COMMENT);
  }

  /* FSM begin/end */
  "#fsm" {
    return KEYWORD_FSM;
  }
  "#end" {
    return KEYWORD_END;
  }

  /* keyword or identifier */
  [A-Za-z_][A-Za-z_0-9]* {
    auto keyword = KEY_WORDS.find(yytext);
    if (keyword != KEY_WORDS.end())
    {
//fprintf(stderr,"%s, %d: KEY_WORD %s %d\n",__FILE__,__LINE__,yytext,yylloc.first.line);
      return keyword->second;
    }
    else
    {
//fprintf(stderr,"%s, %d: IDENTIFIER %s %d\n",__FILE__,__LINE__,yytext,yylloc.first.line);
      yylval.string = strdup(yytext);
      return TOKEN_IDENTIFIER;
    }
  }

  /* string */
  "\"" {
    yy_push_state(STRING);
  }

  /* number */
  [0-9]*"."[0-9]+ {
    yylval.float_ = std::stod(yytext, 0);
    return TOKEN_FLOAT;
  }
  [0-9]+"."[0-9]* {
    yylval.float_ = std::stod(yytext, 0);
    return TOKEN_FLOAT;
  }
  [0-9]+ {
    yylval.integer = std::stol(yytext, 0, 10);
    return TOKEN_INTEGER;
  }

  /* special operators */
  "*=" {
    return TOKEN_MULTIPLY_ASSIGN;
  }
  "/=" {
    return TOKEN_DIVIDE_ASSIGN;
  }
  "%=" {
    return TOKEN_MODULO_ASSIGN;
  }
  "+=" {
    return TOKEN_ADD_ASSIGN;
  }
  "-=" {
    return TOKEN_SUB_ASSIGN;
  }
  "<<=" {
    return TOKEN_SHIFT_LEFT_ASSIGN;
  }
  ">>=" {
    return TOKEN_SHIFT_RIGHT_ASSIGN;
  }
  "&=" {
    return TOKEN_XOR_ASSIGN;
  }
  "^=" {
    return TOKEN_AND_ASSIGN;
  }
  "|=" {
    return TOKEN_OR_ASSIGN;
  }

  "++" {
    return TOKEN_INCREMENT;
  }
  "--" {
    return TOKEN_DECREMENT;
  }
  "==" {
    return TOKEN_EQUALS;
  }
  "!=" {
    return TOKEN_NOT_EQUALS;
  }
  /*
  "<" {
    return TOKEN_LOWER;
  }
  ">" {
    return TOKEN_LOWER_EQUALS;
  }
  */
  "<=" {
    return TOKEN_GREATER;
  }
  ">=" {
    return TOKEN_GREATER_EQUALS;
  }

  "->" {
    return TOKEN_POINTER;
  }

  "&&" {
    return TOKEN_AND;
  }
  "||" {
    return TOKEN_OR;
  }
  "^" {
    return TOKEN_XOR;
  }

  "<<" {
    return TOKEN_SHIFT_LEFT;
  }
  ">>" {
    return TOKEN_SHIFT_RIGHT;
  }

  "..." {
    return TOKEN_ELLIPSIS;
  }

  /* any character */
  . {
    return yytext[0];
  }

  /* end of file */
  <<EOF>> {
    return TOKEN_EOF;
  }
}

<LINE_COMMENT>
{
  /* LF */
  \n {
    yylloc.first.line++;
    yylloc.first.column = 0;
    yy_pop_state();
  }

  /* ignored */
  [^\n]+ {
  }
}

<BLOCK_COMMENT>
{
  "*/" {
    yy_pop_state();
  }

  /* LF */
  \n {
    yylloc.first.line++;
    yylloc.first.column = 0;
  }

  /* ignored */
  . {
  }
}

<STRING>
{
  "\"" {
    yy_pop_state();
    yytext[strlen(yytext)-1] = '\0';
    yylval.string = strdup(yytext);
    return TOKEN_STRING;
  }
  [^"] {
    yymore();
  }
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
  return yylloc.last.line;
}

uint Scanner::getColumn()
{
  return yylloc.last.column;
}

} // namespace FSM
