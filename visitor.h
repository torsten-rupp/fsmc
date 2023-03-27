/***********************************************************************\
*
* Contents:
* Systems: all
*
\***********************************************************************/
#ifndef VISITOR_H
#define VISITOR_H

/****************************** Includes *******************************/
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <functional>

/****************** Conditional compilation switches *******************/

/***************************** Constants *******************************/

/***************************** Datatypes *******************************/
namespace FSM
{

class TextBlock
{
  public:
    TextBlock()
      : lines()
      , columns(0)
    {
    }

    TextBlock(const std::string &line)
      : lines{line}
      , columns(line.length())
    {
    }

    void operator+(const std::string &line)
    {
    }

    void operator+(const TextBlock &textBlock)
    {
    }

  private:
    std::vector<std::string> lines;
    uint                     columns;
};

class TypeDeclaration;
class Expression;
class StateList;
class State;
class BlockStatement;
class VariableDeclarationStatement;
class AssignmentStatement;
class IfStatement;
class ForStatement;
class WhileStatement;
class DoStatement;
class ExpressionStatement;
class NewStateStatement;
class AST;
class Visitor
{
  public:
    enum class Phases
    {
      ENTER,
      LEAVE
    };

    Visitor()
    {
    }
//    Visitor(const Visitor &) = delete;
//    Visitor(const Visitor &&) = delete;

    void run(const AST &ast);

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

    virtual void accept(Phases phase, const TypeDeclaration &typeDeclaration) = 0;
    virtual void accept(Phases phase, const Expression &expression) = 0;
    virtual void accept(Phases phase, const BlockStatement &blockStatement) = 0;
    virtual void accept(Phases phase, const VariableDeclarationStatement &variableDeclarationStatement) = 0;
    virtual void accept(Phases phase, const AssignmentStatement &assignmentStatement) = 0;
    virtual void accept(Phases phase, const IfStatement &ifStatement) = 0;
    virtual void accept(Phases phase, const ForStatement &forStatement) = 0;
    virtual void accept(Phases phase, const WhileStatement &whileStatement) = 0;
    virtual void accept(Phases phase, const DoStatement &doStatement) = 0;
    virtual void accept(Phases phase, const ExpressionStatement &expressionStatement) = 0;
    virtual void accept(Phases phase, const NewStateStatement &newStateStatement) = 0;
    virtual void accept(Phases phase, const State &state) = 0;
    virtual void accept(Phases phase, const StateList &stateList) = 0;
  
  private:
    std::vector<uint> indentions{0};
    std::string       indentString;
};

} // namespace FSM

/***************************** Variables *******************************/

/***************************** Forwards ********************************/

/***************************** Functions *******************************/

#endif // VISITOR_H

/* end of file */
