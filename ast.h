/***********************************************************************\
*
* Contents:
* Systems: all
*
\***********************************************************************/
#ifndef AST_H
#define AST_H

/****************************** Includes *******************************/
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>
#include <cassert>

#include "visitor.h"

/****************** Conditional compilation switches *******************/

/***************************** Constants *******************************/

/***************************** Datatypes *******************************/

// TODO: FSM -> AST
namespace FSM
{

class VisitorInterface
{
  public:
    virtual void travers(Visitor &visitor) const = 0;
};

class Identifier : public std::string
{
  public:
    Identifier(const std::string &identifier)
      : std::string(identifier)
    {
    }

    Identifier()
    {
    }

    inline bool operator==(const Identifier &otherIdentifier)
    {
      return *this == otherIdentifier;
    }
};

class String : public std::string
{
  public:
};

//https://cs.wmich.edu/~gupta/teaching/cs4850/sumII06/The%20syntax%20of%20C%20in%20Backus-Naur%20form.htm
#if 0

enum class StructUnionSpecifier
{
  STRUCT,
  UNION
};

class TypeAttributeList : public std::vector<TypeQualifier>
{
  public:
    TypeAttributeList(TypeQualifier typeAttribute)
      : std::vector<TypeQualifier>{typeAttribute}
    {
    }

    TypeAttributeList()
    {
    }

    void add(TypeQualifier typeAttribute)
    {
      push_back(typeAttribute);
    }
};
#endif

class Constant
{
  public:
    enum class Type
    {
      STRING,
      INTEGER,
      FLOAT,
      ENUM
    };

    int64_t     i;
    double      d;
    std::string s;
    int         e;

    Constant(int64_t i)
      : i(i)
    {
    }
};

class Number
{
  public:
    enum class Type
    {
      INTEGER,
      FLOAT
    };

    Type    type;
    int64_t i;
    double  d;

    Number()
      : type(Type::INTEGER)
      , i(0)
    {
    }

    Number(int64_t i)
      : type(Type::INTEGER)
      , i(i)
    {
    }

    Number(double d)
      : type(Type::FLOAT)
      , d(d)
    {
    }

    int64_t operator()() const
    {
      return i;
    }

    std::string toString() const
    {
      switch (type)
      {
        case Type::INTEGER: return std::to_string(i);
        case Type::FLOAT:   return std::to_string(d);
      }
    }
};

#if 0
class TypeDeclaration
{
  public:
    TypeAttributeList typeAttributeList;
    Identifier        name;

    TypeDeclaration(const TypeAttributeList &typeAttributeList, const Identifier &name)
      : typeAttributeList(typeAttributeList)
      , name(name)
    {
    }

    TypeDeclaration(const Identifier &name)
      : name(name)
    {
    }

    TypeDeclaration()
    {
    }

    void travers(Visitor &visitor) const
    {
      if (!visitor.accept(*this))
      {
        visitor.accept(Visitor::Phases::PRE, *this);
        visitor.accept(Visitor::Phases::POST, *this);
      }
    }

    std::string toString() const
    {
// TODO:
      return "XXX";
    }
};
#endif

class Expression;

class ExpressionList : public std::vector<Expression*>
{
  public:
    ExpressionList(Expression *expression)
    {
      push_back(expression);
    }

    ExpressionList()
    {
    }

    virtual ~ExpressionList()
    {
      // TODO: free
    }

    void add(Expression *expression)
    {
      push_back(expression);
    }
};

class Expression : public VisitorInterface
{
  public:
    enum class Operator
    {
      NONE,

      IDENTIFIER,
      STRING,
      NUMBER,

      NEGATIVE,
      PLUS,
      MINUS,
      MULTIPLY,
      DIVIDE,
      INCREMENT,
      DECREMENT,

      EQUALS,
      NOT_EQUALS,
      LOWER,
      GREATER,
      LOWER_EQUALS,
      GREATER_EQUALS,

      AND,
      OR,
      XOR,
      NOT,

      ASSIGN,

      FUNCTION_CALL,
      CAST
    } operator_;
    Expression     *nodes[2];
    Identifier     identifier;
    std::string    string;
    Number         number;
    Identifier     functionName;
    ExpressionList argumentList;

    Expression(const Expression &other)
      : operator_(other.operator_)
//      , nodes{new Expression(other.nodes[0]),new Expression(other.nodes[1])}
      , nodes{other.nodes[0],other.nodes[1]}
      , identifier(other.identifier)
      , number(other.number)
//      , string(other.string)
//      , functionName(other.functionName)
//      , argumentList(other.argumentList)
    {
//      fprintf(stderr,"%s:%d: %p\n",__FILE__,__LINE__,&other.identifier);
//      fprintf(stderr,"%s:%d: s='%s'\n",__FILE__,__LINE__,other.identifier.c_str());
      this->identifier = other.identifier;
    }

    Expression(const Identifier &identifier)
      : operator_(Operator::IDENTIFIER)
      , nodes{nullptr, nullptr}
      , identifier(identifier)
    {
    }

    Expression(const std::string &string)
      : operator_(Operator::STRING)
      , nodes{nullptr, nullptr}
      , string(string)
    {
    }

    Expression(const Number &number)
      : operator_(Operator::NUMBER)
      , nodes{nullptr, nullptr}
      , number(number)
    {
    }

