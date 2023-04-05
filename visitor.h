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

class State;
class StateList;

class Expression;
class PrimaryExpression;
class PostfixExpression;
class UnaryExpression;
class CastExpression;
class MultiplicativeExpression;
class AdditiveExpression;
class ShiftExpression;
class RelationalExpression;
class EqualityExpression;
class AndExpression;
class ExclusiveOrExpression;
class InclusiveOrExpression;
class LogicalAndExpression;
class LogicalOrExpression;
class ConditionalExpression;
class AssignmentExpression;

class StorageClassSpecifier;
//class StorageClassSpecifiers;
class TypeQualifier;
class TypeSpecifier;
class DeclarationSpecifier;
class DeclarationSpecifiers;
class Declaration;
class DirectDeclarator;
class Declarator;
class Initializer;
class InitDeclarator;

class Statement;
class DeclarationStatement;
class DeclarationStatementList;
class TypeAttributeList;
class TypeDeclaration;
class Expression;
class CompoundStatement;
class VariableDeclarationStatement;
class AssignmentStatement;
class IfStatement;
class ForStatement;
class WhileStatement;
class DoStatement;
class ExpressionStatement;
class JumpStatement;
class NewStateStatement;
class AST;
class Visitor
{
  public:
    enum class Phases
    {
      PRE,
      POST
    };

    Visitor()
    {
    }
//    Visitor(const Visitor &) = delete;
//    Visitor(const Visitor &&) = delete;

// TODO: AST const &ast
    void run(AST &ast);

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

    #define ACCEPT(type) \
      virtual bool accept(const type &) \
      { \
        return false; \
      }; \
      virtual bool accept(Phases phase, const type &) \
      { \
        return false; \
      }

    ACCEPT(State);

    ACCEPT(Expression);
    ACCEPT(PrimaryExpression);
    ACCEPT(PostfixExpression);
    ACCEPT(UnaryExpression);
    ACCEPT(CastExpression);
    ACCEPT(MultiplicativeExpression);
    ACCEPT(AdditiveExpression);
    ACCEPT(ShiftExpression);
    ACCEPT(RelationalExpression);
    ACCEPT(EqualityExpression);
    ACCEPT(AndExpression);
    ACCEPT(ExclusiveOrExpression);
    ACCEPT(InclusiveOrExpression);
    ACCEPT(LogicalAndExpression);
    ACCEPT(LogicalOrExpression);
    ACCEPT(ConditionalExpression);
    ACCEPT(AssignmentExpression);

    ACCEPT(StorageClassSpecifier);
//    ACCEPT(StorageClassSpecifiers);
    ACCEPT(TypeQualifier);
    ACCEPT(TypeSpecifier);
    ACCEPT(DeclarationSpecifier);
    ACCEPT(DeclarationSpecifiers);
    ACCEPT(Declaration);
    ACCEPT(DirectDeclarator);
    ACCEPT(Declarator);
    ACCEPT(Initializer);
    ACCEPT(InitDeclarator);

    ACCEPT(Statement);
    ACCEPT(DeclarationStatement);
    ACCEPT(DeclarationStatementList);
//    virtual bool accept(Phases phase, const TypeAttributeList &typeAttributeList) = 0;
    ACCEPT(TypeDeclaration);
    ACCEPT(CompoundStatement);
//    virtual bool accept(Phases phase, const VariableDeclarationStatement &variableDeclarationStatement) = 0;
//    virtual bool accept(Phases phase, const AssignmentStatement &assignmentStatement) = 0;
    ACCEPT(IfStatement);
    ACCEPT(ForStatement);
    ACCEPT(WhileStatement);
    ACCEPT(DoStatement);
    ACCEPT(ExpressionStatement);
    ACCEPT(JumpStatement);
    ACCEPT(NewStateStatement);
    virtual bool accept(Phases phase, const StateList &stateList) = 0;

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
