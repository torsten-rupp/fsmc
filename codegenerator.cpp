/***********************************************************************\
*
* Contents: C code generator
* Systems: all
*
\***********************************************************************/

/****************************** Includes *******************************/

#include <iostream>
#include <sstream>

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
  public:
    CVisitor(std::ostream &output, const AST &ast)
      : Visitor()
      , ast(ast)
      , output(output)
    {
    }
//    CVisitor(const CVisitor &) = delete;
//    CVisitor(const CVisitor &&) = delete;

#if 0
    bool accept(Phases phase, const TypeAttributeList &typeAttributeList)
    {
      switch (phase)
      {
        case Phases::PRE:
          {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__);
#if 0
            for (TypeAttribute typeAttribute : typeAttributeList)
            {
              switch (typeAttribute)
              {
                case FSM::TypeAttribute::EXTERN:   output << "extern";   break;
                case FSM::TypeAttribute::TYPEDEF:  output << "typedef";  break;
                case FSM::TypeAttribute::CONST:    output << "const";    break;
                case FSM::TypeAttribute::STATIC:   output << "static";   break;
                case FSM::TypeAttribute::VOLATILE: output << "volatile"; break;
                case FSM::TypeAttribute::STRUCT:   output << "struct";   break;
              }
              output << " ";
            }
#endif
          }
          break;
        case Phases::POST:
          break;
      }
    }

    bool accept(Phases phase, const TypeDeclaration &typeDeclaration)
    {
      switch (phase)
      {
        case Phases::PRE:
          accept(Phases::PRE, typeDeclaration.typeAttributeList);
// TODO:
output << typeDeclaration.name;
          break;
        case Phases::POST:
          break;
      }
    }