    Expression(Operator operator_, const Expression &expression1, const Expression &expression2)
      : operator_(operator_)
      , nodes{new Expression(expression1), new Expression(expression2)}
    {
    }

    Expression(Operator operator_, const Expression &expression)
      : operator_(operator_)
      , nodes{new Expression(expression), nullptr}
    {
    }

#if 1
    Expression()
      : operator_(Operator::NONE)
      , nodes{nullptr, nullptr}
    {
    }
#endif

    Expression(const Identifier &functionName, ExpressionList &argumentList)
      : operator_(Operator::FUNCTION_CALL)
      , nodes{nullptr, nullptr}
      , functionName(functionName)
      , argumentList(argumentList)
    {
    }

    virtual ~Expression()
    {
//      if (nodes[1] != nullptr) delete(nodes[1]);
//      if (nodes[0] != nullptr) delete(nodes[0]);
    }

// TODO: remove
#if 0
    Expression& operator=(Expression other)
    {
      operator_  = other.operator_;
      nodes[0]   = new Expression(other.nodes[0]);
      nodes[1]   = new Expression(other.nodes[1]);
      identifier = other.identifier;
      number     = other.number;

      return *this;
    }
#endif
#if 0
    Expression& operator=(const Expression &other)
    {
      operator_  = other.operator_;
      nodes[0]   = (other.nodes[0] != nullptr) ? new Expression(other.nodes[0]) : nullptr;
      nodes[1]   = (other.nodes[1] != nullptr) ? new Expression(other.nodes[1]) : nullptr;
      identifier = other.identifier;
      number     = other.number;
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__);

      return *this;
    }
#endif

    virtual void travers(Visitor &visitor) const
    {
      if (!visitor.accept(*this))
      {
        visitor.accept(Visitor::Phases::PRE, *this);
        visitor.accept(Visitor::Phases::POST, *this);
      }
    }

    virtual std::string toString() const
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__);
      switch (operator_)
      {
//        case Operator::NONE:           return "";

        case Operator::IDENTIFIER:     return identifier;
        case Operator::STRING:         return "\""+string+"\"";
        case Operator::NUMBER:         return number.toString();

        case Operator::ASSIGN:         return nodes[0]->toString()+" = "+nodes[1]->toString();

        case Operator::NEGATIVE:       return "-"+nodes[0]->toString();
        case Operator::PLUS:           return nodes[0]->toString()+" + "+nodes[1]->toString();
        case Operator::MINUS:          return nodes[0]->toString()+" - "+nodes[1]->toString();
        case Operator::MULTIPLY:       return nodes[0]->toString()+" * "+nodes[1]->toString();
        case Operator::DIVIDE:         return nodes[0]->toString()+" / "+nodes[1]->toString();
        case Operator::INCREMENT:      return nodes[0]->toString()+"++";
        case Operator::DECREMENT:      return nodes[0]->toString()+"--";

        case Operator::EQUALS:         return nodes[0]->toString()+" == "+nodes[1]->toString();
        case Operator::NOT_EQUALS:     return nodes[0]->toString()+" != "+nodes[1]->toString();
        case Operator::LOWER:          return nodes[0]->toString()+" < "+nodes[1]->toString();
        case Operator::GREATER:        return nodes[0]->toString()+" > "+nodes[1]->toString();
        case Operator::LOWER_EQUALS:   return nodes[0]->toString()+" <= "+nodes[1]->toString();
        case Operator::GREATER_EQUALS: return nodes[0]->toString()+" >= "+nodes[1]->toString();

        case Operator::NOT:            return "!"+nodes[0]->toString();
        case Operator::AND:            return nodes[0]->toString()+" && "+nodes[1]->toString();
        case Operator::OR:             return nodes[0]->toString()+" || "+nodes[1]->toString();
        case Operator::XOR:            return nodes[0]->toString()+" ^ "+nodes[1]->toString();

        case Operator::FUNCTION_CALL:
          {
            std::stringstream buffer;
            bool              first = true;

            buffer << functionName << "(";
            for (const Expression *argument : argumentList)
            {
              if (!first) buffer << ", ";
              buffer << argument;

              first = false;
            }
            buffer << ")";

            return buffer.str();
          }

        case Operator::CAST:           return "("+nodes[0]->toString()+")"+nodes[1]->toString();

        default:
          throw std::runtime_error("unknown operator");
      }

      return "";
    }

    friend std::ostream& operator<<(std::ostream& outputStream, const Expression *expression)
    {
      outputStream << expression->toString();

      return outputStream;
    }
};

class AssignmentExpression;

class ArgumentExpressionList : public std::vector<Expression*>
{
  public:
    ArgumentExpressionList(Expression *expression)
      : std::vector<Expression*>{expression}
    {
    }

    ArgumentExpressionList()
    {
    }

    void add(Expression *expression)
    {
      push_back(expression);
    }
};


class PrimaryExpression : public Expression
{
  public:
    enum class Type
    {
      IDENTIFIER,
      INTEGER,
      STRING,
      EXPRESSION
    };

    Type              type;
    Identifier        identifier;
    int64_t           n;
    const std::string string;
    const Expression  *expression;

    PrimaryExpression(const Identifier &identifier)
      : type(Type::IDENTIFIER)
      , identifier(identifier)
    {
    }

    PrimaryExpression(int64_t n)
      : type(Type::INTEGER)
      , n(n)
    {
    }

