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

class PrintVisitor : public Visitor
{
  public:
    PrintVisitor(std::ostream &output)
      : output(output)
    {
    }

    /** indent
     */
    void indent()
    {
      indentions.push_back(indentions.back()+2);

      indentString.clear();
      indentString.append(indentions.back(), ' ');
    }

    /** unindent
     */
    void unindent()
    {
      indentions.pop_back();

      indentString.clear();
      indentString.append(indentions.back(), ' ');
    }

    /** indent lamda code
     * @param code code to execute
     */
    void indent(std::function<void(void)> code)
    {
      indent();
      code();
      unindent();
    }

    /** get indent string
     */
    std::string indentSpaces() const
    {
      return indentString;
    }

    void accept(Phases phase, const State &state) override
    {
      currentState = &state;
      switch (phase)
      {
        case Phases::PRE:
          indent();
          switch (state.type)
          {
            case State::Type::START:
              output << indentSpaces() << "state start" << std::endl;
              break;
            case State::Type::DEFAULT:
              output << indentSpaces() << "state default" << std::endl;
              break;
            case State::Type::CUSTOM:
              output << indentSpaces() << "state " << state.name << std::endl;
              break;
          }
          indent();
          break;
        case Phases::POST:
          unindent();
          unindent();
          break;
      }
    }

#if 0
    void accept(Phases phase, const StateList &stateList) override
    {
      switch (phase)
      {
        case Phases::PRE:
          {
            // state type/variable suffix

            // states
            output << indentSpaces() << "typedef enum" << std::endl;
            output << indentSpaces() <<"{" << std::endl;
            indent([&]()
            {
              ast.doStates([&](const State *state)
              {
                switch (state->type)
                {
                  case State::Type::START:
                    output << indentSpaces() << name(state->name) << "," << std::endl;
                    output << indentSpaces() << name("STATE_START") << "," << std::endl;
                    break;
                  case State::Type::DEFAULT:
                    output << indentSpaces() << name("STATE_DEFAULT") << "," << std::endl;
                    break;
                  case State::Type::CUSTOM:
                    output << indentSpaces() << name(state->name) << "," << std::endl;
                    break;
                }
              });
            });
            output << indentSpaces() <<"} " << name("States") << std::endl;

            output << indentSpaces() << "static " << name("States") << " " << name("state");
            const State *startState = ast.getStartState();
            if (startState != nullptr)
            {
              output << " = " << name(startState->name);
            }
            output << std::endl;
            uint stateStackSize = ast.getStateStackSize();
            if (stateStackSize > 0)
            {
              output << indentSpaces() << "static " << name("States") << " " << name("stateStack") << "[" << stateStackSize << "];" << std::endl;
              output << indentSpaces() << "static uint " << name("stateStackIndex") << " = 0;" << std::endl;
            }
            output << std::endl;

            output << indentSpaces() << "switch (" << name("state") << ")" << std::endl;
            output << indentSpaces() << "{" << std::endl;
          }
          break;
        case Phases::POST:
          output << indentSpaces() <<"}" << std::endl;
          break;
      }
    }
#endif

    void accept(const StorageClassSpecifier &storageClassSpecifier) override
    {
      switch (storageClassSpecifier.type)
      {
        case StorageClassSpecifier::Type::TYPEDEF:  output << " 'typedef' ";  break;
        case StorageClassSpecifier::Type::EXTERN:   output << " 'extern' ";   break;
        case StorageClassSpecifier::Type::STATIC:   output << " 'static' ";   break;
        case StorageClassSpecifier::Type::AUTO:     output << " 'auto' ";     break;
        case StorageClassSpecifier::Type::REGISTER: output << " 'register' "; break;
      }
    }

    void accept(const TypeQualifier &typeQualifier) override
    {
      switch (typeQualifier.type)
      {
        case TypeQualifier::Type::CONST:    output << " 'const' "; break;
        case TypeQualifier::Type::VOLATILE: output << " 'volatile' "; break;
      }
    }

    void accept(const TypeSpecifier &typeSpecifier) override
    {
      switch (typeSpecifier.type)
      {
        case TypeSpecifier::Type::VOID:       output << " 'void' "; break;
        case TypeSpecifier::Type::CHAR:       output << " 'char' "; break;
        case TypeSpecifier::Type::SHORT:      output << " 'short' "; break;
        case TypeSpecifier::Type::INT:        output << " 'int' "; break;
        case TypeSpecifier::Type::LONG:       output << " 'long' "; break;
        case TypeSpecifier::Type::FLOAT:      output << " 'float' "; break;
        case TypeSpecifier::Type::DOUBLE:     output << " 'double' "; break;
        case TypeSpecifier::Type::SIGNED:     output << " 'signed' "; break;
        case TypeSpecifier::Type::UNSIGNED:   output << " 'unsigned' "; break;
        case TypeSpecifier::Type::IDENTIFIER: output << "'" << typeSpecifier.identifier << "'"; break;
      }
    }

