/***********************************************************************\
*
* Contents: .dot file generator
* Systems: all
*
\***********************************************************************/

/****************************** Includes *******************************/

#include <iostream>
#include <sstream>
#include <unordered_set>

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
    ast.doStateTransitions(state,[&](const NewStateStatement &newStateStatement)
    {
      if (state->type != State::Type::DEFAULT)
      {
        switch (newStateStatement.type)
        {
          case NewStateStatement::Type::START:
            output << "  " << state->name << " -> " << ast.getStartState() << " " << "[" << optionsToString(newStateStatement) << "]" << std::endl;
            break;
          case NewStateStatement::Type::DEFAULT:
            output << "  " << state->name << " -> " << ast.getDefaultState() << " " << "[" << optionsToString(newStateStatement) << "]" << std::endl;
            break;
          case NewStateStatement::Type::POP:
            {
              for (const State *pushState : getPushStates(ast, ast.getState(state->name)))
              {
                output << "  " << state->name << " -> " << pushState->name << " " << "[" << optionsToString(newStateStatement) << "]" << std::endl;
              }
            }
            break;
          case NewStateStatement::Type::CUSTOM:
            output << "  " << state->name << " -> " << newStateStatement.name << " " << "[" << optionsToString(newStateStatement) << "]" << std::endl;
            break;
        }
      }
    });
  }
  output << "}" << std::endl;
}

std::vector<const State*> DotGenerator::getPushStates(const AST &ast, const State *state) const
{
  std::vector<const State*> pushStates;

  std::unordered_set<const State*> checkStates{state};
  std::unordered_set<const State*> processedStates{};

  while (!checkStates.empty())
  {
    // get next state to process
    std::unordered_set<const State*>::const_iterator iterator = checkStates.begin();
    const State *checkState = *iterator;
    checkStates.erase(iterator);
//fprintf(stderr,"%s:%d: checkState=%s\n",__FILE__,__LINE__,checkState->name.c_str());

    // check all transitions to state
    ast.doStates(checkState,[&](const State *fromState, const NewStateStatement &newStateStatement)
    {
//fprintf(stderr,"%s:%d:   fromState=%s push=%d\n",__FILE__,__LINE__,fromState->name.c_str(),(newStateStatement.prefixOperator == NewStateStatement::PrefixOperator::PUSH));
      if (newStateStatement.prefixOperator == NewStateStatement::PrefixOperator::PUSH)
      {
        switch (fromState->type)
        {
          case State::Type::START:
          case State::Type::DEFAULT:
          case State::Type::CUSTOM:
            pushStates.push_back(fromState);
            break;
        }
      }

      if (processedStates.find(fromState) == processedStates.end())
      {
        checkStates.insert(fromState);
      }
    });

    // add to processed set
    processedStates.insert(checkState);
  }

  return pushStates;
}


/* end of file */
