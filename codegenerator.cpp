/***********************************************************************\
*
* Contents: C code generator
* Systems: all
*
\***********************************************************************/

/****************************** Includes *******************************/

#include <iostream>
#include <sstream>
#include <random>
#include <limits>
#include <regex>

#include "ast.h"
#include "visitor.h"

#include "codegenerator.h"

using namespace FSM;

/****************** Conditional compilation switches *******************/

/***************************** Constants *******************************/

/***************************** Datatypes *******************************/

/***************************** Variables *******************************/

/***************************** Forwards ********************************/

/***************************** Functions *******************************/

class CVisitor : public Visitor
{
  #define name(string) "__" << string << suffix << "__"

  public:
    CVisitor(std::ostream &output, const AST &ast, const std::string &logFunction)
      : Visitor()
      , ast(ast)
      , logFunction(logFunction)
      , output(output)
    {
    }
//    CVisitor(const CVisitor &) = delete;
//    CVisitor(const CVisitor &&) = delete;

    /** indent
     */
    void indent(uint n)
    {
      indentions.push_back(indentions.back()+n);

      indentString.clear();
      indentString.append(indentions.back(), ' ');
    }

    void unindent()
    {
      indentions.pop_back();

      indentString.clear();
      indentString.append(indentions.back(), ' ');
    }

    void indent(uint n,std::function<void(void)> code)
    {
      indent(2);
      code();
      unindent();
    }

    std::string indent() const
    {
      return indentString;
    }

    bool accept(Phases phase, const State &state) override
    {
      currentState = &state;
      switch (phase)
      {
        case Phases::PRE:
          indent(2);
          switch (state.type)
          {
            case State::Type::START:
              output << indent() << "case " << name(state.name) << ":" << std::endl;
              output << indent() << "case " << name("STATE_START") << ":" << std::endl;
              break;
            case State::Type::DEFAULT:
              output << indent() << "case " << name("STATE_DEFAULT") << ":" << std::endl;
              break;
            case State::Type::CUSTOM:
              output << indent() << "case " << name(state.name) << ":" << std::endl;
              break;
          }
          indent(2);
          break;
        case Phases::POST:
          output << indent() << "break;" << std::endl;
          unindent();
          unindent();
          break;
      }

      return true;
    }

    bool accept(Phases phase, const StateList &stateList) override
    {
      switch (phase)
      {
        case Phases::PRE:
          {
            // state type/variable suffix
            std::random_device randomDevice;
            std::mt19937       randomGenerator(randomDevice());
            std::uniform_int_distribution<std::mt19937::result_type> random(0,std::numeric_limits<int>::max());
            std::stringstream buffer;
            buffer << "_" << random(randomGenerator);
            suffix = buffer.str();

            // states
            output << indent() << "typedef enum" << std::endl;
            output << indent() <<"{" << std::endl;
            indent(2,[&]()
            {
              ast.doStates([&](const State *state)
              {
                switch (state->type)
                {
                  case State::Type::START:
                    output << indent() << name(state->name) << "," << std::endl;
                    output << indent() << name("STATE_START") << "," << std::endl;
                    break;
                  case State::Type::DEFAULT:
                    output << indent() << name("STATE_DEFAULT") << "," << std::endl;
                    break;
                  case State::Type::CUSTOM:
                    output << indent() << name(state->name) << "," << std::endl;
                    break;
                }
              });
            });
            output << indent() <<"} " << name("States") << ";" << std::endl;

            output << indent() << "static " << name("States") << " " << name("state");
            const State *startState = ast.getStartState();
            if (startState != nullptr)
            {
              output << " = " << name(startState->name);
            }
            output << ";" << std::endl;
            uint stateStackSize = ast.getStateStackSize();
            if (stateStackSize > 0)
            {
              output << indent() << "static " << name("States") << " " << name("stateStack") << "[" << stateStackSize << "];" << std::endl;
              output << indent() << "static uint " << name("stateStackIndex") << " = 0;" << std::endl;
            }
            output << std::endl;

            output << indent() << "switch (" << name("state") << ")" << std::endl;
            output << indent() << "{" << std::endl;
          }
          break;
        case Phases::POST:
          output << indent() <<"}" << std::endl;
          break;
      }

      return true;
    }