    PrimaryExpression(const std::string string)
      : type(Type::STRING)
      , string(string)
    {
    }

    PrimaryExpression(const Expression *expression)
      : type(Type::EXPRESSION)
      , expression(expression)
    {
    }

    virtual void travers(Visitor &visitor) const
    {
      if (!visitor.accept(*this))
      {
        visitor.accept(Visitor::Phases::PRE, *this);
        visitor.accept(Visitor::Phases::POST, *this);
      }
    }

    std::string toString() const override
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__);
      std::stringstream buffer;

      switch (type)
      {
        case Type::IDENTIFIER:
          buffer << identifier;
          break;
        case Type::INTEGER:
          buffer << n;
          break;
        case Type::STRING:
          buffer << string;
          break;
        case Type::EXPRESSION:
          buffer << '(' << expression->toString() << ')';
          break;
      }

      return buffer.str();
    }
};

class PostfixExpression : public Expression
{
  public:
    enum class Type
    {
      SUBSCRIPT,
      FUNCTION_CALL,
      MEMBER
    };

    Type                   type;
    Expression             *a,*b;
    ArgumentExpressionList argumentExpressionList;
    Identifier             identifier;

    PostfixExpression(Type type, Expression *a, Expression *b)
      : type(type)
      , a(a)
      , b(b)
    {
    }

    PostfixExpression(Type type, Expression *a, const ArgumentExpressionList &argumentList)
      : type(type)
      , a(a)
      , b(nullptr)
      , argumentExpressionList(argumentExpressionList)
    {
    }

    PostfixExpression(Type type, Expression *a)
      : type(type)
      , a(a)
      , b(nullptr)
      , argumentExpressionList(nullptr)
    {
    }

    PostfixExpression(Type type, Expression *a, const Identifier &identifier)
      : type(type)
      , a(a)
      , b(nullptr)
      , identifier(identifier)
      , argumentExpressionList(nullptr)
    {
    }

    void travers(Visitor &visitor) const override
    {
      if (!visitor.accept(*this))
      {
// TODO: type
        a->travers(visitor);
        b->travers(visitor);
      }
    }

    std::string toString() const override
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__);
      return "postfix";
    }
};

class UnaryExpression : public Expression
{
  public:
    enum class Operator
    {
      ADDRESS,
      DEREFERENCE,
      PLUS,
      MINUS,
      NOT,
      LOGICAL_NOT,

      INCREMENT,
      DECREMENT,
      SIZEOF
    };

    Operator         operator_;
    const Expression *expression;

    UnaryExpression(Operator operator_, const Expression *expression)
      : operator_(operator_)
      , expression(expression)
    {
    }

    void travers(Visitor &visitor) const override
    {
      visitor.accept(*this);
    }

    std::string toString() const override
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__);
      return "unary";
    }
};

class CastExpression : public Expression
{
  public:
    const Expression *expression;

    CastExpression(const Expression *expression)
      : expression(expression)
    {
    }

    void travers(Visitor &visitor) const override
    {
      if (!visitor.accept(*this))
      {
        expression->travers(visitor);
      }
    }

    std::string toString() const override
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__);
      return "cast";
    }
};

class MultiplicativeExpression : public Expression
{
  public:
    enum class Type
    {
      MULTIPLY,
      DIVIDE,
      MODULO
    };

    Type             type;
    const Expression *a,*b;

    MultiplicativeExpression(Type type, const Expression *a, const Expression *b)
      : type(type)
      , a(a)
      , b(b)
    {
    }

    void travers(Visitor &visitor) const override
    {
      if (!visitor.accept(*this))
      {
        a->travers(visitor);
        b->travers(visitor);
      }
    }

    std::string toString() const override
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__);
      return "mul";
    }
};

class AdditiveExpression : public Expression
{
  public:
    enum class Type
    {
      ADD,
      SUBTRACT
    };

    Type             type;
    const Expression *a,*b;

    AdditiveExpression(Type type, const Expression *a, const Expression *b)
      : type(type)
      , a(a)
      , b(b)
    {
    }

    void travers(Visitor &visitor) const override
    {
      if (!visitor.accept(*this))
      {
        a->travers(visitor);
        b->travers(visitor);
      }
    }

    std::string toString() const override
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__);
      return "add";
    }
};

class ShiftExpression : public Expression
{
  public:
    enum class Type
    {
      LEFT,
      RIGHT
    };

    Type       type;
    Expression *a,*b;

    ShiftExpression(Type type, Expression *a, Expression *b)
      : a(a)
      , b(b)
    {
    }

    void travers(Visitor &visitor) const override
    {
      if (!visitor.accept(*this))
      {
        a->travers(visitor);
        b->travers(visitor);
      }
    }

    std::string toString() const override
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__);
      return "shift";
    }
};

class RelationalExpression : public Expression
{
  public:
    enum class Type
    {
      NONE,
      LOWER,
      LOWER_EQUALS,
      GREATER,
      GREATER_EQUALS
    };

    Type             type;
    const Expression *a,*b;

    RelationalExpression(Type type, const Expression *a, const Expression *b)
      : type(type)
      , a(a)
      , b(b)
    {
    }

    void travers(Visitor &visitor) const override
    {
      if (!visitor.accept(*this))
      {
        a->travers(visitor);
        b->travers(visitor);
      }
    }

