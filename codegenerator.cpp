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

/****************************** Macros *********************************/

/***************************** Functions *******************************/

class CVisitor : public Visitor
{
  #define name(string) "__" << string << suffix << "__"

  public:
    CVisitor(std::ostream &output, const AST &ast, const std::string &filePath, size_t indentCount, const std::string &logFunction)
      : Visitor()
      , output(output)
      , ast(ast)
      , filePath(filePath)
      , indentCount(indentCount)
      , logFunction(logFunction)
      , currentState(ast.getStartState())
    {
    }

    /** indent
     * @param n spaces to indent
     */
    void indent(size_t n)
    {
      indentions.push_back(indentions.back()+n);

      indentString.clear();
      indentString.append(indentions.back(), ' ');
    }

    /** indent
     */
    void indent()
    {
      indent(indentCount);
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

    void accept(const Initially &initially) override
    {
    }

    void accept(const Finally &finally) override
    {
    }

    void accept(Phases phase, const State &state) override
    {
      currentState = &state;
      switch (phase)
      {
        case Phases::PRE:
          output << indentSpaces() << "#line " << state.first.line << " \"" << filePath << "\"" << std::endl;

          indent();
          switch (state.type)
          {
            case State::Type::START:
              output << indentSpaces() << "case " << name(state.name) << ":" << std::endl;
              output << indentSpaces() << "case " << name("STATE_START") << ":" << std::endl;
              break;
            case State::Type::DEFAULT:
              output << indentSpaces() << "case " << name("STATE_DEFAULT") << ":" << std::endl;
              break;
            case State::Type::CUSTOM:
              output << indentSpaces() << "case " << name(state.name) << ":" << std::endl;
              break;
          }
          indent();
          output << indentSpaces();
          break;
        case Phases::POST:
          output << std::endl << indentSpaces() << "break;" << std::endl;
          unindent();
          unindent();
          break;
      }
    }

    void accept(Phases phase, const StateList &stateList) override
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
            output << indentSpaces() << "typedef enum" << std::endl;
            output << indentSpaces() <<"{" << std::endl;
            indent([this]()
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
            output << indentSpaces() <<"} " << name("States") << ";" << std::endl;

            output << indentSpaces() << "static " << name("States") << " " << name("state");
            const State *startState = ast.getStartState();
            if (startState != nullptr)
            {
              output << " = " << name(startState->name);
            }
            output << ";" << std::endl;
            if (ast.isStackRequired())
            {
              size_t stateStackSize = ast.getStateStackSize();
              if (stateStackSize == 0)
              {
                throw std::runtime_error("no state stack defined");
              }
              output << indentSpaces() << "static " << name("States") << " " << name("stateStack") << "[" << stateStackSize << "];" << std::endl;
              output << indentSpaces() << "static size_t " << name("stateStackIndex") << " = 0;" << std::endl;
            }
            output << std::endl;

            const Initially *initially = ast.getInitiallyStatement();
            if (initially != nullptr)
            {
              output << indentSpaces(); initially->compoundStatement->traverse(*this); output << std::endl;
            }

            output << indentSpaces() << "switch (" << name("state") << ")" << std::endl;
            output << indentSpaces() << "{" << std::endl;
          }
          break;
        case Phases::POST:
          {
            output << indentSpaces() <<"}" << std::endl;
            output << indentSpaces() << name("end") << ":" << std::endl;

            const Finally *finally = ast.getFinallyStatement();
            if (finally != nullptr)
            {
              output << indentSpaces(); finally->compoundStatement->traverse(*this); output << std::endl;
            }
          }
          break;
      }
    }

    void accept(const StorageClassSpecifier &storageClassSpecifier) override
    {
      switch (storageClassSpecifier.type)
      {
        case StorageClassSpecifier::Type::TYPEDEF:  output << "typedef";  break;
        case StorageClassSpecifier::Type::EXTERN:   output << "extern";   break;
        case StorageClassSpecifier::Type::STATIC:   output << "static";   break;
        case StorageClassSpecifier::Type::AUTO:     output << "auto";     break;
        case StorageClassSpecifier::Type::REGISTER: output << "register"; break;
      }
    }

    void accept(const TypeQualifier &typeQualifier) override
    {
      switch (typeQualifier.type)
      {
        case TypeQualifier::Type::CONST:    output << "const"; break;
        case TypeQualifier::Type::VOLATILE: output << "volatile"; break;
        case TypeQualifier::Type::STRUCT:   output << "struct"; break;
default:
throw std::runtime_error("NYI void accept(const TypeQualifier &typeQualifier)");
break;
      }
    }

    void accept(const TypeSpecifier &typeSpecifier) override
    {
      switch (typeSpecifier.type)
      {
        case TypeSpecifier::Type::VOID:       output << "void"; break;
        case TypeSpecifier::Type::CHAR:       output << "int"; break;
        case TypeSpecifier::Type::SHORT:      output << "int"; break;
        case TypeSpecifier::Type::INT:        output << "int"; break;
        case TypeSpecifier::Type::LONG:       output << "int"; break;
        case TypeSpecifier::Type::FLOAT:      output << "int"; break;
        case TypeSpecifier::Type::DOUBLE:     output << "int"; break;
        case TypeSpecifier::Type::SIGNED:     output << "int"; break;
        case TypeSpecifier::Type::UNSIGNED:   output << "int"; break;

        case TypeSpecifier::Type::STRUCT:     output << "struct" << " " << typeSpecifier.identifier; break;
        case TypeSpecifier::Type::UNION:      output << "union" << " " << typeSpecifier.identifier; break;

        case TypeSpecifier::Type::IDENTIFIER: output << typeSpecifier.identifier; break;
default:
fprintf(stderr,"%s:%d: %d\n",__FILE__,__LINE__,(int)typeSpecifier.type);
throw std::runtime_error("NYI void accept(const TypeSpecifier &typeSpecifier");
break;
      }
    }

    void accept(const SpecifierQualifierList &specifierQualifierList) override
    {
      bool first = true;
      for (const DeclarationSpecifier *declarationSpecifier : specifierQualifierList)
      {
        if (!first) output << " ";
        declarationSpecifier->traverse(*this);
        first = false;
      }
    }

    void accept(const TypeName &typeName) override
    {
      bool first = true;
      for (const DeclarationSpecifier *declarationSpecifier : *typeName.specifierQualifierList)
      {
        if (!first) output << " ";
        declarationSpecifier->traverse(*this);
        first = false;
      }
      if (typeName.abstractDeclarator != nullptr)
      {
        typeName.abstractDeclarator->traverse(*this);
      }
    }

    void accept(const AbstractDeclarator &abstractDeclarator) override
    {
      if (abstractDeclarator.pointer != nullptr)
      {
        output << "*";
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
      output << directDeclarator.identifier;
    }

    void accept(const Pointer &pointer) override
    {
      output << "*";
    }

    void accept(const Declarator &declarator) override
    {
      if (declarator.pointer != nullptr) declarator.pointer->traverse(*this);
      declarator.directDeclarator->traverse(*this);
    }

    void accept(const PrimaryExpression &primaryExpression) override
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
    }

    void accept(const ArgumentExpressionList &argumentExpressionList) override
    {
      bool first = true;
      for (const Expression *expression : argumentExpressionList)
      {
        if (!first) output << ", ";
        expression->traverse(*this);
        first = false;
      }
    }

    void accept(const PostfixExpression &postfixExpression) override
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
          output << "." << *postfixExpression.identifier;
          break;
        case PostfixExpression::Type::POINTER:
          output << "->";
          postfixExpression.expression->traverse(*this);
          break;
        case PostfixExpression::Type::INCREMENT:
          postfixExpression.expression->traverse(*this);
          output << "++";
          break;
        case PostfixExpression::Type::DECREMENT:
          postfixExpression.expression->traverse(*this);
          output << "--";
          break;
      }
    }

    void accept(const UnaryExpression &unaryExpression) override
    {
      switch (unaryExpression.operator_)
      {
        case UnaryExpression::Operator::ADDRESS:     output << "&"; break;
        case UnaryExpression::Operator::DEREFERENCE: output << "*"; break;
        case UnaryExpression::Operator::PLUS:        output << "+"; break;
        case UnaryExpression::Operator::MINUS:       output << "-"; break;
        case UnaryExpression::Operator::NOT:         output << "~"; break;
        case UnaryExpression::Operator::LOGICAL_NOT: output << "!"; break;

        case UnaryExpression::Operator::INCREMENT:   output << "++"; break;
        case UnaryExpression::Operator::DECREMENT:   output << "--"; break;
        case UnaryExpression::Operator::SIZEOF:      output << "sizeof"; break;
      }
      unaryExpression.expression->traverse(*this);
    }

    void accept(const CastExpression &castExpression) override
    {
      output << "(";
      castExpression.typeName->traverse(*this);
      output << ")";
      castExpression.expression->traverse(*this);
    }

    void accept(const MultiplicativeExpression &multiplicativeExpression) override
    {
//output << "multiplicativeExpression";
      multiplicativeExpression.a->traverse(*this);
      switch (multiplicativeExpression.type)
      {
        case MultiplicativeExpression::Type::MULTIPLY: output << " * "; break;
        case MultiplicativeExpression::Type::DIVIDE:   output << " / "; break;
        case MultiplicativeExpression::Type::MODULO:   output << " % "; break;
      }
      multiplicativeExpression.b->traverse(*this);
    }

    void accept(const AdditiveExpression &additiveExpression) override
    {
//output << "additiveExpression";
      additiveExpression.a->traverse(*this);
      switch (additiveExpression.type)
      {
        case AdditiveExpression::Type::ADD:      output << " + "; break;
        case AdditiveExpression::Type::SUBTRACT: output << " - "; break;
      }
      additiveExpression.b->traverse(*this);
    }

    void accept(const ShiftExpression &shiftExpression) override
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
    }

    void accept(const RelationalExpression &relationalExpression) override
    {
//output << "relationalExpression";
      switch (relationalExpression.type)
      {
        case RelationalExpression::Type::NONE:
          break;
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
    }

    void accept(const EqualityExpression &equalityExpression) override
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
    }

    void accept(const AndExpression &andExpression) override
    {
//output << "andExpression";
      andExpression.a->traverse(*this);
      output << " & ";
      andExpression.b->traverse(*this);
    }

    void accept(const ExclusiveOrExpression &exclusiveOrExpression) override
    {
//output << "exclusiveOrExpression";
      exclusiveOrExpression.a->traverse(*this);
      output << " ^ ";
      exclusiveOrExpression.b->traverse(*this);
    }

    void accept(const InclusiveOrExpression &inclusiveOrExpression) override
    {
//output << "inclusiveOrExpression";
      inclusiveOrExpression.a->traverse(*this);
      output << " | ";
      inclusiveOrExpression.b->traverse(*this);
    }

    void accept(const LogicalAndExpression &logicalAndExpression) override
    {
//output << "logicalAndExpression";
      logicalAndExpression.a->traverse(*this);
      output << " && ";
      logicalAndExpression.b->traverse(*this);
    }

    void accept(const LogicalOrExpression &logicalOrExpression) override
    {
//output << "logicalOrExpression";
      logicalOrExpression.a->traverse(*this);
      output << " || ";
      logicalOrExpression.b->traverse(*this);
    }

    void accept(const ConditionalExpression &conditionalExpression) override
    {
//output << "conditionalExpression";
      conditionalExpression.condition->traverse(*this);
      output << " ? ";
      conditionalExpression.a->traverse(*this);
      output << " : ";
      conditionalExpression.b->traverse(*this);
    }

    void accept(const AssignmentExpression &assignmentExpression) override
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
    }

    void accept(const Initializer &initializer) override
    {
      initializer.expression->traverse(*this);
    }

    void accept(const InitDeclarator &initDeclarator) override
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
    }

    void accept(const Declaration &declaration) override
    {
      if (declaration.storageClassDeclarationSpecifiers != nullptr)
      {
        declaration.storageClassDeclarationSpecifiers->traverse(*this);
        output << " ";
      }
      declaration.initDeclaratorList->traverse(*this); output << ';';
    }

    void accept(const DeclarationStatementList &declarationStatementList) override
    {
      for (const DeclarationStatement *declarationStatement : declarationStatementList)
      {
        output << indentSpaces(); declarationStatement->traverse(*this); output << std::endl;
      }
    }

    void accept(const CompoundStatement &compoundStatement) override
    {
      output << "#line " << compoundStatement.first.line << " \"" << filePath << "\"" << std::endl;
      output << indentSpaces() << "{" << std::endl;
      indent([this,&compoundStatement]()
      {
        if (compoundStatement.declarationStatementList != nullptr)
        {
          compoundStatement.declarationStatementList->traverse(*this);
        }
      });
      output << indentSpaces() << "}";
    }

    void accept(const IfStatement &ifStatement) override
    {
      output << "#line " << ifStatement.first.line << " \"" << filePath << "\"" << std::endl;
      output << indentSpaces() << "if ("; ifStatement.condition->traverse(*this); output << ")" << std::endl;
      output << indentSpaces(); ifStatement.ifStatement->traverse(*this);
      if (ifStatement.elseStatement != nullptr)
      {
        output << std::endl << indentSpaces() << "else" << std::endl;
        output << indentSpaces(); ifStatement.elseStatement->traverse(*this);
      }
    }

    void accept(const SwitchStatement &switchStatement) override
    {
      output << "#line " << switchStatement.first.line << " \"" << filePath << "\"" << std::endl;
      output << indentSpaces() << "switch ("; switchStatement.expression->traverse(*this); output << ")" << std::endl;
      indent();
      output << indentSpaces(); switchStatement.statement->traverse(*this);
    }

    void accept(const LabeledStatement &labeledStatement) override
    {
      output << "#line " << labeledStatement.first.line << " \"" << filePath << "\"" << std::endl;
      switch (labeledStatement.type)
      {
        case LabeledStatement::Type::CASE:
          unindent();
          output << indentSpaces() << "case "; labeledStatement.constantExpression->traverse(*this); output << ":" << std::endl;
          indent();
          output << indentSpaces(); labeledStatement.statement->traverse(*this);
          break;
        case LabeledStatement::Type::DEFAULT:
          unindent();
          output << indentSpaces() << "default:" << std::endl;
          indent();
          output << indentSpaces(); labeledStatement.statement->traverse(*this);
          break;
      }
    }

    void accept(const ForStatement &forStatement) override
    {
      output << "#line " << forStatement.first.line << " \"" << filePath << "\"" << std::endl;
      output << indentSpaces() << "for (";
      forStatement.init->traverse(*this);
      output << " ";
      forStatement.condition->traverse(*this);
      output << " ";
      if (forStatement.increment != nullptr)
      {
        forStatement.increment->traverse(*this);
      }
      output << ")" << std::endl;
      output << indentSpaces(); forStatement.statement->traverse(*this);
    }

    void accept(const WhileStatement &whileStatement) override
    {
      output << "#line " << whileStatement.first.line << " \"" << filePath << "\"" << std::endl;
      output << indentSpaces() << "while ("; whileStatement.condition->traverse(*this); output << ")" << std::endl;
      output << indentSpaces(); whileStatement.statement->traverse(*this);
    }

    void accept(const DoStatement &doStatement) override
    {
      output << "#line " << doStatement.first.line << " \"" << filePath << "\"" << std::endl;
      output << indentSpaces() << "do" << std::endl;
      output << indentSpaces(); doStatement.statement->traverse(*this); output << std::endl;
      output << indentSpaces() << "while (";
      doStatement.condition->traverse(*this);
      output << ");" << std::endl;
      output << indentSpaces();
    }

    void accept(const ExpressionStatement &expressionStatement) override
    {
      output << "#line " << expressionStatement.first.line << " \"" << filePath << "\"" << std::endl;
      output << indentSpaces(); expressionStatement.expression->traverse(*this); output << ';';
    }

    void accept(const JumpStatement &jumpStatement) override
    {
      output << "#line " << jumpStatement.first.line << " \"" << filePath << "\"" << std::endl;
      switch (jumpStatement.type)
      {
        case JumpStatement::Type::CONTINUE:
          output << std::endl << indentSpaces() << "continue" << ';';
          break;
        case JumpStatement::Type::BREAK:
          output << std::endl << indentSpaces() << "break" << ';';
          break;
        case JumpStatement::Type::RETURN:
          output << "return";
          if (jumpStatement.expression != nullptr)
          {
            output << " ";
            jumpStatement.expression->traverse(*this);
          }
          output << ";";
        break;
      }
    }

    void accept(const NewStateStatement &newStateStatement) override
    {
      output << "#line " << newStateStatement.first.line << " \"" << filePath << "\"" << std::endl;
      output << indentSpaces() << "do" << std::endl;
      output << indentSpaces() << "{" << std::endl;
      indent();
      switch (newStateStatement.prefixOperator)
      {
        case NewStateStatement::PrefixOperator::PUSH:
          if (ast.hasStateStack())
          {
            if (ast.isAsserts())
            {
              output << indentSpaces() << "assert(" << name("stateStackIndex") << " < " << ast.getStateStackSize() << ");" << std::endl;
            }
            output << indentSpaces() << name("stateStack") << "[" << name("stateStackIndex") << "] = " << name("state") << ";" << std::endl;
            output << indentSpaces() << name("stateStackIndex") <<"++;" << std::endl;
          }
          else
          {
            throw std::runtime_error("no state stack defined");
          }
          break;
        case NewStateStatement::PrefixOperator::RESET:
          if (ast.hasStateStack())
          {
            output << indentSpaces() << name("stateStackIndex") <<" = 0;" << std::endl;
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
          output << indentSpaces() << name("state") << " = " << name(ast.getStartState()->name) << ";" << std::endl;
          break;
        case NewStateStatement::Type::DEFAULT:
          output << indentSpaces() << name("state") << " = " << name("STATE_DEFAULT") << ";" << std::endl;
          break;
        case NewStateStatement::Type::POP:
          if (ast.hasStateStack())
          {
            if (ast.isAsserts())
            {
              output << indentSpaces() << "assert(" << name("stateStackIndex") << " > 0);" << std::endl;
              output << indentSpaces() << name("stateStackIndex") << "--;" << std::endl;
              output << indentSpaces() << name("state") << " = " << name("stateStack") << "[" << name("stateStackIndex") << "];" << std::endl;
            }
            else
            {
              output << indentSpaces() << "if (" << name("stateStackIndex") << " > 0)" << std::endl;
              output << indentSpaces() << "{" << std::endl;
              indent([this]()
              {
                output << indentSpaces() << name("stateStackIndex") << "--;" << std::endl;
                output << indentSpaces() << name("state") << " = " << name("stateStack") << "[" << name("stateStackIndex") << "];" << std::endl;
              });
              output << indentSpaces() << "}" << std::endl;
            }
          }
          else
          {
            throw std::runtime_error("no state stack defined");
          }
          break;
        case NewStateStatement::Type::CUSTOM:
          output << indentSpaces() << name("state") << " = " << name(newStateStatement.name) << ";" << std::endl;
          if (!logFunction.empty())
          {
            output << indentSpaces() << expandMacros(logFunction,newStateStatement.name) << ";" << std::endl;
          }
          break;
      }
      output << indentSpaces() << "goto " << name("end") << ";" << std::endl;
      unindent();
      output << indentSpaces() << "}" << std::endl;
      output << indentSpaces() << "while (0);" << std::endl;
      output << indentSpaces() << "#line " << newStateStatement.first.line << " \"" << filePath << "\"" << std::endl;
    }

    void accept(const CPPIfStatement &cppIfStatement) override
    {
      switch (cppIfStatement.type)
      {
        case CPPIfStatement::Type::IFDEF:
          output << "#ifdef " << cppIfStatement.identifier << std::endl;
          indent([this,&cppIfStatement]()
          {
            cppIfStatement.ifDeclarationStatementList->traverse(*this);
          });
          if (cppIfStatement.elseDeclarationStatementList != nullptr)
          {
            output << indentSpaces() << "#else" << std::endl;
            indent([this,&cppIfStatement]()
            {
              cppIfStatement.elseDeclarationStatementList->traverse(*this);
            });
          }
          output << indentSpaces() << "#endif" << std::endl;
          break;
        case CPPIfStatement::Type::IFNDEF:
          output << "#ifndef " << cppIfStatement.identifier << std::endl;
          indent([this,&cppIfStatement]()
          {
            cppIfStatement.ifDeclarationStatementList->traverse(*this);
          });
          if (cppIfStatement.elseDeclarationStatementList != nullptr)
          {
            output << indentSpaces() << "#else" << std::endl;
            indent([this,&cppIfStatement]()
            {
              cppIfStatement.elseDeclarationStatementList->traverse(*this);
            });
          }
          output << indentSpaces() << "#endif" << std::endl;
          break;
        case CPPIfStatement::Type::IF:
          output << "#if " << cppIfStatement.identifier << std::endl;
          indent([this,&cppIfStatement]()
          {
            cppIfStatement.ifDeclarationStatementList->traverse(*this);
          });
          if (cppIfStatement.elseDeclarationStatementList != nullptr)
          {
            output << indentSpaces() << "#else" << std::endl;
            indent([this,&cppIfStatement]()
            {
              cppIfStatement.elseDeclarationStatementList->traverse(*this);
            });
          }
          output << indentSpaces() << "#endif" << std::endl;
          break;
      }
    }

  private:
    std::ostream        &output;
    const AST           &ast;
    const std::string   &filePath;
    size_t              startIndentCount;
    size_t              indentCount;
    const std::string   &logFunction;
    std::string         suffix;
    const State         *currentState;

    std::vector<size_t> indentions{0};
    std::string         indentString;

    std::string expandMacros(const std::string &logFunction, const std::string &toStateName)
    {
      std::regex fromStateNameRegEx("@fromStateName@");
      std::regex toStateNameRegEx("@toStateName@");

      std::string string = logFunction;
      string = std::regex_replace(string,fromStateNameRegEx,currentState->name);
      string = std::regex_replace(string,toStateNameRegEx,toStateName);
      return string;
    }
};

void CodeGenerator::generate(const AST &ast)
{
  CVisitor visitor(output, ast, filePath, indentCount, logFunction);

  visitor.indent(startIndentCount);
  visitor.indent([this,&visitor,&ast]()
  {
    output << visitor.indentSpaces() << "" << std::endl;
    output << visitor.indentSpaces() << "// FSM start " << ast.getFSMName() << std::endl;
    output << visitor.indentSpaces() << "{" << std::endl;
    visitor.indent();
    visitor.run(ast);
    visitor.unindent();
    output << visitor.indentSpaces() << "}" << std::endl;
    output << visitor.indentSpaces() << "// FSM end" << std::endl;
    output << visitor.indentSpaces() << "#line " << ast.getFSMLocation().last.line << " \"" << filePath << "\"" << std::endl;
  });
  visitor.unindent();
}

/* end of file */
