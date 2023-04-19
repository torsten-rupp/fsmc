/***********************************************************************\
*
* Contents: 
* Systems: all
*
\***********************************************************************/

/****************************** Includes *******************************/
#include <stdio.h>
#include <stdarg.h>

#include <iostream>

#include "location.h"
#include "scanner.h"
#include "parser.h"
#include "ast.h"

using namespace FSM;

/****************** Conditional compilation switches *******************/

/***************************** Constants *******************************/

/***************************** Datatypes *******************************/

/***************************** Variables *******************************/

/***************************** Forwards ********************************/


/***************************** Functions *******************************/

#include "parser.tab.c"

Parser::Parser(const std::string &inputFilePath, FSM::Scanner &scanner, FSM::AST &ast, bool debug)
{
  extern int yydebug;

  ::inputFilePath = &inputFilePath;
  ::scanner = &scanner;
  ::ast = &ast;
  ::yydebug = debug ? 1 : 0;
}

void Parser::parse()
{
  if (::yyparse() != 0)
  {
    throw std::runtime_error("parsing failed");
  }
}

/* end of file */