    std::string toString() const override
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__);
      return "eq";
    }
};

class EqualityExpression : public Expression
{
  public:
    enum class Type
    {
      EQUALS,
      NOT_EQUALS
    };

    Type             type;
    const Expression *a,*b;

    EqualityExpression(Type type, const Expression *a, const Expression *b)
      : type(type)
      , a(a)
      , b(b)
    {
    }

    void travers(Visitor &visitor) const override
    {
      if (!visitor.accept(*this))
      {
        a->travers(visitor);
        b->travers(visitor);
      }
    }

    std::string toString() const override
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__);
      return "eq";
    }
};

class AndExpression : public Expression
{
  public:
    const Expression *a,*b;

    AndExpression(const Expression *a, const Expression *b)
      : a(a)
      , b(b)
    {
    }

    void travers(Visitor &visitor) const override
    {
      if (!visitor.accept(*this))
      {
        a->travers(visitor);
        b->travers(visitor);
      }
    }

    std::string toString() const override
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__);
      return "and";
    }
};

class ExclusiveOrExpression : public Expression
{
  public:
    const Expression *a,*b;

    ExclusiveOrExpression(const Expression *a, const Expression *b)
      : a(a)
      , b(b)
    {
    }

    void travers(Visitor &visitor) const override
    {
      if (!visitor.accept(*this))
      {
        a->travers(visitor);
        b->travers(visitor);
      }
    }

    std::string toString() const override
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__);
      return "xor";
    }
};

class InclusiveOrExpression : public Expression
{
  public:
    const Expression *a,*b;

    InclusiveOrExpression(const Expression *a, const Expression *b)
      : a(a)
      , b(b)
    {
    }

    void travers(Visitor &visitor) const override
    {
      if (!visitor.accept(*this))
      {
        a->travers(visitor);
        b->travers(visitor);
      }
    }

    std::string toString() const override
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__);
      return "or";
    }
};

class LogicalAndExpression : public Expression
{
  public:
    const Expression *a,*b;

    LogicalAndExpression(const Expression *a, const Expression *b)
      : a(a)
      , b(b)
    {
    }

    void travers(Visitor &visitor) const override
    {
      if (!visitor.accept(*this))
      {
        a->travers(visitor);
        b->travers(visitor);
      }
    }

    std::string toString() const override
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__);
      return "log and";
    }
};

class LogicalOrExpression : public Expression
{
  public:
    const Expression *a,*b;

    LogicalOrExpression(const Expression *a, const Expression *b)
      : a(a)
      , b(b)
    {
    }

    void travers(Visitor &visitor) const override
    {
      if (!visitor.accept(*this))
      {
        a->travers(visitor);
        b->travers(visitor);
      }
    }

    std::string toString() const override
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__);
      return "log or";
    }
};

class ConditionalExpression : public Expression
{
  public:
    const Expression *condition,*a,*b;

    ConditionalExpression(const Expression *condition, const Expression *a, const Expression *b)
      : condition(condition)
      , a(a)
      , b(b)
    {
    }

    void travers(Visitor &visitor) const override
    {
      if (!visitor.accept(*this))
      {
        condition->travers(visitor);
        a->travers(visitor);
        b->travers(visitor);
      }
    }

    std::string toString() const override
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__);
      return "cnd";
    }
};

class AssignmentExpression : public Expression
{
  public:
    enum class Operator
    {
      ASSIGN,
      MULTIPLY_ASSIGN,
      DIVIDE_ASSIGN,
      MODULO_ASSIGN,
      ADD_ASSIGN,
      SUB_ASSIGN,
      SHIFT_LEFT_ASSIGN,
      SHIFT_RIGHT_ASSIGN,
      AND_ASSIGN,
      XOR_ASSIGN,
      OR_ASSIGN
    };

    Operator         operator_;
    const Expression *a,*b;

    AssignmentExpression(Operator operator_, const Expression *a, const Expression *b)
      : operator_(operator_)
      , a(a)
      , b(b)
    {
    }

    AssignmentExpression()
    {
    }

    ~AssignmentExpression() override
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }

    void travers(Visitor &visitor) const override
    {
      if (!visitor.accept(*this))
      {
        visitor.accept(Visitor::Phases::PRE, *this);
        visitor.accept(Visitor::Phases::POST, *this);
      }
    }

    std::string toString() const override
    {
      std::stringstream buffer;
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__);

//      buffer << unaryExpression->toString() << " = " << assignmentExpression->toString();
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__);

      return buffer.str();
    }
};

class StorageClassSpecifier : public VisitorInterface
{
  public:
    enum class Type
    {
      TYPEDEF,
      EXTERN,
      STATIC,
      AUTO,
      REGISTER
    };

    Type type;

    StorageClassSpecifier(Type type)
      : type(type)
    {
    }

    void travers(Visitor &visitor) const override
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    };

#if 0
    void accept(Visitor &visitor) const override
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__);
//      visitor.accept(*this);
    }
#endif
};

class DeclarationSpecifier : public VisitorInterface
{
  public:
#if 0
    DeclarationSpecifier()
    {
    }
#endif

    virtual void travers(Visitor &visitor) const override = 0;
/*
    void accept(Visitor &visitor) const override
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__);
      visitor.accept(*this);
    }
*/
};

class TypeQualifier : public DeclarationSpecifier
{
  public:
    enum class Type
    {
      CONST,
      VOLATILE,
      STRUCT
    };

