/***********************************************************************\
*
* Contents: .dot file generator
* Systems: all
*
\***********************************************************************/

/****************************** Includes *******************************/

#include <iostream>
#include <sstream>

#include "ast.h"
#include "visitor.h"

#include "dotgenerator.h"

using namespace FSM;

/****************** Conditional compilation switches *******************/

/***************************** Constants *******************************/

/***************************** Datatypes *******************************/

/***************************** Variables *******************************/

/***************************** Forwards ********************************/

/***************************** Functions *******************************/
//  INITIAL                                  -> CHECK_MAX_NUM_OF_RRT [color=blue, penwidth=3.0];

void DotGenerator::generate(const AST &ast)
{
  output << "" << std::endl;
  output << "// FSM " << ast.getFSMName() << std::endl;
  output << "digraph" << std::endl;
  output << "{" << std::endl;
  for (const State &state : ast.getStateList())
  {
    ast.doStateTransitions(state.name,[&](const NewStateStatement &newStateStatement)
    {
      output << "  " << state.name << "->" << newStateStatement.name << "[" << "]" << std::endl;
    });
  }
  output << "}" << std::endl;
}

/* end of file */