    void accept(const StorageClassDeclarationSpecifiers &storageClassDeclarationSpecifiers) override
    {
      bool first = true;
      for (const StorageClassDeclarationSpecifier *storageClassDeclarationSpecifier : storageClassDeclarationSpecifiers)
      {
        if (!first) output << " ";
        storageClassDeclarationSpecifier->traverse(*this);
        first = false;
      }
    }

    void accept(const DirectDeclarator &directDeclarator) override
    {
      output << "'" << directDeclarator.identifier << "'";
    }

    void accept(const Pointer &pointer) override
    {
      output << " '*' ";
    }

    void accept(const Declarator &declarator) override
    {
      if (declarator.pointer != nullptr) declarator.pointer->traverse(*this);
      declarator.directDeclarator->traverse(*this);
    }

    void accept(const Initializer &initializer) override
    {
// TODO:
//if (initializer.expression != nullptr)
      initializer.expression->traverse(*this);
    }

    void accept(const InitDeclarator &initDeclarator) override
    {
      output << "InitDeclarator: ";
      if (initDeclarator.storageClassDeclarationSpecifiers != nullptr)
      {
        initDeclarator.storageClassDeclarationSpecifiers->traverse(*this);
        output << " ";
      }
      initDeclarator.declarator->traverse(*this);
      if (initDeclarator.initializer != nullptr)
      {
        output << " '=' ";
        initDeclarator.initializer->traverse(*this);
      }
    }

    void accept(const Declaration &declaration) override
    {
      output << indentSpaces() << "Declaration: ";
      if (declaration.storageClassDeclarationSpecifiers != nullptr)
      {
        declaration.storageClassDeclarationSpecifiers->traverse(*this);
        output << " ";
      }
      declaration.initDeclaratorList->traverse(*this);
      output << std::endl;
    }

    void accept(const PrimaryExpression &primaryExpression) override
    {
      output << "PrimaryExpression: ";
      switch (primaryExpression.type)
      {
        case PrimaryExpression::Type::IDENTIFIER: output << "'" << primaryExpression.identifier << "'"; break;
        case PrimaryExpression::Type::INTEGER:    output << "'" << primaryExpression.n << "'"; break;
        case PrimaryExpression::Type::STRING:     output << " '\"' " << primaryExpression.string << " '\"' "; break;
        case PrimaryExpression::Type::EXPRESSION:
          output << " '(' ";
          primaryExpression.expression->traverse(*this);
          output << " ')' ";
          break;
      }
    }

    void accept(const PostfixExpression &postfixExpression) override
    {
      output << "PostfixExpression: ";
      switch (postfixExpression.type)
      {
        case PostfixExpression::Type::SUBSCRIPT:
          postfixExpression.array->traverse(*this);
          output << " '[' ";
          postfixExpression.index->traverse(*this);
          output << " ']' ";
          break;
        case PostfixExpression::Type::FUNCTION_CALL:
          postfixExpression.call->traverse(*this);
          output << " '(' ";
          if (postfixExpression.argumentExpressionList != nullptr)
          {
            postfixExpression.argumentExpressionList->traverse(*this);
          }
          output << " ')' ";
          break;
        case PostfixExpression::Type::MEMBER:
          postfixExpression.structure->traverse(*this);
          output << " '.' " << *postfixExpression.identifier;
          break;
      }
    }

    void accept(const UnaryExpression &unaryExpression) override
    {
      output << "UnaryExpression: ";
      switch (unaryExpression.operator_)
      {
        case UnaryExpression::Operator::ADDRESS:     output << " '&' "; break;
        case UnaryExpression::Operator::DEREFERENCE: output << " '*' "; break;
        case UnaryExpression::Operator::PLUS:        output << " '+' "; break;
        case UnaryExpression::Operator::MINUS:       output << " '-' "; break;
        case UnaryExpression::Operator::NOT:         output << " '~' "; break;
        case UnaryExpression::Operator::LOGICAL_NOT: output << " '!' "; break;
      }
      unaryExpression.expression->traverse(*this);
    }

    void accept(const CastExpression &castExpression) override
    {
      output << "CastExpression: ";
      castExpression.expression->traverse(*this);
    }