#endif

    bool accept(Phases phase, const State &state) override
    {
      switch (phase)
      {
        case Phases::PRE:
          indent(2);
          output << indent() << "case " << state.name << ":" << std::endl;
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
            output << indent() << "typedef enum" << std::endl;
            output << indent() <<"{" << std::endl;
            indent(2,[&]()
            {
              for (const State &state : stateList)
              {
                output << indent() << state.name << "," << std::endl;
              }
            });
            output << indent() <<"} States;" << std::endl;

            output << indent() << "static States state";
            std::string startState = ast.getStartState();
            if (!startState.empty())
            {
              output << " = " << startState;
            }
            output << ";" << std::endl;
            output << std::endl;

            output << indent() << "switch (state)" << std::endl;
            output << indent() << "{" << std::endl;
          }
          break;
        case Phases::POST:
          output << indent() <<"}" << std::endl;
          break;
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

    bool accept(const DeclarationSpecifiers &declarationSpecifiers) override
    {
      bool first = true;
      for (const DeclarationSpecifier *declarationSpecifier : declarationSpecifiers)
      {
        if (!first) output << " ";
        declarationSpecifier->travers(*this);
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
      declarator.directDeclarator.travers(*this);

      return true;
    }

    bool accept(const Initializer &initializer) override
    {
      initializer.expression->travers(*this);

      return true;
    }

    bool accept(const InitDeclarator &initDeclarator) override
    {
      if (!initDeclarator.declarationSpecifiers.empty())
      {
        initDeclarator.declarationSpecifiers.travers(*this);
        output << " ";
      }
      initDeclarator.declarator.travers(*this);
      if (initDeclarator.initializer != nullptr)
      {
        output << " = ";
        initDeclarator.initializer->travers(*this);
      }

      return true;
    }

    bool accept(const Declaration &declaration) override
    {
      output << indent();
      if (!declaration.declarationSpecifiers.empty())
      {
        declaration.declarationSpecifiers.travers(*this);
        output << " ";
      }
      declaration.initDeclaratorList.travers(*this);
      output << ";" << std::endl;

      return true;
    }

    bool accept(const Expression &expression) override
    {
      switch (expression.operator_)
      {
        case Expression::Operator::IDENTIFIER:     output << expression.identifier; break;
        case Expression::Operator::STRING:         output << "\"" << expression.string << "\""; break;
        case Expression::Operator::NUMBER:         output << expression.number.toString(); break;

        case Expression::Operator::ASSIGN:         expression.nodes[0]->travers(*this); output << " = "; expression.nodes[1]->travers(*this); break;

        case Expression::Operator::NEGATIVE:       output << "-"; expression.nodes[0]->travers(*this); break;
        case Expression::Operator::PLUS:           expression.nodes[0]->travers(*this); output << " + "; expression.nodes[1]->travers(*this); break;
        case Expression::Operator::MINUS:          expression.nodes[0]->travers(*this); output << " - "; expression.nodes[1]->travers(*this); break;
        case Expression::Operator::MULTIPLY:       expression.nodes[0]->travers(*this); output << " * "; expression.nodes[1]->travers(*this); break;
        case Expression::Operator::DIVIDE:         expression.nodes[0]->travers(*this); output << " / "; expression.nodes[1]->travers(*this); break;
        case Expression::Operator::INCREMENT:      expression.nodes[0]->travers(*this); output << "++"; break;
        case Expression::Operator::DECREMENT:      expression.nodes[0]->travers(*this); output << "--"; break;

        case Expression::Operator::EQUALS:         expression.nodes[0]->travers(*this); output << " == "; expression.nodes[1]->travers(*this); break;
        case Expression::Operator::NOT_EQUALS:     expression.nodes[0]->travers(*this); output << " != "; expression.nodes[1]->travers(*this); break;
        case Expression::Operator::LOWER:          expression.nodes[0]->travers(*this); output << " < "; expression.nodes[1]->travers(*this); break;
        case Expression::Operator::GREATER:        expression.nodes[0]->travers(*this); output << " > "; expression.nodes[1]->travers(*this); break;
        case Expression::Operator::LOWER_EQUALS:   expression.nodes[0]->travers(*this); output << " <= "; expression.nodes[1]->travers(*this); break;
        case Expression::Operator::GREATER_EQUALS: expression.nodes[0]->travers(*this); output << " >= "; expression.nodes[1]->travers(*this); break;

        case Expression::Operator::NOT:            output << "!"; expression.nodes[0]->travers(*this); break;
        case Expression::Operator::AND:            expression.nodes[0]->travers(*this); output << " && "; expression.nodes[1]->travers(*this); break;
        case Expression::Operator::OR:             expression.nodes[0]->travers(*this); output << " || "; expression.nodes[1]->travers(*this); break;
        case Expression::Operator::XOR:            expression.nodes[0]->travers(*this); output << " ^ "; expression.nodes[1]->travers(*this); break;

        case Expression::Operator::FUNCTION_CALL:
          {
            bool first = true;

            output << expression.functionName << "(";
            for (const Expression *argument : expression.argumentList)
            {
              if (!first) output << ", ";
              output << argument;

              first = false;
            }
            output << ")";
          }
          break;

        case Expression::Operator::CAST:           output << "("; expression.nodes[0]->travers(*this); output << ")"; expression.nodes[1]->travers(*this); break;

        default:
          break;
      }

      return true;
    }

    bool accept(const PrimaryExpression &primaryExpression) override
    {
      switch (primaryExpression.type)
      {
        case PrimaryExpression::Type::IDENTIFIER: output << primaryExpression.identifier; break;
        case PrimaryExpression::Type::INTEGER:    output << primaryExpression.n; break;
        case PrimaryExpression::Type::STRING:     output << primaryExpression.string; break;
        case PrimaryExpression::Type::EXPRESSION:
          output << '(';
          primaryExpression.expression->travers(*this);
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
          postfixExpression.a->travers(*this);
          output << "[";
          postfixExpression.b->travers(*this);
          output << "]";
          break;
        case PostfixExpression::Type::FUNCTION_CALL:
          postfixExpression.a->travers(*this);
          output << "(";
          if (postfixExpression.b != nullptr)
          {
            postfixExpression.b->travers(*this);
          }
          output << ")";
          break;
        case PostfixExpression::Type::MEMBER:
          postfixExpression.a->travers(*this);
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
      unaryExpression.expression->travers(*this);

      return true;
    }

    bool accept(const CastExpression &castExpression) override
    {
//output << "castExpression";
//fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
      castExpression.expression->travers(*this);

      return true;
    }

    bool accept(const MultiplicativeExpression &multiplicativeExpression) override
    {
//output << "multiplicativeExpression";
      multiplicativeExpression.a->travers(*this);
      switch (multiplicativeExpression.type)
      {
        case MultiplicativeExpression::Type::MULTIPLY: output << " * "; break;
        case MultiplicativeExpression::Type::DIVIDE:   output << " / "; break;
      }
      multiplicativeExpression.b->travers(*this);

      return true;
    }

    bool accept(const AdditiveExpression &additiveExpression) override
    {
//output << "additiveExpression";
      additiveExpression.a->travers(*this);
      switch (additiveExpression.type)
      {
        case AdditiveExpression::Type::ADD:      output << " + "; break;
        case AdditiveExpression::Type::SUBTRACT: output << " - "; break;
      }
      additiveExpression.b->travers(*this);

      return true;
    }

    bool accept(const ShiftExpression &shiftExpression) override
    {
//output << "shiftExpression";
      shiftExpression.a->travers(*this);
      if (shiftExpression.b != nullptr)
      {
        switch (shiftExpression.type)
        {
          case ShiftExpression::Type::LEFT:  output << " << "; break;
          case ShiftExpression::Type::RIGHT: output << " >> "; break;
        }
        shiftExpression.b->travers(*this);
      }

      return true;
    }

    bool accept(const RelationalExpression &relationalExpression) override
    {
//output << "relationalExpression";
      switch (relationalExpression.type)
      {
        case RelationalExpression::Type::LOWER:
          relationalExpression.a->travers(*this);
          output << " < ";
          relationalExpression.b->travers(*this);
          break;
        case RelationalExpression::Type::LOWER_EQUALS:
          relationalExpression.a->travers(*this);
          output << " <= ";
          relationalExpression.b->travers(*this);
          break;
        case RelationalExpression::Type::GREATER:
          relationalExpression.a->travers(*this);
          output << " > ";
          relationalExpression.b->travers(*this);
          break;
        case RelationalExpression::Type::GREATER_EQUALS:
          relationalExpression.a->travers(*this);
          output << " >= ";
          relationalExpression.b->travers(*this);
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
          equalityExpression.a->travers(*this);
          output << " == ";
          equalityExpression.b->travers(*this);
          break;
        case EqualityExpression::Type::NOT_EQUALS:
          equalityExpression.a->travers(*this);
          output << " != ";
          equalityExpression.b->travers(*this);
          break;
      }

      return true;
    }

    bool accept(const AndExpression &andExpression) override
    {
//output << "andExpression";
      andExpression.a->travers(*this);
      output << " & ";
      andExpression.b->travers(*this);

      return true;
    }

    bool accept(const ExclusiveOrExpression &exclusiveOrExpression) override
    {
//output << "exclusiveOrExpression";
      exclusiveOrExpression.a->travers(*this);
      output << " ^ ";
      exclusiveOrExpression.b->travers(*this);

      return true;
    }

    bool accept(const InclusiveOrExpression &inclusiveOrExpression) override
    {
//output << "inclusiveOrExpression";
      inclusiveOrExpression.a->travers(*this);
      output << " | ";
      inclusiveOrExpression.b->travers(*this);

      return true;
    }

    bool accept(const LogicalAndExpression &logicalAndExpression) override
    {
//output << "logicalAndExpression";
      logicalAndExpression.a->travers(*this);
      output << " && ";
      logicalAndExpression.b->travers(*this);

      return true;
    }

    bool accept(const LogicalOrExpression &logicalOrExpression) override
    {
//output << "logicalOrExpression";
      logicalOrExpression.a->travers(*this);
      output << " || ";
      logicalOrExpression.b->travers(*this);

      return true;
    }

    bool accept(const ConditionalExpression &conditionalExpression) override
    {
//output << "conditionalExpression";
      conditionalExpression.condition->travers(*this);
      output << " ? ";
      conditionalExpression.a->travers(*this);
      output << " : ";
      conditionalExpression.b->travers(*this);

      return true;
    }

    bool accept(const AssignmentExpression &assignmentExpression) override
    {
      assignmentExpression.a->travers(*this);
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
default:
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
break;
      }
      assignmentExpression.b->travers(*this);

      return true;
    }

    bool accept(const CompoundStatement &compoundStatement) override
    {
      output << indent() << "{" << std::endl;
      indent(2,[&]()
      {
        compoundStatement.declarationStatementList.travers(*this);
      });
      output << indent() << "}" << std::endl;

      return true;
    }

    bool accept(const IfStatement &ifStatement) override
    {
      output << indent() << "if ("; ifStatement.condition->travers(*this); output << ")" << std::endl;
      ifStatement.ifStatement->travers(*this);
      if (ifStatement.elseStatement != nullptr)
      {
        output << indent() << "else" << std::endl;
        ifStatement.elseStatement->travers(*this);
      }

      return true;
    }

    bool accept(const ForStatement &forStatement) override
    {
      output << indent() << "for (";
      forStatement.init->travers(*this);
      output << "; ";
      forStatement.condition->travers(*this);
      output << "; ";
      forStatement.increment->travers(*this);
      output << ")" << std::endl;
      forStatement.statement->travers(*this);
    }

    bool accept(const WhileStatement &whileStatement) override
    {
      output << indent() << "while (";
      whileStatement.condition->travers(*this);
      output << ")" << std::endl;
      whileStatement.statement->travers(*this);
    }

    bool accept(const DoStatement &doStatement) override
    {
      output << indent() << "do" << std::endl;
      doStatement.statement->travers(*this);
      output << indent() << "while (";
      doStatement.condition->travers(*this);
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
            jumpStatement.expression->travers(*this);
          }
          output << ";" << std::endl;
        break;
      }

      return true;
    }

    bool accept(const NewStateStatement &newStateStatement) override
    {
      output << indent() << "state = " << newStateStatement.name << ";" << std::endl;

      return true;
    }

  private:
    std::ostream &output;
    const AST    &ast;
};

void CodeGenerator::generate(AST &ast, uint indent)
{
  CVisitor visitor(output, ast);

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
