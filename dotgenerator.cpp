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

/****************************** Macros *********************************/

/***************************** Functions *******************************/

static std::string optionsToString(const NewStateStatement &newStateStatement)
{
  std::stringstream buffer;
  bool              first = true;

  if (!newStateStatement.options->label.empty())
  {
    if (!first) buffer << ", ";
    buffer << "label=\" " << newStateStatement.options->label << " \"";
    first = false;
  }
  if (!newStateStatement.options->color.empty())
  {
    if (!first) buffer << ", ";
    buffer << "color=" << newStateStatement.options->color;
    first = false;
  }
  if (newStateStatement.options->lineWidth > 0)
  {
    if (!first) buffer << ", ";
    buffer << "penwidth=" << newStateStatement.options->lineWidth;
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
  std::set<std::string> transitionSet;
  for (const State *state : ast.getStateList())
  {
    if (ignoreStates.find(state->name) == ignoreStates.end())
    {
      // initially/finally state transitions
      ast.doStateTransitions([&](const NewStateStatement &newStateStatement)
      {
        if (   !state->name.empty()
            && (state->name != newStateStatement.name)
            && (ignoreStates.find(newStateStatement.name) == ignoreStates.end())
            && (   (newStateStatement.context == FSM::NewStateStatement::Context::INITIALLY)
                || (newStateStatement.context == FSM::NewStateStatement::Context::FINALLY)
               )
           )
        {
          if (transitionSet.find(state->name+newStateStatement.name) == transitionSet.end())
          {
            output << "  " << state->name << " -> " << newStateStatement.name << " " << "[" << optionsToString(newStateStatement) << "]" << std::endl;
            transitionSet.insert(state->name+newStateStatement.name);
          }
        }
      });

      // state transitions
      ast.doStateTransitions(state,[&](const NewStateStatement &newStateStatement)
      {
        if (ignoreStates.find(newStateStatement.name) == ignoreStates.end())
        {
          if (state->type != State::Type::DEFAULT)
          {
            switch (newStateStatement.type)
            {
              case NewStateStatement::Type::START:
                if (transitionSet.find(state->name+ast.getStartState()->name) == transitionSet.end())
                {
                  output << "  " << state->name << " -> " << ast.getStartState()->name << " " << "[" << optionsToString(newStateStatement) << "]" << std::endl;
                  transitionSet.insert(state->name+ast.getStartState()->name);
                }
                break;
              case NewStateStatement::Type::DEFAULT:
                if (transitionSet.find(state->name+ast.getDefaultState()->name) == transitionSet.end())
                {
                  output << "  " << state->name << " -> " << ast.getDefaultState()->name << " " << "[" << optionsToString(newStateStatement) << "]" << std::endl;
                  transitionSet.insert(state->name+ast.getDefaultState()->name);
                }
                break;
              case NewStateStatement::Type::POP:
                // nothing to do: push/pop is implemented by a double direction arrow in push
                break;
              case NewStateStatement::Type::CUSTOM:
                if (transitionSet.find(state->name+newStateStatement.name) == transitionSet.end())
                {
                  
                  output << "  "
                         << state->name << " -> " << newStateStatement.name << " "
                         << "["
                         << ((newStateStatement.prefixOperator == NewStateStatement::PrefixOperator::PUSH) ? "dir=both, " : "" )
                         << optionsToString(newStateStatement)
                         << "]" << std::endl;
                  transitionSet.insert(state->name+newStateStatement.name);
                }
                break;
            }
          }
        }
      });
    }
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