    Type type;

    TypeQualifier(Type type)
      : type(type)
    {
    }

    TypeQualifier()
    {
    }

    void travers(Visitor &visitor) const override
    {
      visitor.accept(*this);
    }
};

class TypeQualifierList : public std::vector<TypeQualifier*>
{
  public:
    TypeQualifierList(TypeQualifier *typeQualifier)
      : std::vector<TypeQualifier*>{typeQualifier}
    {
    }

    TypeQualifierList()
    {
    }

    void add(TypeQualifier *typeQualifier)
    {
      push_back(typeQualifier);
    }
};

class TypeSpecifier : public DeclarationSpecifier
{
  public:
    enum class Type
    {
      VOID,
      CHAR,
      SHORT,
      INT,
      LONG,
      FLOAT,
      DOUBLE,
      SIGNED,
      UNSIGNED,
      IDENTIFIER
    };

    Type             type;
    const Identifier identifier;

    TypeSpecifier(Type type)
      : type(type)
    {
    }

    TypeSpecifier(const Identifier &identifier)
      : type(Type::IDENTIFIER)
      , identifier(identifier)
    {
    }

    TypeSpecifier()
    {
    }

    void travers(Visitor &visitor) const override
    {
      visitor.accept(*this);
    }
};

// TODO: std::list?
class DeclarationSpecifiers : public std::vector<DeclarationSpecifier*>, VisitorInterface
{
  public:
    DeclarationSpecifiers(StorageClassSpecifier *storageClassSpecifier)
// TODO: storageClassSpecifier
      : std::vector<DeclarationSpecifier*>{}
    {
    }

    DeclarationSpecifiers(DeclarationSpecifier *declarationSpecifier)
      : std::vector<DeclarationSpecifier*>{declarationSpecifier}
    {
    }

    DeclarationSpecifiers()
    {
    }

    void add(StorageClassSpecifier *storageClassSpecifier)
    {
// TODO:
//      push_back(declarationSpecifier);
    }

    void prepend(StorageClassSpecifier *storageClassSpecifier)
    {
// TODO:
//      insert(begin(), declarationSpecifier);
    }

    void add(DeclarationSpecifier *declarationSpecifier)
    {
      push_back(declarationSpecifier);
    }

    void prepend(DeclarationSpecifier *declarationSpecifier)
    {
      insert(begin(), declarationSpecifier);
    }

    void travers(Visitor &visitor) const
    {
      if (!visitor.accept(*this))
      {
        for (const DeclarationSpecifier *declarationSpecifier : *this)
        {
          declarationSpecifier->travers(visitor);
        }
      }
    }
};

#if 0
// TODO: std::list?
class StorageClassSpecifiers : public std::vector<StorageClassSpecifier*>, VisitorInterface
{
  public:
    StorageClassSpecifiers(StorageClassSpecifier *storageClassSpecifier)
      : std::vector<StorageClassSpecifier*>{storageClassSpecifier}
    {
    }

    StorageClassSpecifiers()
    {
    }

    void add(StorageClassSpecifier *storageClassSpecifier)
    {
      push_back(storageClassSpecifier);
    }

    void prepend(StorageClassSpecifier *storageClassSpecifier)
    {
      insert(begin(), storageClassSpecifier);
    }

    void travers(Visitor &visitor) const
    {
      if (!visitor.accept(*this))
      {
        for (const StorageClassSpecifier *storageClassSpecifier : *this)
        {
          storageClassSpecifier->travers(visitor);
        }
      }
    }
};
#endif

class DirectDeclarator : public VisitorInterface
{
  public:
    Identifier identifier;

    DirectDeclarator(const Identifier &identifier)
      : identifier(identifier)
    {
    }

    DirectDeclarator()
    {
    }

    void travers(Visitor &visitor) const
    {
      visitor.accept(*this);
    }
};

class Declarator : public VisitorInterface
{
  public:
// TODO: pointer
//    Identifier identifier;
    DirectDeclarator directDeclarator;

#if 0
    Declarator(const Identifier &identifier)
      : identifier(identifier)
    {
    }
#endif

    Declarator(const DirectDeclarator &directDeclarator)
      : directDeclarator(directDeclarator)
    {
    }

    Declarator()
    {
    }

    void travers(Visitor &visitor) const
    {
      if (!visitor.accept(*this))
      {
// TODO: pointer
        directDeclarator.travers(visitor);
      }
    }
};

class Initializer : public VisitorInterface
{
  public:
    const Expression *expression;

    Initializer(const Expression *expression)
      : expression(expression)
    {
    }

    Initializer(const Initializer &otherInitializer)
      : expression(otherInitializer.expression)
    {
    }

    Initializer()
      : expression(nullptr)
    {
    }

    void travers(Visitor &visitor) const
    {
      if (!visitor.accept(*this))
      {
        expression->travers(visitor);
      }
    }
};

class InitDeclarator : public VisitorInterface
{
  public:
    const DeclarationSpecifiers declarationSpecifiers;
    const Declarator            declarator;
    const Initializer           *initializer;

    InitDeclarator(const DeclarationSpecifiers &declarationSpecifiers, const Declarator &declarator, const Initializer &initializer)
      : declarationSpecifiers(declarationSpecifiers)
      , declarator(declarator)
      , initializer(new Initializer(initializer))
    {
    }

