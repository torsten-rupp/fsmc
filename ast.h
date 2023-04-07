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
    virtual void traverse(Visitor &visitor) const = 0;
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

#if 0
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
#endif

class Expression : public VisitorInterface
{
  public:
    Expression     *nodes[2];
    Identifier     identifier;
    std::string    string;

    virtual ~Expression()
    {
    }

    virtual void traverse(Visitor &visitor) const
    {
      if (!visitor.accept(*this))
      {
        visitor.accept(Visitor::Phases::PRE, *this);
        visitor.accept(Visitor::Phases::POST, *this);
      }
    }

    virtual std::string toString() const = 0;
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
      , expression(nullptr)
    {
    }

    PrimaryExpression(int64_t n)
      : type(Type::INTEGER)
      , n(n)
      , expression(nullptr)
    {
    }

    PrimaryExpression(const std::string string)
      : type(Type::STRING)
      , string(string)
      , expression(nullptr)
    {
    }

    PrimaryExpression(const Expression *expression)
      : type(Type::EXPRESSION)
      , expression(expression)
    {
    }

    ~PrimaryExpression() override
    {
      delete(expression);
    }

    virtual void traverse(Visitor &visitor) const
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

class ArgumentExpressionList : public std::vector<Expression*>, private VisitorInterface
{
  public:
    ArgumentExpressionList(Expression *expression)
      : std::vector<Expression*>{expression}
    {
    }

    virtual ~ArgumentExpressionList()
    {
      for (const Expression *expression : *this)
      {
        delete(expression);
      }
    }

    void add(Expression *expression)
    {
      push_back(expression);
    }

    void traverse(Visitor &visitor) const override
    {
      for (const Expression *expression : *this)
      {
        expression->traverse(visitor);
      }
    }
};

class PostfixExpression : public Expression
{
  public:
    enum class Type
    {
      SUBSCRIPT,
      FUNCTION_CALL,
      MEMBER,
      POINTER,
      INCREMENT,
      DECREMENT
    };

    Type type;
    union
    {
      struct
      {
        Expression *array;
        Expression *index;
      };
      struct
      {
        Expression             *call;
        ArgumentExpressionList *argumentExpressionList;
      };
      struct
      {
        Expression *structure;
        Identifier *identifier;
      };
      struct
      {
        Expression *expression;
      };
    };

    PostfixExpression(Type type, Expression *array, Expression *index)
      : type(Type::SUBSCRIPT)
      , array(array)
      , index(index)
    {
      assert(type == Type::SUBSCRIPT);
    }

    PostfixExpression(Type type, Expression *call, ArgumentExpressionList *argumentExpressionList)
      : type(type)
      , call(call)
      , argumentExpressionList(argumentExpressionList)
    {
      assert(type == Type::FUNCTION_CALL);
    }

    PostfixExpression(Type type, Expression *expression)
      : type(type)
    {
      assert(   (type == Type::FUNCTION_CALL)
             || (type == Type::POINTER)
             || (type == Type::INCREMENT)
             || (type == Type::DECREMENT)
            );

      switch (type)
      {
        case Type::FUNCTION_CALL:
          call = expression;
          argumentExpressionList = nullptr;
          break;
        case Type::POINTER:
        case Type::INCREMENT:
        case Type::DECREMENT:
          this->expression = expression;
          break;
      }
    }

    PostfixExpression(Type type, Expression *structure, const Identifier &identifier)
      : type(type)
      , structure(structure)
      , identifier(new Identifier(identifier))
    {
      assert(type == Type::MEMBER);
    }

    ~PostfixExpression() override
    {
      switch (type)
      {
        case Type::SUBSCRIPT:
          delete(index);
          delete(array);
          break;
        case Type::FUNCTION_CALL:
          delete(argumentExpressionList);
          delete(call);
          break;
        case Type::MEMBER:
          delete(identifier);
          delete(structure);
          break;
      }
    }