    void accept(const MultiplicativeExpression &multiplicativeExpression) override
    {
      output << "MultiplicativeExpression: ";
      multiplicativeExpression.a->traverse(*this);
      switch (multiplicativeExpression.type)
      {
        case MultiplicativeExpression::Type::MULTIPLY: output << " '*' "; break;
        case MultiplicativeExpression::Type::DIVIDE:   output << " '/' "; break;
      }
      multiplicativeExpression.b->traverse(*this);
    }

    void accept(const AdditiveExpression &additiveExpression) override
    {
      output << "AdditiveExpression: ";
      additiveExpression.a->traverse(*this);
      switch (additiveExpression.type)
      {
        case AdditiveExpression::Type::ADD:      output << " '+' "; break;
        case AdditiveExpression::Type::SUBTRACT: output << " '-' "; break;
      }
      additiveExpression.b->traverse(*this);
    }

    void accept(const ShiftExpression &shiftExpression) override
    {
      output << "ShiftExpression: ";
      shiftExpression.a->traverse(*this);
      if (shiftExpression.b != nullptr)
      {
        switch (shiftExpression.type)
        {
          case ShiftExpression::Type::LEFT:  output << " '<<' "; break;
          case ShiftExpression::Type::RIGHT: output << " '>>' "; break;
        }
        shiftExpression.b->traverse(*this);
      }
    }

    void accept(const RelationalExpression &relationalExpression) override
    {
      output << "RelationalExpression: ";
      switch (relationalExpression.type)
      {
        case RelationalExpression::Type::LOWER:
          relationalExpression.a->traverse(*this);
          output << " '<' ";
          relationalExpression.b->traverse(*this);
          break;
        case RelationalExpression::Type::LOWER_EQUALS:
          relationalExpression.a->traverse(*this);
          output << " '<=' ";
          relationalExpression.b->traverse(*this);
          break;
        case RelationalExpression::Type::GREATER:
          relationalExpression.a->traverse(*this);
          output << " '>' ";
          relationalExpression.b->traverse(*this);
          break;
        case RelationalExpression::Type::GREATER_EQUALS:
          relationalExpression.a->traverse(*this);
          output << " '>=' ";
          relationalExpression.b->traverse(*this);
          break;
      }
    }

    void accept(const EqualityExpression &equalityExpression) override
    {
      output << "EqualityExpression: ";
      switch (equalityExpression.type)
      {
        case EqualityExpression::Type::EQUALS:
          equalityExpression.a->traverse(*this);
          output << " '==' ";
          equalityExpression.b->traverse(*this);
          break;
        case EqualityExpression::Type::NOT_EQUALS:
          equalityExpression.a->traverse(*this);
          output << " '!=' ";
          equalityExpression.b->traverse(*this);
          break;
      }
    }

    void accept(const AndExpression &andExpression) override
    {
      output << "AndExpression: ";
      andExpression.a->traverse(*this);
      output << " '&' ";
      andExpression.b->traverse(*this);
    }

    void accept(const ExclusiveOrExpression &exclusiveOrExpression) override
    {
      output << "ExclusiveOrExpression: ";
      exclusiveOrExpression.a->traverse(*this);
      output << " '^' ";
      exclusiveOrExpression.b->traverse(*this);
    }

    void accept(const InclusiveOrExpression &inclusiveOrExpression) override
    {
      output << "AndEInclusiveOrExpressionxpression: ";
      inclusiveOrExpression.a->traverse(*this);
      output << " '|' ";
      inclusiveOrExpression.b->traverse(*this);
    }

    void accept(const LogicalAndExpression &logicalAndExpression) override
    {
      output << "LogicalAndExpression: ";
      logicalAndExpression.a->traverse(*this);
      output << " '&&' ";
      logicalAndExpression.b->traverse(*this);
    }

    void accept(const LogicalOrExpression &logicalOrExpression) override
    {
      output << "LogicalOrExpression: ";
      logicalOrExpression.a->traverse(*this);
      output << " '||' ";
      logicalOrExpression.b->traverse(*this);
    }

    void accept(const ConditionalExpression &conditionalExpression) override
    {
      output << "ConditionalExpression: ";
      conditionalExpression.condition->traverse(*this);
      output << " '?' ";
      conditionalExpression.a->traverse(*this);
      output << " : ";
      conditionalExpression.b->traverse(*this);
    }