    bool accept(const StorageClassSpecifier &storageClassSpecifier) override
    {
      switch (storageClassSpecifier.type)
      {
        case StorageClassSpecifier::Type::TYPEDEF:  output << "typedef";  break;
        case StorageClassSpecifier::Type::EXTERN:   output << "extern";   break;
        case StorageClassSpecifier::Type::STATIC:   output << "static";   break;
        case StorageClassSpecifier::Type::AUTO:     output << "auto";     break;
        case StorageClassSpecifier::Type::REGISTER: output << "register"; break;
      }

      return true;
    }

    bool accept(const TypeQualifier &typeQualifier) override
    {
      switch (typeQualifier.type)
      {
        case TypeQualifier::Type::CONST:    output << "const"; break;
        case TypeQualifier::Type::VOLATILE: output << "volatile"; break;
      }
    }

    bool accept(const TypeSpecifier &typeSpecifier) override
    {
      switch (typeSpecifier.type)
      {
        case TypeSpecifier::Type::INT:        output << "int"; break;
        case TypeSpecifier::Type::IDENTIFIER: output << typeSpecifier.identifier; break;
      }
    }

    bool accept(const StorageClassDeclarationSpecifiers &storageClassDeclarationSpecifiers) override
    {
      bool first = true;
      for (const StorageClassDeclarationSpecifier *storageClassDeclarationSpecifier : storageClassDeclarationSpecifiers)
      {
        if (!first) output << " ";
        storageClassDeclarationSpecifier->traverse(*this);
        first = false;
      }

      return true;
    }

    bool accept(const DirectDeclarator &directDeclarator) override
    {
      output << directDeclarator.identifier;

      return true;
    }

    bool accept(const Declarator &declarator) override
    {
      declarator.directDeclarator->traverse(*this);

      return true;
    }

    bool accept(const Initializer &initializer) override
    {
      initializer.expression->traverse(*this);

      return true;
    }

    bool accept(const InitDeclarator &initDeclarator) override
    {
      if (initDeclarator.storageClassDeclarationSpecifiers != nullptr)
      {
        initDeclarator.storageClassDeclarationSpecifiers->traverse(*this);
        output << " ";
      }
      initDeclarator.declarator->traverse(*this);
      if (initDeclarator.initializer != nullptr)
      {
        output << " = ";
        initDeclarator.initializer->traverse(*this);
      }

      return true;
    }

    bool accept(const Declaration &declaration) override
    {
      output << indent();
      if (declaration.storageClassDeclarationSpecifiers != nullptr)
      {
        declaration.storageClassDeclarationSpecifiers->traverse(*this);
        output << " ";
      }
      declaration.initDeclaratorList->traverse(*this);
      output << ";" << std::endl;

      return true;
    }

    bool accept(const PrimaryExpression &primaryExpression) override
    {
      switch (primaryExpression.type)
      {
        case PrimaryExpression::Type::IDENTIFIER: output << primaryExpression.identifier; break;
        case PrimaryExpression::Type::INTEGER:    output << primaryExpression.n; break;
        case PrimaryExpression::Type::STRING:     output << "\"" << primaryExpression.string << "\""; break;
        case PrimaryExpression::Type::EXPRESSION:
          output << '(';
          primaryExpression.expression->traverse(*this);
          output << ')';
          break;
      }

      return true;
    }

    bool accept(const PostfixExpression &postfixExpression) override
    {
      switch (postfixExpression.type)
      {
        case PostfixExpression::Type::SUBSCRIPT:
          postfixExpression.array->traverse(*this);
          output << "[";
          postfixExpression.index->traverse(*this);
          output << "]";
          break;
        case PostfixExpression::Type::FUNCTION_CALL:
          postfixExpression.call->traverse(*this);
          output << "(";
          if (postfixExpression.argumentExpressionList != nullptr)
          {
            postfixExpression.argumentExpressionList->traverse(*this);
          }
          output << ")";
          break;
        case PostfixExpression::Type::MEMBER:
          postfixExpression.structure->traverse(*this);
          output << "." << postfixExpression.identifier;
          break;
      }

      return true;
    }

