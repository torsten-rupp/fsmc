/***********************************************************************\
*
* Contents:
* Systems: all
*
\***********************************************************************/
#ifndef __VISITOR__
#define __VISITOR__

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

class StorageClassSpecifier;
class TypeQualifier;
class TypeSpecifier;
class DeclarationSpecifier;
class SpecifierQualifierList;
class TypeName;
class AbstractDeclarator;
class StorageClassDeclarationSpecifiers;
class Declaration;
class DirectDeclarator;
class Pointer;
class Declarator;

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

class Initializer;
class InitDeclarator;

class Statement;
class DeclarationStatement;
class DeclarationStatementList;
class Expression;
class LabeledStatement;
class CompoundStatement;
class AssignmentStatement;
class IfStatement;
class SwitchStatement;
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

    #define ACCEPT(Type) \
      virtual void accept(const Type &type) \
      { \
        throw Exception(); \
      }; \
      virtual void accept(Phases phase, const Type &type) \
      { \
      }

    ACCEPT(State);
    ACCEPT(StateList);

    ACCEPT(StorageClassSpecifier);
    ACCEPT(TypeQualifier);
    ACCEPT(TypeSpecifier);
    ACCEPT(DeclarationSpecifier);
    ACCEPT(SpecifierQualifierList);
    ACCEPT(TypeName);
    ACCEPT(AbstractDeclarator);
    ACCEPT(StorageClassDeclarationSpecifiers);
    ACCEPT(Declaration);
    ACCEPT(DirectDeclarator);
    ACCEPT(Pointer);
    ACCEPT(Declarator);

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

    ACCEPT(Initializer);
    ACCEPT(InitDeclarator);

    ACCEPT(Statement);
    ACCEPT(DeclarationStatement);
    ACCEPT(DeclarationStatementList);
    ACCEPT(LabeledStatement);
    ACCEPT(CompoundStatement);
    ACCEPT(IfStatement);
    ACCEPT(SwitchStatement);
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

#endif // __VISITOR__

/* end of file */