    void accept(const AssignmentExpression &assignmentExpression) override
    {
      output << "AssignmentExpression: ";
      assignmentExpression.a->traverse(*this);
      switch (assignmentExpression.operator_)
      {
        case AssignmentExpression::Operator::ASSIGN:             output << " '=' ";   break;
        case AssignmentExpression::Operator::MULTIPLY_ASSIGN:    output << " '*=' ";  break;
        case AssignmentExpression::Operator::DIVIDE_ASSIGN:      output << " '/=' ";  break;
        case AssignmentExpression::Operator::MODULO_ASSIGN:      output << " '%=' ";  break;
        case AssignmentExpression::Operator::ADD_ASSIGN:         output << " '+=' ";  break;
        case AssignmentExpression::Operator::SUB_ASSIGN:         output << " '-=' ";  break;
        case AssignmentExpression::Operator::SHIFT_LEFT_ASSIGN:  output << " '<<=' "; break;
        case AssignmentExpression::Operator::SHIFT_RIGHT_ASSIGN: output << " '>>=' "; break;
        case AssignmentExpression::Operator::AND_ASSIGN:         output << " '&=' ";  break;
        case AssignmentExpression::Operator::XOR_ASSIGN:         output << " '^=' ";  break;
        case AssignmentExpression::Operator::OR_ASSIGN:          output << " '|=' ";  break;
      }
      assignmentExpression.b->traverse(*this);
    }

    void accept(Visitor::Phases phase, const CompoundStatement &compoundStatement) override
    {
      output << indentSpaces() << "CompoundStatement: " << std::endl;
      switch (phase)
      {
        case Phases::PRE:
          indent();
          break;
        case Phases::POST:
          unindent();
          break;
      }
    }

    void accept(const IfStatement &ifStatement) override
    {
      output << indentSpaces() << "IfStatement: "; ifStatement.condition->traverse(*this); output << std::endl;
      indent();
      ifStatement.ifStatement->traverse(*this);
      unindent();
      if (ifStatement.elseStatement != nullptr)
      {
        output << indentSpaces() << "else" << std::endl;
        indent();
        ifStatement.elseStatement->traverse(*this);
        unindent();
      }
    }

    void accept(const ForStatement &forStatement) override
    {
      output << indentSpaces() << "ForStatement: " << std::endl;
      indent();
      forStatement.init->traverse(*this);
      forStatement.condition->traverse(*this);
      if (forStatement.increment != nullptr) forStatement.increment->traverse(*this);
      forStatement.statement->traverse(*this);
      unindent();
    }

    void accept(const WhileStatement &whileStatement) override
    {
      output << indentSpaces() << "WhileStatement: ";
      whileStatement.condition->traverse(*this);
      output << " ')' " << std::endl;
      whileStatement.statement->traverse(*this);
    }

    void accept(const DoStatement &doStatement) override
    {
      output << indentSpaces() << "DoStatement: ";
      doStatement.statement->traverse(*this);
      output << indentSpaces() << " ';' ";
      doStatement.condition->traverse(*this);
    }

    void accept(const ExpressionStatement &expressionStatement) override
    {
      output << indentSpaces() << "ExpressionStatement: "; expressionStatement.expression->traverse(*this); output << " ';' " << std::endl;
    }

    void accept(const JumpStatement &jumpStatement) override
    {
      switch (jumpStatement.type)
      {
        case JumpStatement::Type::CONTINUE:
          output << indentSpaces() << " 'continue' " <<  std::endl;
          break;
        case JumpStatement::Type::BREAK:
          output << indentSpaces() << " 'break' " <<  std::endl;
          break;
        case JumpStatement::Type::RETURN:
          output << indentSpaces() << " 'return' ";
          if (jumpStatement.expression != nullptr)
          {
            output << " ";
            jumpStatement.expression->traverse(*this);
          }
          output << std::endl;
        break;
      }
    }

    void accept(const NewStateStatement &newStateStatement) override
    {
      switch (newStateStatement.prefixOperator)
      {
        case NewStateStatement::PrefixOperator::PUSH:
          break;
        case NewStateStatement::PrefixOperator::RESET:
          break;
        default:
          break;
      }
      switch (newStateStatement.type)
      {
        case NewStateStatement::Type::START:
          output << indentSpaces() << "new state " << "start" << std::endl;
          break;
        case NewStateStatement::Type::DEFAULT:
          output << indentSpaces() << "new state " << "default" << std::endl;
          break;
        case NewStateStatement::Type::POP:
          break;
        case NewStateStatement::Type::CUSTOM:
          output << indentSpaces() << "new state " << newStateStatement.name << std::endl;
          break;
      }
    }

  private:
    std::ostream      &output;
    std::string       suffix;
    const State       *currentState;

    std::vector<uint> indentions{0};
    std::string       indentString;
};

void AST::print() const
{
  PrintVisitor printVisitor(std::cout);

  std::cout << "AST:" << std::endl;
  printVisitor.run(*this);
}

void AST::addState(FSM::State *state)
{
  assert(state != nullptr);

  states[state->name] = state;
}

/* end of file */