    bool accept(const UnaryExpression &unaryExpression) override
    {
      switch (unaryExpression.operator_)
      {
        case UnaryExpression::Operator::ADDRESS:     output << "&"; break;
        case UnaryExpression::Operator::DEREFERENCE: output << "*"; break;
        case UnaryExpression::Operator::PLUS:        output << "+"; break;
        case UnaryExpression::Operator::MINUS:       output << "-"; break;
        case UnaryExpression::Operator::NOT:         output << "~"; break;
        case UnaryExpression::Operator::LOGICAL_NOT: output << "!"; break;
      }
      unaryExpression.expression->traverse(*this);

      return true;
    }

    bool accept(const CastExpression &castExpression) override
    {
//output << "castExpression";
//fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
      castExpression.expression->traverse(*this);

      return true;
    }

    bool accept(const MultiplicativeExpression &multiplicativeExpression) override
    {
//output << "multiplicativeExpression";
      multiplicativeExpression.a->traverse(*this);
      switch (multiplicativeExpression.type)
      {
        case MultiplicativeExpression::Type::MULTIPLY: output << " * "; break;
        case MultiplicativeExpression::Type::DIVIDE:   output << " / "; break;
      }
      multiplicativeExpression.b->traverse(*this);

      return true;
    }

    bool accept(const AdditiveExpression &additiveExpression) override
    {
//output << "additiveExpression";
      additiveExpression.a->traverse(*this);
      switch (additiveExpression.type)
      {
        case AdditiveExpression::Type::ADD:      output << " + "; break;
        case AdditiveExpression::Type::SUBTRACT: output << " - "; break;
      }
      additiveExpression.b->traverse(*this);

      return true;
    }

    bool accept(const ShiftExpression &shiftExpression) override
    {
//output << "shiftExpression";
      shiftExpression.a->traverse(*this);
      if (shiftExpression.b != nullptr)
      {
        switch (shiftExpression.type)
        {
          case ShiftExpression::Type::LEFT:  output << " << "; break;
          case ShiftExpression::Type::RIGHT: output << " >> "; break;
        }
        shiftExpression.b->traverse(*this);
      }

      return true;
    }

    bool accept(const RelationalExpression &relationalExpression) override
    {
//output << "relationalExpression";
      switch (relationalExpression.type)
      {
        case RelationalExpression::Type::LOWER:
          relationalExpression.a->traverse(*this);
          output << " < ";
          relationalExpression.b->traverse(*this);
          break;
        case RelationalExpression::Type::LOWER_EQUALS:
          relationalExpression.a->traverse(*this);
          output << " <= ";
          relationalExpression.b->traverse(*this);
          break;
        case RelationalExpression::Type::GREATER:
          relationalExpression.a->traverse(*this);
          output << " > ";
          relationalExpression.b->traverse(*this);
          break;
        case RelationalExpression::Type::GREATER_EQUALS:
          relationalExpression.a->traverse(*this);
          output << " >= ";
          relationalExpression.b->traverse(*this);
          break;
      }

      return true;
    }

    bool accept(const EqualityExpression &equalityExpression) override
    {
//output << "equalityExpression";
      switch (equalityExpression.type)
      {
        case EqualityExpression::Type::EQUALS:
          equalityExpression.a->traverse(*this);
          output << " == ";
          equalityExpression.b->traverse(*this);
          break;
        case EqualityExpression::Type::NOT_EQUALS:
          equalityExpression.a->traverse(*this);
          output << " != ";
          equalityExpression.b->traverse(*this);
          break;
      }

      return true;
    }

    bool accept(const AndExpression &andExpression) override
    {
//output << "andExpression";
      andExpression.a->traverse(*this);
      output << " & ";
      andExpression.b->traverse(*this);

      return true;
    }

    bool accept(const ExclusiveOrExpression &exclusiveOrExpression) override
    {
//output << "exclusiveOrExpression";
      exclusiveOrExpression.a->traverse(*this);
      output << " ^ ";
      exclusiveOrExpression.b->traverse(*this);

      return true;
    }

    bool accept(const InclusiveOrExpression &inclusiveOrExpression) override
    {
//output << "inclusiveOrExpression";
      inclusiveOrExpression.a->traverse(*this);
      output << " | ";
      inclusiveOrExpression.b->traverse(*this);

      return true;
    }