    InitDeclarator(const DeclarationSpecifiers &declarationSpecifiers, const Declarator &declarator)
      : declarationSpecifiers(declarationSpecifiers)
      , declarator(declarator)
      , initializer(nullptr)
    {
    }

    InitDeclarator()
    {
    }

    void travers(Visitor &visitor) const
    {
      if (!visitor.accept(*this))
      {
        declarationSpecifiers.travers(visitor);
        declarator.travers(visitor);
        if (initializer != nullptr)
        {
          initializer->travers(visitor);
        }
      }
    }
};

class InitDeclaratorList : public std::vector<InitDeclarator*>, private VisitorInterface
{
  public:
    InitDeclaratorList(InitDeclarator *initDeclarator)
    {
      push_back(initDeclarator);
    }

    InitDeclaratorList()
    {
    }

    virtual ~InitDeclaratorList()
    {
      // TODO: free
    }

    void add(InitDeclarator *initDeclarator)
    {
      push_back(initDeclarator);
    }

    void travers(Visitor &visitor) const override
    {
      for (const InitDeclarator *initDeclarator : *this)
      {
        initDeclarator->travers(visitor);
      }
    }
};

class Declaration : public VisitorInterface
{
  public:
    DeclarationSpecifiers declarationSpecifiers;
    InitDeclaratorList    initDeclaratorList;

    Declaration(const DeclarationSpecifiers &declarationSpecifiers, const InitDeclaratorList &initDeclaratorList)
      : declarationSpecifiers(declarationSpecifiers)
      , initDeclaratorList(initDeclaratorList)
    {
    }

    Declaration(const DeclarationSpecifiers &declarationSpecifiers)
      : declarationSpecifiers(declarationSpecifiers)
    {
    }

    Declaration(const InitDeclaratorList &initDeclaratorList)
      : initDeclaratorList(initDeclaratorList)
    {
    }

    void travers(Visitor &visitor) const
    {
      if (!visitor.accept(*this))
      {
        declarationSpecifiers.travers(visitor);
        initDeclaratorList.travers(visitor);
      }
    }

    std::string toString() const
    {
// TODO:
      return "Declaration";
    }
};

class Statement : public VisitorInterface
{
  public:
    virtual std::string toString() const = 0;

    friend std::ostream& operator<<(std::ostream& outputStream, const Statement *statement)
    {
      outputStream << statement->toString();

      return outputStream;
    }
};

class DeclarationStatement : public VisitorInterface
{
  public:
    enum class Type
    {
      DECLARATION,
      STATEMENT
    };

    Type              type;
    const Declaration *declaration;
    const Statement   *statement;

    DeclarationStatement(const Declaration *declaration)
      : type(Type::DECLARATION)
      , declaration(declaration)
    {
    }

    DeclarationStatement(const Statement *statement)
      : type(Type::STATEMENT)
      , statement(statement)
    {
    }

    void travers(Visitor &visitor) const
    {
      switch (type)
      {
        case DeclarationStatement::Type::DECLARATION: declaration->travers(visitor); break;
        case DeclarationStatement::Type::STATEMENT:   statement->travers(visitor); break;
      }
    }

    friend std::ostream& operator<<(std::ostream& outputStream, const DeclarationStatement &declarationStatement)
    {
      switch (declarationStatement.type)
      {
        case DeclarationStatement::Type::DECLARATION: outputStream << declarationStatement.declaration->toString(); break;
        case DeclarationStatement::Type::STATEMENT:   outputStream << declarationStatement.statement->toString(); break;
      }

      return outputStream;
    }
};

class DeclarationStatementList : public std::vector<DeclarationStatement>
{
  public:
    DeclarationStatementList(const Declaration *declaration)
    {
      push_back(DeclarationStatement(declaration));
    }

    DeclarationStatementList(const Statement *statement)
    {
      push_back(DeclarationStatement(statement));
    }

    DeclarationStatementList()
    {
    }

    void add(const Declaration *declaration)
    {
      push_back(DeclarationStatement(declaration));
    }

    void add(const Statement *statement)
    {
      push_back(DeclarationStatement(statement));
    }

    void travers(Visitor &visitor) const
    {
      for (const DeclarationStatement &declarationStatement : *this)
      {
        declarationStatement.travers(visitor);
      }
    }
};

class CompoundStatement : public Statement
{
  public:
    DeclarationStatementList declarationStatementList;

    CompoundStatement(const DeclarationStatementList &declarationStatementList)
      : declarationStatementList(declarationStatementList)
    {
    }

    CompoundStatement()
    {
    }

    void travers(Visitor &visitor) const override
    {
      if (!visitor.accept(*this))
      {
        declarationStatementList.travers(visitor);
      }
    }

    std::string toString() const override
    {
      std::stringstream buffer;

      buffer << " {";
      for (const DeclarationStatement declarationStatement : declarationStatementList)
      {
        buffer << declarationStatement;
      }
      buffer << "}";

      return buffer.str();
    }
};

#if 0
class VariableDeclarationStatement : public Statement
{
  public:
    TypeDeclaration typeDeclaration;
    Identifier      name;
    Expression      init;

    VariableDeclarationStatement(const TypeDeclaration &typeDeclaration, const Identifier &name, const Expression &init)
      : typeDeclaration(typeDeclaration)
      , name(name)
      , init(init)
    {
    }

