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

AST::AST() :
  stateList()
{
}

void AST::clear()
{
  stateList.clear();
}

void AST::print() const
{
  std::cout << "AST:" << std::endl;

  std::cout << "stateList:" << std::endl;
  for (const State &state : stateList)
  {
      std::cout << "  " << state.name << ":" << std::endl;
      for (const Statement *statement : state.statementList)
      {
        std::cout << "  " << statement << std::endl;
      }
      std::cout << std::endl;
  }
}

void AST::addState(FSM::State state)
{
  stateList.add(state);
}

void Visitor::run(const AST &ast)
{
  ast.stateList.accept(*this);
}

/* end of file */
