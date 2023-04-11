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

/****************** Conditional compilation switches *******************/

/***************************** Constants *******************************/

/***************************** Datatypes *******************************/
namespace FSM
{

class State;
class StateList;

class Expression;
class PrimaryExpression;
class ArgumentExpressionList;
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
class TypeQualifier;
class TypeSpecifier;
class DeclarationSpecifier;
class StorageClassDeclarationSpecifiers;
class Declaration;
class DirectDeclarator;
class Declarator;
class Initializer;
class InitDeclarator;

class Statement;
class DeclarationStatement;
class DeclarationStatementList;
class Expression;
class CompoundStatement;
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
    class Exception
    {
    };

    enum class Phases
    {
      PRE,
      POST
    };

    Visitor()
    {
    }

// TODO: AST const &ast
    void run(const AST &ast);

    #define ACCEPT(type) \
      virtual void accept(const type &) \
      { \
        throw Exception(); \
      }; \
      virtual void accept(Phases phase, const type &) \
      { \
      }

    ACCEPT(State);
    ACCEPT(StateList);

    ACCEPT(Expression);
    ACCEPT(PrimaryExpression);
    ACCEPT(ArgumentExpressionList);
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
    ACCEPT(TypeQualifier);
    ACCEPT(TypeSpecifier);
    ACCEPT(DeclarationSpecifier);
    ACCEPT(StorageClassDeclarationSpecifiers);
    ACCEPT(Declaration);
    ACCEPT(DirectDeclarator);
    ACCEPT(Declarator);
    ACCEPT(Initializer);
    ACCEPT(InitDeclarator);

    ACCEPT(Statement);
    ACCEPT(DeclarationStatement);
    ACCEPT(DeclarationStatementList);
    ACCEPT(CompoundStatement);
    ACCEPT(IfStatement);
    ACCEPT(ForStatement);
    ACCEPT(WhileStatement);
    ACCEPT(DoStatement);
    ACCEPT(ExpressionStatement);
    ACCEPT(JumpStatement);
    ACCEPT(NewStateStatement);
  private:
};

} // namespace FSM

/***************************** Variables *******************************/

/***************************** Forwards ********************************/

/***************************** Functions *******************************/

#endif // VISITOR_H

/* end of file */