    VariableDeclarationStatement(const TypeDeclaration &typeDeclaration, const Identifier &name)
      : typeDeclaration(typeDeclaration)
      , name(name)
    {
    }

    virtual void travers(Visitor &visitor) const
    {
      if (!visitor.accept(*this))
      {
        visitor.accept(Visitor::Phases::PRE, *this);
        visitor.accept(Visitor::Phases::POST, *this);
      }
    }

    std::string toString() const override
    {
      std::stringstream buffer;

      buffer << typeDeclaration.toString() << name << ";";

      return buffer.str();
    }
};
#endif

class IfStatement : public Statement
{
  public:
    const Expression *condition;
    const Statement  *ifStatement;
    const Statement  *elseStatement;

    IfStatement(const Expression *condition, const Statement *ifStatement, const Statement *elseStatement)
      : condition(condition)
      , ifStatement(ifStatement)
      , elseStatement(elseStatement)
    {
    }

    IfStatement(const Expression *condition, const Statement *ifStatement)
      : condition(condition)
      , ifStatement(ifStatement)
      , elseStatement(nullptr)
    {
    }

    virtual void travers(Visitor &visitor) const
    {
      if (!visitor.accept(*this))
      {
        condition->travers(visitor);
        ifStatement->travers(visitor);
        if (elseStatement != nullptr) elseStatement->travers(visitor);
      }
    }

    std::string toString() const override
    {
      std::stringstream buffer;

      buffer << "if" << " (" << condition->toString() << ")";
      buffer << ifStatement;
      if (elseStatement)
      {
        buffer << "else" << elseStatement;
      }

      return buffer.str();
    }
};

class ForStatement : public Statement
{
  public:
    const Expression *init;
    const Expression *condition;
    const Expression *increment;
    const Statement  *statement;

    ForStatement(const Expression *init, const Expression *condition, const Expression *increment, const Statement *statement)
      : init(init)
      , condition(condition)
      , increment(increment)
      , statement(statement)
    {
    }

    virtual void travers(Visitor &visitor) const
    {
      if (!visitor.accept(*this))
      {
        if (init != nullptr) init->travers(visitor);
        if (condition != nullptr) condition->travers(visitor);
        if (increment != nullptr) increment->travers(visitor);
        statement->travers(visitor);
      }
    }

    std::string toString() const override
    {
      std::stringstream buffer;


      buffer << "for" << " (" << init->toString() << ";" << condition->toString() << ";" << increment->toString() << ")";
      buffer << "{";
      buffer << statement;
      buffer << "}";

      return buffer.str();
    }
};

class WhileStatement : public Statement
{
  public:
    const Expression *condition;
    const Statement  *statement;

    WhileStatement(const Expression *condition, const Statement *statement)
      : condition(condition)
      , statement(statement)
    {
    }

    virtual void travers(Visitor &visitor) const
    {
      if (!visitor.accept(*this))
      {
        condition->travers(visitor);
        statement->travers(visitor);
      }
    }

    std::string toString() const override
    {
      std::stringstream buffer;

      buffer << "while" << " (" << condition->toString() << ")";
      buffer << statement;

      return buffer.str();
    }
};

class DoStatement : public Statement
{
  public:
    const Statement  *statement;
    const Expression *condition;

    DoStatement(const Statement *statement, const Expression *condition)
      : statement(statement)
      , condition(condition)
    {
    }

    virtual void travers(Visitor &visitor) const
    {
      if (!visitor.accept(*this))
      {
        statement->travers(visitor);
        condition->travers(visitor);
      }
    }

    std::string toString() const override
    {
      std::stringstream buffer;

      buffer << "do";
      buffer << statement;
      buffer << "while" << " (" << condition->toString() << ")";

      return buffer.str();
    }
};

class AssignmentStatement : public Statement
{
  public:
    const Expression *left;
    const Expression *right;

    AssignmentStatement(const Expression *left, const Expression *right)
      : left(left)
      , right(right)
    {
    }

    virtual void travers(Visitor &visitor) const
    {
      if (!visitor.accept(*this))
      {
        left->travers(visitor);
        right->travers(visitor);
      }
    }

    std::string toString() const override
    {
      std::stringstream buffer;

      buffer << left->toString() << " = " << right->toString() << ";";

      return buffer.str();
    }

};

class ExpressionStatement : public Statement
{
  public:
    Expression *expression;

    ExpressionStatement(Expression *expression)
      : expression(expression)
    {
    }

    ExpressionStatement()
      : expression(nullptr)
    {
    }

    void travers(Visitor &visitor) const override
    {
      if (!visitor.accept(*this))
      {
        visitor.accept(Visitor::Phases::PRE, *this);
        expression->travers(visitor);
        visitor.accept(Visitor::Phases::POST, *this);
      }
    }

    std::string toString() const
    {
      std::stringstream buffer;

      buffer << expression->toString()  << ";";

      return buffer.str();
    }
};

class JumpStatement : public Statement
{
  public:
    enum class Type
    {
      CONTINUE,
      BREAK,
      RETURN
    };

    Type       type;
    Expression *expression;

    JumpStatement(Type type, Expression *expression)
      : type(type)
      , expression(expression)
    {
    }

    JumpStatement(Type type)
      : type(type)
      , expression(nullptr)
    {
    }