    bool accept(const LogicalAndExpression &logicalAndExpression) override
    {
//output << "logicalAndExpression";
      logicalAndExpression.a->traverse(*this);
      output << " && ";
      logicalAndExpression.b->traverse(*this);

      return true;
    }

    bool accept(const LogicalOrExpression &logicalOrExpression) override
    {
//output << "logicalOrExpression";
      logicalOrExpression.a->traverse(*this);
      output << " || ";
      logicalOrExpression.b->traverse(*this);

      return true;
    }

    bool accept(const ConditionalExpression &conditionalExpression) override
    {
//output << "conditionalExpression";
      conditionalExpression.condition->traverse(*this);
      output << " ? ";
      conditionalExpression.a->traverse(*this);
      output << " : ";
      conditionalExpression.b->traverse(*this);

      return true;
    }

    bool accept(const AssignmentExpression &assignmentExpression) override
    {
      assignmentExpression.a->traverse(*this);
      switch (assignmentExpression.operator_)
      {
        case AssignmentExpression::Operator::ASSIGN:             output << " = ";   break;
        case AssignmentExpression::Operator::MULTIPLY_ASSIGN:    output << " *= ";  break;
        case AssignmentExpression::Operator::DIVIDE_ASSIGN:      output << " /= ";  break;
        case AssignmentExpression::Operator::MODULO_ASSIGN:      output << " %= ";  break;
        case AssignmentExpression::Operator::ADD_ASSIGN:         output << " += ";  break;
        case AssignmentExpression::Operator::SUB_ASSIGN:         output << " -= ";  break;
        case AssignmentExpression::Operator::SHIFT_LEFT_ASSIGN:  output << " <<= "; break;
        case AssignmentExpression::Operator::SHIFT_RIGHT_ASSIGN: output << " >>= "; break;
        case AssignmentExpression::Operator::AND_ASSIGN:         output << " &= ";  break;
        case AssignmentExpression::Operator::XOR_ASSIGN:         output << " ^= ";  break;
        case AssignmentExpression::Operator::OR_ASSIGN:          output << " |= ";  break;
      }
      assignmentExpression.b->traverse(*this);

      return true;
    }

    bool accept(const CompoundStatement &compoundStatement) override
    {
      output << indent() << "{" << std::endl;
      indent(2,[&]()
      {
        if (compoundStatement.declarationStatementList != nullptr)
        {
          compoundStatement.declarationStatementList->traverse(*this);
        }
      });
      output << indent() << "}" << std::endl;

      return true;
    }

    bool accept(const IfStatement &ifStatement) override
    {
      output << indent() << "if ("; ifStatement.condition->traverse(*this); output << ")" << std::endl;
      ifStatement.ifStatement->traverse(*this);
      if (ifStatement.elseStatement != nullptr)
      {
        output << indent() << "else" << std::endl;
        ifStatement.elseStatement->traverse(*this);
      }

      return true;
    }

    bool accept(const ForStatement &forStatement) override
    {
      output << indent() << "for (";
      forStatement.init->traverse(*this);
      output << "; ";
      forStatement.condition->traverse(*this);
      output << "; ";
      forStatement.increment->traverse(*this);
      output << ")" << std::endl;
      forStatement.statement->traverse(*this);
    }

    bool accept(const WhileStatement &whileStatement) override
    {
      output << indent() << "while (";
      whileStatement.condition->traverse(*this);
      output << ")" << std::endl;
      whileStatement.statement->traverse(*this);
    }

    bool accept(const DoStatement &doStatement) override
    {
      output << indent() << "do" << std::endl;
      doStatement.statement->traverse(*this);
      output << indent() << "while (";
      doStatement.condition->traverse(*this);
      output << ");" << std::endl;

      return true;
    }

    bool accept(Phases phase, const ExpressionStatement &expressionStatement) override
    {
      switch (phase)
      {
        case Phases::PRE:
          output << indent();
          break;
        case Phases::POST:
          output << ";" << std::endl;
          break;
      }

      return true;
    }

    bool accept(const JumpStatement &jumpStatement) override
    {
      switch (jumpStatement.type)
      {
        case JumpStatement::Type::CONTINUE:
          output << indent() << "continue" << ';' <<  std::endl;
          break;
        case JumpStatement::Type::BREAK:
          output << indent() << "break" << ';' <<  std::endl;
          break;
        case JumpStatement::Type::RETURN:
          output << indent() << "return";
          if (jumpStatement.expression != nullptr)
          {
            output << " ";
            jumpStatement.expression->traverse(*this);
          }
          output << ";" << std::endl;
        break;
      }

      return true;
    }

