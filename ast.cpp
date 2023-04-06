/***********************************************************************\
*
* Contents:
* Systems: all
*
\***********************************************************************/

/****************************** Includes *******************************/

#include <iostream>
#include <sstream>

#include "ast.h"

using namespace FSM;

/****************** Conditional compilation switches *******************/

/***************************** Constants *******************************/

/***************************** Datatypes *******************************/

/***************************** Variables *******************************/

/***************************** Forwards ********************************/


/***************************** Functions *******************************/

AST::AST()
  : stateList()
{
}

void AST::clear()
{
  stateList.clear();
}

void AST::traverse(Visitor &visitor)
{
  stateList.traverse(visitor);
}

void AST::print() const
{
  std::cout << "AST:" << std::endl;

  std::cout << "stateList:" << std::endl;
  for (const State *state : stateList)
  {
      std::cout << "  " << state->name << ":" << std::endl;
      for (const DeclarationStatement *declarationStatement : *state->compoundStatement->declarationStatementList)
      {
        std::cout << "  " << declarationStatement << std::endl;
      }
      std::cout << std::endl;
  }
}

void AST::addState(FSM::State *state)
{
  stateList.add(state);
}

/* end of file */