    void travers(Visitor &visitor) const override
    {
      if (!visitor.accept(*this))
      {
        visitor.accept(Visitor::Phases::PRE, *this);
        if (expression != nullptr)
        {
          expression->travers(visitor);
        }
        visitor.accept(Visitor::Phases::POST, *this);
      }
    }

    std::string toString() const
    {
      std::stringstream buffer;

      buffer << expression->toString()  << ";";

      return buffer.str();
    }
};

class NewStateStatement : public Statement
{
  public:
    Identifier  name;
    std::string label;
    std::string color;
    Number      lineWidth;

    NewStateStatement(const Identifier &name, const std::string &label, const std::string &color, const Number &lineWidth)
      : name(name)
      , label(label)
      , color(color)
      , lineWidth(lineWidth)
    {
    }

    NewStateStatement(const Identifier &name, const std::string &label, const std::string &color)
      : name(name)
      , label(label)
      , color(color)
      , lineWidth()
    {
    }

    NewStateStatement(const Identifier &name, const std::string &label)
      : name(name)
      , label(label)
      , color()
      , lineWidth()
    {
    }

    NewStateStatement(const Identifier &name)
      : name(name)
      , label()
      , color()
      , lineWidth()
    {
    }

    virtual void travers(Visitor &visitor) const
    {
      if (!visitor.accept(*this))
      {
        visitor.accept(Visitor::Phases::PRE, *this);
        visitor.accept(Visitor::Phases::POST, *this);
      }
    }

    std::string toString() const override
    {
      std::stringstream buffer;

      buffer << "->" << name;
      if (!label.empty() || !color.empty() || (lineWidth() > 0))
      {
        buffer << "{";
        if (!label.empty())
        {
          buffer << "\"" << label << "\"";
        }
        if (!color.empty())
        {
          if (!label.empty()) buffer << ", ";
          buffer << color;
        }
        if (lineWidth() > 0)
        {
          if (!label.empty() || !color.empty()) buffer << ", ";
          buffer << lineWidth.toString();
        }
        buffer << "}";
      }

      return buffer.str();
    }
};

class State
{
  public:
    const Identifier        name;
    const CompoundStatement *compoundStatement;

    State(const Identifier &name, const CompoundStatement *compoundStatement)
      : name(name)
      , compoundStatement(compoundStatement)
    {
    }

    State(CompoundStatement *compoundStatement)
      : compoundStatement(compoundStatement)
    {
    }

    State()
    {
    }

    void travers(Visitor &visitor) const
    {
      if (!visitor.accept(*this))
      {
        visitor.accept(Visitor::Phases::PRE, *this);
        compoundStatement->travers(visitor);
        visitor.accept(Visitor::Phases::POST, *this);
      }
    }
};

class StateList : public std::vector<State>
{
  public:
    void add(const State &state)
    {
      push_back(state);
    }

    void travers(Visitor &visitor) const
    {
      visitor.accept(Visitor::Phases::PRE, *this);
      for (const FSM::State state : *this)
      {
        state.travers(visitor);
      }
      visitor.accept(Visitor::Phases::POST, *this);
    }
};

class AST
{
  public:
    typedef std::unordered_multimap<Identifier,
                                    NewStateStatement&,
                                    std::hash<std::string>
                                   > StateTransitionMap;

    AST();

    /**
     * clear AST
     */
    void clear();

    std::string getFSMName() const
    {
      return fsmName;
    }

    Identifier getStartState() const
    {
      if      (!startState.empty())
      {
        return startState;
      }
      else if (stateList.size() > 0)
      {
        return stateList[0].name;
      }
      else
      {
        return Identifier();
      }
    }

    const StateList& getStateList() const
    {
      return stateList;
    }

    void doStateTransitions(const Identifier &name,
                            std::function<void(const NewStateStatement &newStateStatement)> handler
                           ) const
    {
      std::pair<StateTransitionMap::const_iterator, StateTransitionMap::const_iterator> iterators = stateTransitions.equal_range(name);
      while (iterators.first != iterators.second)
      {
        handler(iterators.first->second);
        iterators.first++;
      }
    }

    void traverse(Visitor &visitor);

    /**
     * print AST
     */
    void print() const;

  private:
    // allow access to private methods
    friend class Parser;
    friend class Visitor;

    void setFSMName(const std::string &fsmName)
    {
      this->fsmName = fsmName;
    }

    void setStartState(const Identifier &startState)
    {
      this->startState = startState;
    }

    void addState(State state);

    void addStateTransition(const Identifier &name, NewStateStatement &newStateStatement)
    {
      std::pair<StateTransitionMap::const_iterator, StateTransitionMap::const_iterator> iterators = stateTransitions.equal_range(name);
      while ((iterators.first != iterators.second) && (iterators.first->second.name != newStateStatement.name))
      {
        iterators.first++;
      }
      if (iterators.first == iterators.second)
      {
        stateTransitions.insert(StateTransitionMap::value_type(name,newStateStatement));
      }
    }

  private:
    std::string        fsmName;
    Identifier         startState;
    StateList          stateList;
    StateTransitionMap stateTransitions;
};

} // namespace FSM

template <>
struct std::hash<FSM::Identifier>
{
  public:
    std::size_t operator()(const FSM::Identifier &idendifier) const
    {
      return 0;
    }
};


/***************************** Variables *******************************/

/***************************** Forwards ********************************/

/***************************** Functions *******************************/

#endif // AST_H

/* end of file */
