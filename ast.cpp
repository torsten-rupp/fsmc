/***********************************************************************\
*
* Contents:
* Systems: all
*
\***********************************************************************/

/****************************** Includes *******************************/

#include <iostream>
#include <sstream>
#include <algorithm>

#include "ast.h"

using namespace FSM;

/****************** Conditional compilation switches *******************/

/***************************** Constants *******************************/

/***************************** Datatypes *******************************/

/***************************** Variables *******************************/

/***************************** Forwards ********************************/


/***************************** Functions *******************************/

AST::AST(uint stateStackSize, bool asserts)
  : stateStackSize(stateStackSize)
  , asserts(asserts)
  , states()
{
}

void AST::clear()
{
  states.clear();
}

void AST::traverse(Visitor &visitor) const
{
  getStateList().traverse(visitor);
}

void AST::validateStates() const
{
  doStateTransitions([&](const NewStateStatement &newStateStatement)
  {
    if (newStateStatement.type == NewStateStatement::Type::CUSTOM)
    {
      if (getState(newStateStatement.name) == nullptr)
      {
        std::stringstream buffer;
        buffer << "state '" << newStateStatement.name << "' does not exists";
        throw std::runtime_error(buffer.str());
      }
    }
  });
}

const State* AST::getState(const std::string &name) const
{
  StateMap::const_iterator iterator = states.find(name);

  if (iterator != states.end())
  {
    return iterator->second;
  }

  std::stringstream buffer;
  buffer << "state '" << name << "' not found";
  throw std::runtime_error(buffer.str());
}

const State* AST::getStartState() const
{
  StateMap::const_iterator iterator = std::find_if(states.begin(),states.end(),[](std::pair<const std::string,State*> pair)
                                                   {
                                                     return pair.second->type == State::Type::START;
                                                   }
                                                  );

  return (iterator != states.end()) ? iterator->second : nullptr;
}

const State* AST::getDefaultState() const
{
  StateMap::const_iterator iterator = std::find_if(states.begin(),states.end(),[](std::pair<const std::string,State*> pair)
                                                   {
                                                     return pair.second->type == State::Type::DEFAULT;
                                                   }
                                                  );

  return (iterator != states.end()) ? iterator->second : nullptr;
}

void AST::doStates(const State                                                                  *toState,
                   std::function<void(const State*,const NewStateStatement &newStateStatement)> handler
                  ) const
{
  assert(toState != nullptr);

#if 0
for (const std::pair<const std::string,const NewStateStatement*> &p : stateTransitions)
{
  fprintf(stderr,"%s:%d: %s -> %s\n",__FILE__,__LINE__,p.first.c_str(),p.second->name.c_str());
}
fprintf(stderr,"%s:%d: ----\n",__FILE__,__LINE__);
#endif

  for (const std::pair<const std::string,const NewStateStatement*> &pair : stateTransitions)
  {
    switch (pair.second->type)
    {
      case NewStateStatement::Type::START:
        break;
      case NewStateStatement::Type::DEFAULT:
        break;
      case NewStateStatement::Type::POP:
        break;
      case NewStateStatement::Type::CUSTOM:
        if (getState(pair.second->name) == toState)
        {
          handler(getState(pair.first),*pair.second);
        }
        break;
    }
  }
}

void AST::doStates(std::function<void(const State *state)> handler) const
{
  for (const std::pair<const std::string,State*> &pair : states)
  {
    handler(pair.second);
  }
}

void AST::doStateTransitions(const State                                                     *fromState,
                             std::function<void(const NewStateStatement &newStateStatement)> handler
                            ) const
{
  assert(fromState != nullptr);

  std::pair<StateTransitionMap::const_iterator, StateTransitionMap::const_iterator> iterators = stateTransitions.equal_range(fromState->name);
  while (iterators.first != iterators.second)
  {
    handler(*iterators.first->second);
    iterators.first++;
//fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,iterators.second->second->name.c_str());
  }
}

void AST::doStateTransitions(std::function<void(const NewStateStatement &newStateStatement)> handler) const
{
  for (const std::pair<Identifier,const NewStateStatement*> &pair : stateTransitions)
  {
    handler(*pair.second);
  }
}

void AST::print() const
{
  std::cout << "AST:" << std::endl;

  std::cout << "stateList:" << std::endl;
  for (const std::pair<const std::string,State*> &pair : states)
  {
      std::cout << "  " << pair.second->name << ":" << std::endl;
      for (const DeclarationStatement *declarationStatement : *pair.second->compoundStatement->declarationStatementList)
      {
        std::cout << "  " << declarationStatement << std::endl;
      }
      std::cout << std::endl;
  }
}

void AST::addState(FSM::State *state)
{
  assert(state != nullptr);

  states[state->name] = state;
}

/* end of file */
