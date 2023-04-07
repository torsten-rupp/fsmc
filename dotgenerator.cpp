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

std::string optionsToString(const NewStateStatement &newStateStatement)
{
  std::stringstream buffer;
  bool              first = true;

  if (!newStateStatement.options.label.empty())
  {
    if (!first) buffer << ", ";
    buffer << "label=\"" << newStateStatement.options.label << "\"";
    first = false;
  }
  if (!newStateStatement.options.color.empty())
  {
    if (!first) buffer << ", ";
    buffer << "color=" << newStateStatement.options.color;
    first = false;
  }
  if (newStateStatement.options.lineWidth > 0)
  {
    if (!first) buffer << ", ";
    buffer << "penwidth=" << newStateStatement.options.lineWidth;
    first = false;
  }

  return buffer.str();
}

void DotGenerator::generate(const AST &ast)
{
  output << "" << std::endl;
  output << "// FSM " << ast.getFSMName() << std::endl;
  output << "digraph" << std::endl;
  output << "{" << std::endl;
  for (const State *state : ast.getStateList())
  {
    ast.doStateTransitions(state->name,[&](const NewStateStatement &newStateStatement)
    {
      if (!state->name.empty())
      {
        switch (newStateStatement.type)
        {
          case NewStateStatement::Type::START:
            output << "  " << state->name << " -> " << ast.getStartState() << " " << "[" << optionsToString(newStateStatement) << "]" << std::endl;
            break;
          case NewStateStatement::Type::DEFAULT:
            output << "  " << state->name << " -> " << ast.getDefaultState() << " " << "[" << optionsToString(newStateStatement) << "]" << std::endl;
            break;
          case NewStateStatement::Type::CUSTOM:
            output << "  " << state->name << " -> " << newStateStatement.name << " " << "[" << optionsToString(newStateStatement) << "]" << std::endl;
            break;
        }
        switch (newStateStatement.prefixOperator)
        {
          case NewStateStatement::PrefixOperator::PUSH:
            switch (newStateStatement.type)
            {
              case NewStateStatement::Type::START:
                output << "  " << ast.getStartState() << " -> " << state->name << " " << "[" << optionsToString(newStateStatement) << "]" << std::endl;
                break;
              case NewStateStatement::Type::DEFAULT:
                output << "  " << ast.getDefaultState() << " -> " << state->name << " " << "[" << optionsToString(newStateStatement) << "]" << std::endl;
                break;
              case NewStateStatement::Type::CUSTOM:
                output << "  " << newStateStatement.name << " -> " << state->name << " " << "[" << optionsToString(newStateStatement) << "]" << std::endl;
                break;
            }
            break;
        }
      }
    });
  }
  output << "}" << std::endl;
}

/* end of file */
