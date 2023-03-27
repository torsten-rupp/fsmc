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

    void accept(Phases phase, const TypeDeclaration &typeDeclaration)
    {
      switch (phase)
      {
        case Phases::ENTER:
// TODO:
output << typeDeclaration.name;
          break;
        case Phases::LEAVE:
          break;
      }
    }

    void accept(Phases phase, const Expression &expression)
    {
      switch (phase)
      {
        case Phases::ENTER:
          switch (expression.operator_)
          {
            case Expression::Operators::IDENTIFIER:     output << expression.identifier; break;
            case Expression::Operators::STRING:         output << "\"" << expression.string << "\""; break;
            case Expression::Operators::NUMBER:         output << expression.number.toString(); break;

            case Expression::Operators::ASSIGN:         expression.nodes[0]->accept(*this); output << " = "; expression.nodes[1]->accept(*this); break;

            case Expression::Operators::NEGATIVE:       output << "-"; expression.nodes[0]->accept(*this); break;
            case Expression::Operators::PLUS:           expression.nodes[0]->accept(*this); output << " + "; expression.nodes[1]->accept(*this); break;
            case Expression::Operators::MINUS:          expression.nodes[0]->accept(*this); output << " - "; expression.nodes[1]->accept(*this); break;
            case Expression::Operators::MULTIPLY:       expression.nodes[0]->accept(*this); output << " * "; expression.nodes[1]->accept(*this); break;
            case Expression::Operators::DIVIDE:         expression.nodes[0]->accept(*this); output << " / "; expression.nodes[1]->accept(*this); break;
            case Expression::Operators::INCREMENT:      expression.nodes[0]->accept(*this); output << "++"; break;
            case Expression::Operators::DECREMENT:      expression.nodes[0]->accept(*this); output << "--"; break;

            case Expression::Operators::EQUALS:         expression.nodes[0]->accept(*this); output << " == "; expression.nodes[1]->accept(*this); break;
            case Expression::Operators::NOT_EQUALS:     expression.nodes[0]->accept(*this); output << " != "; expression.nodes[1]->accept(*this); break;
            case Expression::Operators::LOWER:          expression.nodes[0]->accept(*this); output << " < "; expression.nodes[1]->accept(*this); break;
            case Expression::Operators::GREATER:        expression.nodes[0]->accept(*this); output << " > "; expression.nodes[1]->accept(*this); break;
            case Expression::Operators::LOWER_EQUALS:   expression.nodes[0]->accept(*this); output << " <= "; expression.nodes[1]->accept(*this); break;
            case Expression::Operators::GREATER_EQUALS: expression.nodes[0]->accept(*this); output << " >= "; expression.nodes[1]->accept(*this); break;

            case Expression::Operators::NOT:            output << "!"; expression.nodes[0]->accept(*this); break;
            case Expression::Operators::AND:            expression.nodes[0]->accept(*this); output << " && "; expression.nodes[1]->accept(*this); break;
            case Expression::Operators::OR:             expression.nodes[0]->accept(*this); output << " || "; expression.nodes[1]->accept(*this); break;
            case Expression::Operators::XOR:            expression.nodes[0]->accept(*this); output << " ^ "; expression.nodes[1]->accept(*this); break;

            case Expression::Operators::FUNCTION_CALL:
              {
                bool first = true;

                output << expression.functionName << "(";
                for (const Expression &argument : expression.argumentList)
                {
                  if (!first) output << ", ";
                  output << argument;

                  first = false;
                }
                output << ")";
              }
              break;

            case Expression::Operators::CAST:           output << "("; expression.nodes[0]->accept(*this); output << ")"; expression.nodes[1]->accept(*this); break;

            default:
              break;
          }
          break;
        case Phases::LEAVE:
          break;
      }
    }

    void accept(Phases phase, const State &state)
    {
      switch (phase)
      {
        case Phases::ENTER:
          indent(2);
          output << indent() << "case " << state.name << ":" << std::endl;
          indent(2);
          output << indent() << "{" << std::endl;
          indent(2);
          break;
        case Phases::LEAVE:
          unindent();
          output << indent() << "}" << std::endl;
          output << indent() << "break;" << std::endl;
          unindent();
          unindent();
          break;
      }
    }

    void accept(Phases phase, const StateList &stateList)
    {
      switch (phase)
      {
        case Phases::ENTER:
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
        case Phases::LEAVE:
          output << indent() <<"}" << std::endl;
          break;
      }
    }

    void accept(Phases phase, const BlockStatement &blockStatement)
    {
      switch (phase)
      {
        case Phases::ENTER:
          output << indent() << "{" << std::endl;
          indent(2,[&]()
          {
            blockStatement.statementList.accept(*this);
          });
          output << indent() << "}" << std::endl;
          break;
        case Phases::LEAVE:
          break;
      }
    }

    void accept(Phases phase, const VariableDeclarationStatement &variableDeclarationStatement)
    {
      switch (phase)
      {
        case Phases::ENTER:
           output << indent();
           variableDeclarationStatement.typeDeclaration.accept(*this);
           output << " " << variableDeclarationStatement.name;
           if (variableDeclarationStatement.init.operator_ != Expression::Operators::NONE)
           {
             output << " = ";
             variableDeclarationStatement.init.accept(*this);
           }
           output << ";" << std::endl;
          break;
        case Phases::LEAVE:
          break;
      }
    }

    void accept(Phases phase, const AssignmentStatement &assignmentStatement)
    {
      switch (phase)
      {
        case Phases::ENTER:
           output << indent();
           assignmentStatement.left.accept(*this);
           output << " = ";
           assignmentStatement.right.accept(*this);
           output << ";" << std::endl;
          break;
        case Phases::LEAVE:
          break;
      }
    }

    void accept(Phases phase, const IfStatement &ifStatement)
    {
      switch (phase)
      {
        case Phases::ENTER:
           output << indent() << "if ("; ifStatement.condition.accept(*this); output << ")" << std::endl;
           ifStatement.statement->accept(*this);
           if (ifStatement.elseStatement != nullptr)
           {
             output << indent() << "else" << std::endl;
             ifStatement.elseStatement->accept(*this);
           }
          break;
        case Phases::LEAVE:
          break;
      }
    }

    void accept(Phases phase, const ForStatement &forStatement)
    {
      switch (phase)
      {
        case Phases::ENTER:
           output << indent() << "for (";
           forStatement.init.accept(*this);
           output << "; ";
           forStatement.condition.accept(*this);
           output << "; ";
           forStatement.increment.accept(*this);
           output << ")" << std::endl;
           forStatement.statement->accept(*this);
          break;
        case Phases::LEAVE:
          break;
      }
    }

    void accept(Phases phase, const WhileStatement &whileStatement)
    {
      switch (phase)
      {
        case Phases::ENTER:
           output << indent() << "while (";
           whileStatement.condition.accept(*this);
           output << ")" << std::endl;
           whileStatement.statement->accept(*this);
          break;
        case Phases::LEAVE:
          break;
      }
    }

    void accept(Phases phase, const DoStatement &doStatement)
    {
      switch (phase)
      {
        case Phases::ENTER:
           output << indent() << "do" << std::endl;
           doStatement.statement->accept(*this);
           output << indent() << "while (";
           doStatement.condition.accept(*this);
           output << ");" << std::endl;
          break;
        case Phases::LEAVE:
          break;
      }
    }

    void accept(Phases phase, const ExpressionStatement &expressionStatement)
    {
      switch (phase)
      {
        case Phases::ENTER:
          output << indent();
          break;
        case Phases::LEAVE:
          output << ";" << std::endl;
          break;
      }
    }

    void accept(Phases phase, const NewStateStatement &newStateStatement)
    {
      switch (phase)
      {
        case Phases::ENTER:
          output << indent() << "state = " << newStateStatement.name << ";" << std::endl;
          break;
        case Phases::LEAVE:
          break;
      }
    }

  private:
    std::ostream &output;
    const AST    &ast;
};

void CodeGenerator::generate(const AST &ast, uint indent)
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