    bool accept(const NewStateStatement &newStateStatement) override
    {
      output << indent() << "{" << std::endl;
      indent(2);
      switch (newStateStatement.prefixOperator)
      {
        case NewStateStatement::PrefixOperator::PUSH:
          if (ast.hasStateStack())
          {
            if (ast.isAsserts())
            {
              output << indent() << "assert(" << name("stateStackIndex") << " < " << ast.getStateStackSize() << ");" << std::endl;
            }
            output << indent() << name("stateStack") << "[" << name("stateStackIndex") << "] = " << name("state") << ";" << std::endl;
            output << indent() << name("stateStackIndex") <<"++;" << std::endl;
          }
          else
          {
            throw std::runtime_error("no state stack defined");
          }
          break;
        case NewStateStatement::PrefixOperator::RESET:
          if (ast.hasStateStack())
          {
            output << indent() << name("stateStackIndex") <<" = 0;" << std::endl;
          }
          else
          {
            throw std::runtime_error("no state stack defined");
          }
          break;
        default:
          break;
      }
      switch (newStateStatement.type)
      {
        case NewStateStatement::Type::START:
          output << indent() << name("state") << " = " << name(ast.getStartState()) << ";" << std::endl;
          break;
        case NewStateStatement::Type::DEFAULT:
          output << indent() << name("state") << " = " << name("STATE_DEFAULT") << ";" << std::endl;
          break;
        case NewStateStatement::Type::POP:
          if (ast.hasStateStack())
          {
            if (ast.isAsserts())
            {
              output << indent() << "assert(" << name("stateStackIndex") << " > 0);" << std::endl;
              output << indent() << name("stateStackIndex") << "--;" << std::endl;
              output << indent() << name("state") << " = " << name("stateStack") << "[" << name("stateStackIndex") << "];" << std::endl;
            }
            else
            {
              output << indent() << "if (" << name("stateStackIndex") << " > 0)" << std::endl;
              output << indent() << "{" << std::endl;
              indent(2,[&]()
              {
                output << indent() << name("stateStackIndex") << "--;" << std::endl;
                output << indent() << name("state") << " = " << name("stateStack") << "[" << name("stateStackIndex") << "];" << std::endl;
              });
              output << indent() << "}" << std::endl;
            }
          }
          else
          {
            throw std::runtime_error("no state stack defined");
          }
          break;
        case NewStateStatement::Type::CUSTOM:
          output << indent() << name("state") << " = " << name(newStateStatement.name) << ";" << std::endl;
          if (!logFunction.empty())
          {
            output << indent() << expandMacros(logFunction,newStateStatement.name) << ";" << std::endl;
          }
          break;
      }
      unindent();
      output << indent() << "}" << std::endl;

      return true;
    }

  private:
    std::ostream      &output;
    const AST         &ast;
    const std::string &logFunction;
    std::string       suffix;
    const State       *currentState;

    std::vector<uint> indentions{0};
    std::string       indentString;

    std::string expandMacros(const std::string &logFunction, const std::string &toStateName)
    {
      std::regex fromStateNameRegEx("@fromStateName@");
      std::regex toStateNameRegEx("@toStateName@");

      std::string string = logFunction;
      string = std::regex_replace(string,fromStateNameRegEx,"\""+currentState->name+"\"");
      string = std::regex_replace(string,toStateNameRegEx,"\""+toStateName+"\"");
      return string;
    }
};

void CodeGenerator::generate(AST &ast)
{
  CVisitor visitor(output, ast, logFunction);

  visitor.indent(indent);
  output << visitor.indent() << "" << std::endl;
  output << visitor.indent() << "// FSM start " << ast.getFSMName() << std::endl;
  output << visitor.indent() << "{" << std::endl;
  visitor.indent(2);
  visitor.run(ast);
  visitor.unindent();
  output << visitor.indent() << "}" << std::endl;
  output << visitor.indent() << "// FSM end" << std::endl;
  visitor.unindent();
}

/* end of file */