    void traverse(Visitor &visitor) const override
    {
      if (!visitor.accept(*this))
      {
        switch (type)
        {
          case Type::SUBSCRIPT:
            array->traverse(visitor);
            index->traverse(visitor);
            break;
          case Type::FUNCTION_CALL:
            call->traverse(visitor);
            argumentExpressionList->traverse(visitor);
            break;
          case Type::MEMBER:
            structure->traverse(visitor);
            break;
        }
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

    ~UnaryExpression() override
    {
      delete(expression);
    }

    void traverse(Visitor &visitor) const override
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

    void traverse(Visitor &visitor) const override
    {
      if (!visitor.accept(*this))
      {
        expression->traverse(visitor);
      }
    }

    ~CastExpression() override
    {
      delete(expression);
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

    ~MultiplicativeExpression() override
    {
      delete(b);
      delete(a);
    }

    void traverse(Visitor &visitor) const override
    {
      if (!visitor.accept(*this))
      {
        a->traverse(visitor);
        b->traverse(visitor);
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

    ~AdditiveExpression() override
    {
      delete(b);
      delete(a);
    }

    void traverse(Visitor &visitor) const override
    {
      if (!visitor.accept(*this))
      {
        a->traverse(visitor);
        b->traverse(visitor);
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
      : type(type)
      , a(a)
      , b(b)
    {
    }

    ~ShiftExpression() override
    {
      delete(b);
      delete(a);
    }

    void traverse(Visitor &visitor) const override
    {
      if (!visitor.accept(*this))
      {
        a->traverse(visitor);
        b->traverse(visitor);
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

    ~RelationalExpression() override
    {
      delete(b);
      delete(a);
    }

    void traverse(Visitor &visitor) const override
    {
      if (!visitor.accept(*this))
      {
        a->traverse(visitor);
        b->traverse(visitor);
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

    ~EqualityExpression() override
    {
      delete(b);
      delete(a);
    }

    void traverse(Visitor &visitor) const override
    {
      if (!visitor.accept(*this))
      {
        a->traverse(visitor);
        b->traverse(visitor);
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

    ~AndExpression() override
    {
      delete(b);
      delete(a);
    }

    void traverse(Visitor &visitor) const override
    {
      if (!visitor.accept(*this))
      {
        a->traverse(visitor);
        b->traverse(visitor);
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

    ~ExclusiveOrExpression() override
    {
      delete(b);
      delete(a);
    }

    void traverse(Visitor &visitor) const override
    {
      if (!visitor.accept(*this))
      {
        a->traverse(visitor);
        b->traverse(visitor);
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

    ~InclusiveOrExpression() override
    {
      delete(b);
      delete(a);
    }

    void traverse(Visitor &visitor) const override
    {
      if (!visitor.accept(*this))
      {
        a->traverse(visitor);
        b->traverse(visitor);
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

    ~LogicalAndExpression() override
    {
      delete(b);
      delete(a);
    }

    void traverse(Visitor &visitor) const override
    {
      if (!visitor.accept(*this))
      {
        a->traverse(visitor);
        b->traverse(visitor);
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

    ~LogicalOrExpression() override
    {
      delete(b);
      delete(a);
    }

    void traverse(Visitor &visitor) const override
    {
      if (!visitor.accept(*this))
      {
        a->traverse(visitor);
        b->traverse(visitor);
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

    ~ConditionalExpression() override
    {
      delete(b);
      delete(a);
      delete(condition);
    }

    void traverse(Visitor &visitor) const override
    {
      if (!visitor.accept(*this))
      {
        condition->traverse(visitor);
        a->traverse(visitor);
        b->traverse(visitor);
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
      delete(b);
      delete(a);
    }

    void traverse(Visitor &visitor) const override
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

class StorageClassDeclarationSpecifier : public VisitorInterface
{
  public:
};

class StorageClassSpecifier : public StorageClassDeclarationSpecifier
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

    void traverse(Visitor &visitor) const override
    {
      if (!visitor.accept(*this))
      {
        visitor.accept(Visitor::Phases::PRE, *this);
        visitor.accept(Visitor::Phases::POST, *this);
      }
    };
};

class DeclarationSpecifier : public StorageClassDeclarationSpecifier
{
  public:
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

    void traverse(Visitor &visitor) const override
    {
      visitor.accept(*this);
    }
};

class TypeQualifierList : public std::vector<TypeQualifier*>, private VisitorInterface
{
  public:
    TypeQualifierList(TypeQualifier *typeQualifier)
      : std::vector<TypeQualifier*>{typeQualifier}
    {
    }

    TypeQualifierList()
    {
    }

    virtual ~TypeQualifierList()
    {
      for (const TypeQualifier *typeQualifier : *this)
      {
        delete(typeQualifier);
      }
    }

    void add(TypeQualifier *typeQualifier)
    {
      push_back(typeQualifier);
    }

    void traverse(Visitor &visitor) const override
    {
      for (const TypeQualifier *typeQualifier : *this)
      {
        typeQualifier->traverse(visitor);
      }
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

    void traverse(Visitor &visitor) const override
    {
      visitor.accept(*this);
    }
};

// TODO: std::list?
class StorageClassDeclarationSpecifiers : public std::vector<StorageClassDeclarationSpecifier*>, VisitorInterface
{
  public:
    StorageClassDeclarationSpecifiers(StorageClassSpecifier *storageClassSpecifier)
// TODO: storageClassSpecifier
      : std::vector<StorageClassDeclarationSpecifier*>{storageClassSpecifier}
    {
    }

    StorageClassDeclarationSpecifiers(DeclarationSpecifier *declarationSpecifier)
      : std::vector<StorageClassDeclarationSpecifier*>{declarationSpecifier}
    {
    }

    virtual ~StorageClassDeclarationSpecifiers()
    {
      for (const StorageClassDeclarationSpecifier *storageClassDeclarationSpecifier : *this)
      {
        delete(storageClassDeclarationSpecifier);
      }
    }

    void add(StorageClassSpecifier *storageClassSpecifier)
    {
      push_back(storageClassSpecifier);
    }

    void prepend(StorageClassSpecifier *storageClassSpecifier)
    {
      insert(begin(), storageClassSpecifier);
    }

    void add(DeclarationSpecifier *declarationSpecifier)
    {
      push_back(declarationSpecifier);
    }

    void prepend(DeclarationSpecifier *declarationSpecifier)
    {
      insert(begin(), declarationSpecifier);
    }

    void traverse(Visitor &visitor) const
    {
      if (!visitor.accept(*this))
      {
        for (const StorageClassDeclarationSpecifier *storageClassDeclarationSpecifier : *this)
        {
          storageClassDeclarationSpecifier->traverse(visitor);
        }
      }
    }
};

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

    void traverse(Visitor &visitor) const
    {
      visitor.accept(*this);
    }
};

class Declarator : public VisitorInterface
{
  public:
    const DirectDeclarator *directDeclarator;

    Declarator(const DirectDeclarator *directDeclarator)
      : directDeclarator(directDeclarator)
    {
    }

    Declarator()
    {
    }

    virtual ~Declarator()
    {
      delete(directDeclarator);
    }

    void traverse(Visitor &visitor) const
    {
      if (!visitor.accept(*this))
      {
        directDeclarator->traverse(visitor);
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

    virtual ~Initializer()
    {
      delete(expression);
    }

    void traverse(Visitor &visitor) const
    {
      if (!visitor.accept(*this))
      {
        expression->traverse(visitor);
      }
    }
};

class InitDeclarator : public VisitorInterface
{
  public:
    const StorageClassDeclarationSpecifiers *storageClassDeclarationSpecifiers;
    const Declarator                        *declarator;
    const Initializer                       *initializer;

    InitDeclarator(const StorageClassDeclarationSpecifiers *storageClassDeclarationSpecifiers, const Declarator *declarator, const Initializer *initializer)
      : storageClassDeclarationSpecifiers(storageClassDeclarationSpecifiers)
      , declarator(declarator)
      , initializer(initializer)
    {
    }

    InitDeclarator(const StorageClassDeclarationSpecifiers *storageClassDeclarationSpecifiers, const Declarator *declarator)
      : storageClassDeclarationSpecifiers(storageClassDeclarationSpecifiers)
      , declarator(declarator)
      , initializer(nullptr)
    {
    }

    virtual ~InitDeclarator()
    {
      delete(initializer);
      delete(declarator);
      delete(storageClassDeclarationSpecifiers);
    }

    void traverse(Visitor &visitor) const
    {
      if (!visitor.accept(*this))
      {
        storageClassDeclarationSpecifiers->traverse(visitor);
        declarator->traverse(visitor);
        if (initializer != nullptr)
        {
          initializer->traverse(visitor);
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
      for (const InitDeclarator *initDeclarator : *this)
      {
        delete(initDeclarator);
      }
    }

    void add(InitDeclarator *initDeclarator)
    {
      push_back(initDeclarator);
    }

    void traverse(Visitor &visitor) const override
    {
      for (const InitDeclarator *initDeclarator : *this)
      {
        initDeclarator->traverse(visitor);
      }
    }
};

class DeclarationStatement : public VisitorInterface
{
  public:
    virtual ~DeclarationStatement()
    {
    }
};

class Declaration : public DeclarationStatement
{
  public:
    const StorageClassDeclarationSpecifiers *storageClassDeclarationSpecifiers;
    const InitDeclaratorList                *initDeclaratorList;

    Declaration(const StorageClassDeclarationSpecifiers *storageClassDeclarationSpecifiers, const InitDeclaratorList *initDeclaratorList)
      : storageClassDeclarationSpecifiers(storageClassDeclarationSpecifiers)
      , initDeclaratorList(initDeclaratorList)
    {
    }

    Declaration(const StorageClassDeclarationSpecifiers *storageClassDeclarationSpecifiers)
      : storageClassDeclarationSpecifiers(storageClassDeclarationSpecifiers)
      , initDeclaratorList(nullptr)
    {
    }

    Declaration(const InitDeclaratorList *initDeclaratorList)
      : storageClassDeclarationSpecifiers(nullptr)
      , initDeclaratorList(initDeclaratorList)
    {
    }

    ~Declaration() override
    {
      delete(initDeclaratorList);
      delete(storageClassDeclarationSpecifiers);
    }

    void traverse(Visitor &visitor) const
    {
      if (!visitor.accept(*this))
      {
        if (storageClassDeclarationSpecifiers != nullptr)
        {
          storageClassDeclarationSpecifiers->traverse(visitor);
        }
        if (initDeclaratorList != nullptr)
        {
          initDeclaratorList->traverse(visitor);
        }
      }
    }

    std::string toString() const
    {
// TODO:
      return "Declaration";
    }
};

class Statement : public DeclarationStatement
{
  public:
    virtual ~Statement() override
    {
    }

    virtual std::string toString() const = 0;

    friend std::ostream& operator<<(std::ostream& outputStream, const Statement *statement)
    {
      outputStream << statement->toString();

      return outputStream;
    }
};

class DeclarationStatementList : public std::vector<DeclarationStatement*>
{
  public:
    DeclarationStatementList(Declaration *declaration)
    {
      push_back(declaration);
    }

    DeclarationStatementList(Statement *statement)
    {
      push_back(statement);
    }

    virtual ~DeclarationStatementList()
    {
      for (const DeclarationStatement *declarationStatement : *this)
      {
        delete(declarationStatement);
      }
    }

    void add(Declaration *declaration)
    {
      push_back(declaration);
    }

    void add(Statement *statement)
    {
      push_back(statement);
    }

    void traverse(Visitor &visitor) const
    {
      for (const DeclarationStatement *declarationStatement : *this)
      {
        declarationStatement->traverse(visitor);
      }
    }
};

class CompoundStatement : public Statement
{
  public:
    const DeclarationStatementList *declarationStatementList;

    CompoundStatement(const DeclarationStatementList *declarationStatementList)
      : declarationStatementList(declarationStatementList)
    {
    }

    CompoundStatement()
      : declarationStatementList(nullptr)
    {
    }

    ~CompoundStatement() override
    {
      delete(declarationStatementList);
    }

    void traverse(Visitor &visitor) const override
    {
      if (!visitor.accept(*this))
      {
        if (declarationStatementList != nullptr)
        {
          declarationStatementList->traverse(visitor);
        }
      }
    }

    std::string toString() const override
    {
      std::stringstream buffer;

      buffer << " {";
      for (const DeclarationStatement *declarationStatement : *declarationStatementList)
      {
        buffer << declarationStatement;
      }
      buffer << "}";

      return buffer.str();
    }
};

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

    ~IfStatement() override
    {
      delete(elseStatement);
      delete(ifStatement);
      delete(condition);
    }

    virtual void traverse(Visitor &visitor) const
    {
      if (!visitor.accept(*this))
      {
        condition->traverse(visitor);
        ifStatement->traverse(visitor);
        if (elseStatement != nullptr) elseStatement->traverse(visitor);
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

    ~ForStatement() override
    {
      delete(statement);
      delete(increment);
      delete(condition);
      delete(init);
    }

    virtual void traverse(Visitor &visitor) const
    {
      if (!visitor.accept(*this))
      {
        if (init != nullptr) init->traverse(visitor);
        if (condition != nullptr) condition->traverse(visitor);
        if (increment != nullptr) increment->traverse(visitor);
        statement->traverse(visitor);
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

    ~WhileStatement() override
    {
      delete(statement);
      delete(condition);
    }

    virtual void traverse(Visitor &visitor) const
    {
      if (!visitor.accept(*this))
      {
        condition->traverse(visitor);
        statement->traverse(visitor);
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

    ~DoStatement() override
    {
      delete(statement);
      delete(condition);
    }

    virtual void traverse(Visitor &visitor) const
    {
      if (!visitor.accept(*this))
      {
        statement->traverse(visitor);
        condition->traverse(visitor);
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

    ~AssignmentStatement() override
    {
      delete(right);
      delete(left);
    }

    virtual void traverse(Visitor &visitor) const
    {
      if (!visitor.accept(*this))
      {
        left->traverse(visitor);
        right->traverse(visitor);
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

    ~ExpressionStatement() override
    {
      delete(expression);
    }

    void traverse(Visitor &visitor) const override
    {
      if (!visitor.accept(*this))
      {
        visitor.accept(Visitor::Phases::PRE, *this);
        expression->traverse(visitor);
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

    ~JumpStatement() override
    {
      delete(expression);
    }

    void traverse(Visitor &visitor) const override
    {
      if (!visitor.accept(*this))
      {
        visitor.accept(Visitor::Phases::PRE, *this);
        if (expression != nullptr)
        {
          expression->traverse(visitor);
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
    enum class Type
    {
      START,
      DEFAULT,
      PUSH,
      POP,
      CUSTOM
    };

    enum class PrefixOperator
    {
      NONE,
      PUSH
    };

    class Options
    {
      public:
        std::string label;
        std::string color;
        double      lineWidth;

      Options(const std::string label, const std::string color, double lineWidth)
        : label(label)
        , color(color)
        , lineWidth(lineWidth)
      {
      }

      Options(const std::string label, const std::string color)
        : label(label)
        , color(color)
        , lineWidth(0)
      {
      }

      Options(const std::string label)
        : label(label)
        , color()
        , lineWidth(0)
      {
      }

      Options()
        : label()
        , color()
        , lineWidth(0)
      {
      }
    };

    Type           type;
    Identifier     name;
    PrefixOperator prefixOperator;
    Options        options;

    NewStateStatement(const Identifier &name, PrefixOperator prefixOperator, const Options &options)
      : type(getType(name))
      , name(name)
      , prefixOperator(prefixOperator)
      , options(options)
    {
    }

    NewStateStatement(const Identifier &name, PrefixOperator prefixOperator)
      : type(getType(name))
      , name(name)
      , prefixOperator(prefixOperator)
      , options()
    {
    }

    NewStateStatement(const Identifier &name, const Options &options)
      : type(getType(name))
      , name(name)
      , prefixOperator(PrefixOperator::NONE)
      , options(options)
    {
    }

    NewStateStatement(const Identifier &name)
      : type(getType(name))
      , name(name)
      , prefixOperator(PrefixOperator::NONE)
      , options()
    {
    }

    virtual void traverse(Visitor &visitor) const
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

      buffer << "->";
      switch (prefixOperator)
      {
        case PrefixOperator::PUSH:
          buffer << "push,";
          break;
        default:
          break;
      }
      switch (type)
      {
        case Type::START:   buffer << "start"; break;
        case Type::DEFAULT: buffer << "default"; break;
        case Type::POP:     buffer << "pop"; break;
        case Type::CUSTOM:  buffer << name; break;
      }
      if (!options.label.empty() || !options.color.empty() || (options.lineWidth > 0))
      {
        buffer << "{";
        if (!options.label.empty())
        {
          buffer << "\"" << options.label << "\"";
        }
        if (!options.color.empty())
        {
          if (!options.label.empty()) buffer << ", ";
          buffer << options.color;
        }
        if (options.lineWidth > 0)
        {
          if (!options.label.empty() || !options.color.empty()) buffer << ", ";
          buffer << options.lineWidth;
        }
        buffer << "}";
      }

      return buffer.str();
    }

  private:
    static Type getType(const std::string name)
    {
      if      (name == "start"  ) return NewStateStatement::Type::START;
      else if (name == "default") return NewStateStatement::Type::DEFAULT;
      else if (name == "pop")     return NewStateStatement::Type::POP;
      else                        return NewStateStatement::Type::CUSTOM;
    }
};

class State
{
  public:
    enum class Type
    {
      START,
      DEFAULT,
      CUSTOM
    };

    const Type              type;
    const Identifier        name;
    const CompoundStatement *compoundStatement;

    State(Type type, const Identifier &name, const CompoundStatement *compoundStatement)
      : type(type)
      , name(name)
      , compoundStatement(compoundStatement)
    {
    }

    State(Type type, const CompoundStatement *compoundStatement)
      : type(type)
      , name()
      , compoundStatement(compoundStatement)
    {
    }

#if 0
    State()
    {
    }
#endif

    virtual ~State()
    {
      delete(compoundStatement);
    }

    void traverse(Visitor &visitor) const
    {
      if (!visitor.accept(*this))
      {
        visitor.accept(Visitor::Phases::PRE, *this);
        if (compoundStatement != nullptr)
        {
          compoundStatement->traverse(visitor);
        }
        visitor.accept(Visitor::Phases::POST, *this);
      }
    }
};

class StateList : public std::vector<State*>
{
  public:
    void add(State *state)
    {
      push_back(state);
    }

    virtual ~StateList()
    {
      for (const State *state : *this)
      {
        delete(state);
      }
    }

    void traverse(Visitor &visitor) const
    {
      visitor.accept(Visitor::Phases::PRE, *this);
      for (const FSM::State *state : *this)
      {
        state->traverse(visitor);
      }
      visitor.accept(Visitor::Phases::POST, *this);
    }
};

class AST
{
  public:
    typedef std::unordered_multimap<Identifier,
                                    const NewStateStatement*,
                                    std::hash<std::string>
                                   > StateTransitionMap;

    AST(uint stateStackSize, bool asserts);

    bool hasStateStack() const
    {
      return stateStackSize > 0;
    }

    uint getStateStackSize() const
    {
      return stateStackSize;
    }

    uint isAsserts() const
    {
      return asserts;
    }

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
      for (const State *state : stateList)
      {
        if (state->type == State::Type::START)
        {
          return state->name;
        }
      }

      return Identifier();
    }

    Identifier getDefaultState() const
    {
      for (const State *state : stateList)
      {
        if (state->type == State::Type::DEFAULT)
        {
          return state->name;
        }
      }

      return Identifier();
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
        handler(*iterators.first->second);
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

    void addState(State *state);

    void addStateTransition(const Identifier &name, const NewStateStatement *newStateStatement)
    {
      std::pair<StateTransitionMap::const_iterator, StateTransitionMap::const_iterator> iterators = stateTransitions.equal_range(name);
      while ((iterators.first != iterators.second) && (iterators.first->second->name != newStateStatement->name))
      {
        iterators.first++;
      }
      if (iterators.first == iterators.second)
      {
        stateTransitions.insert(StateTransitionMap::value_type(name,newStateStatement));
      }
    }

  private:
    uint               stateStackSize;
    bool               asserts;
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
