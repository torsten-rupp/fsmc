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

#include "location.h"
#include "visitor.h"

/****************** Conditional compilation switches *******************/

/***************************** Constants *******************************/

/***************************** Datatypes *******************************/

// parser forward reference
int yyparse();

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
      try
      {
        visitor.accept(*this);
      }
      catch (const Visitor::Exception &)
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
class SpecifierQualifierList : public std::vector<DeclarationSpecifier*>, VisitorInterface
{
  public:
    SpecifierQualifierList(TypeQualifier *typeQualifier)
      : std::vector<DeclarationSpecifier*>{typeQualifier}
    {
    }

    SpecifierQualifierList(TypeSpecifier *typeSpecifier)
      : std::vector<DeclarationSpecifier*>{typeSpecifier}
    {
    }

    virtual ~SpecifierQualifierList()
    {
      for (const DeclarationSpecifier *declarationSpecifier : *this)
      {
        delete(declarationSpecifier);
      }
    }

    void add(TypeQualifier *typeQualifier)
    {
      push_back(typeQualifier);
    }

    void prepend(TypeQualifier *typeQualifier)
    {
      insert(begin(), typeQualifier);
    }

    void add(TypeSpecifier *typeSpecifier)
    {
      push_back(typeSpecifier);
    }

    void prepend(TypeSpecifier *typeSpecifier)
    {
      insert(begin(), typeSpecifier);
    }

    void traverse(Visitor &visitor) const
    {
      try
      {
        visitor.accept(*this);
      }
      catch (const Visitor::Exception &)
      {
        visitor.accept(Visitor::Phases::PRE, *this);
        for (const DeclarationSpecifier *declarationSpecifier : *this)
        {
          declarationSpecifier->traverse(visitor);
        }
        visitor.accept(Visitor::Phases::POST, *this);
      }
    }
};

// TODO: std::list?
class StorageClassDeclarationSpecifiers : public std::vector<StorageClassDeclarationSpecifier*>, VisitorInterface
{
  public:
    StorageClassDeclarationSpecifiers(StorageClassSpecifier *storageClassSpecifier)
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
      try
      {
        visitor.accept(*this);
      }
      catch (const Visitor::Exception &)
      {
        visitor.accept(Visitor::Phases::PRE, *this);
        for (const StorageClassDeclarationSpecifier *storageClassDeclarationSpecifier : *this)
        {
          storageClassDeclarationSpecifier->traverse(visitor);
        }
        visitor.accept(Visitor::Phases::POST, *this);
      }
    }
};

class AbstractDeclarator : public VisitorInterface
{
  public:
    const Pointer *pointer;

    AbstractDeclarator(const Pointer *pointer)
      : pointer(pointer)
    {
    }

    AbstractDeclarator()
    {
    }

    void traverse(Visitor &visitor) const
    {
      visitor.accept(*this);
    }
};

class TypeName : public VisitorInterface
{
  public:
    SpecifierQualifierList *specifierQualifierList;
    AbstractDeclarator     *abstractDeclarator;

    TypeName(SpecifierQualifierList *specifierQualifierList, AbstractDeclarator *abstractDeclarator)
      : specifierQualifierList(specifierQualifierList)
      , abstractDeclarator(abstractDeclarator)
    {
    }

    TypeName(SpecifierQualifierList *specifierQualifierList)
      : specifierQualifierList(specifierQualifierList)
      , abstractDeclarator(nullptr)
    {
    }

    virtual ~TypeName()
    {
      delete(abstractDeclarator);
      delete(specifierQualifierList);
    }

    void traverse(Visitor &visitor) const override
    {
      try
      {
        visitor.accept(*this);
      }
      catch (const Visitor::Exception &)
      {
        visitor.accept(Visitor::Phases::PRE, *this);
        specifierQualifierList->traverse(visitor);
        if (abstractDeclarator != nullptr) abstractDeclarator->traverse(visitor);
        visitor.accept(Visitor::Phases::POST, *this);
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

class Pointer : public VisitorInterface
{
  public:
    Pointer()
    {
    }

    virtual ~Pointer()
    {
    }

    virtual void traverse(Visitor &visitor) const
    {
      try
      {
        visitor.accept(*this);
      }
      catch (const Visitor::Exception &)
      {
        visitor.accept(Visitor::Phases::PRE, *this);
        visitor.accept(Visitor::Phases::POST, *this);
      }
    }
};

class Declarator : public VisitorInterface
{
  public:
    const Pointer          *pointer;
    const DirectDeclarator *directDeclarator;

    Declarator(const Pointer *pointer, const DirectDeclarator *directDeclarator)
      : pointer(pointer)
      , directDeclarator(directDeclarator)
    {
    }

    Declarator(const DirectDeclarator *directDeclarator)
      : pointer(nullptr)
      , directDeclarator(directDeclarator)
    {
    }

    virtual ~Declarator()
    {
      delete(directDeclarator);
      delete(pointer);
    }

    void traverse(Visitor &visitor) const
    {
      try
      {
        visitor.accept(*this);
      }
      catch (const Visitor::Exception &)
      {
        visitor.accept(Visitor::Phases::PRE, *this);
        if (pointer != nullptr) pointer->traverse(visitor);
        directDeclarator->traverse(visitor);
        visitor.accept(Visitor::Phases::POST, *this);
      }
    }
};

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
      try
      {
        visitor.accept(*this);
      }
      catch (const Visitor::Exception &)
      {
        visitor.accept(Visitor::Phases::PRE, *this);
        visitor.accept(Visitor::Phases::POST, *this);
      }
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
      try
      {
        visitor.accept(*this);
      }
      catch (const Visitor::Exception &)
      {
        visitor.accept(Visitor::Phases::PRE, *this);
        visitor.accept(Visitor::Phases::POST, *this);
      }
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
      try
      {
        visitor.accept(*this);
      }
      catch (const Visitor::Exception &)
      {
        visitor.accept(Visitor::Phases::PRE, *this);
        for (const Expression *expression : *this)
        {
          expression->traverse(visitor);
        }
        visitor.accept(Visitor::Phases::POST, *this);
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
      assert((type == Type::MEMBER) || (type == Type::POINTER));
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
      try
      {
        visitor.accept(*this);
      }
      catch (const Visitor::Exception &)
      {
        visitor.accept(Visitor::Phases::PRE, *this);
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
          case Type::POINTER:
          case Type::INCREMENT:
          case Type::DECREMENT:
            structure->traverse(visitor);
            break;
        }
        visitor.accept(Visitor::Phases::POST, *this);
      }
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
};

class CastExpression : public Expression
{
  public:
    const TypeName   *typeName;
    const Expression *expression;

    CastExpression(const TypeName *typeName, const Expression *expression)
      : typeName(typeName)
      , expression(expression)
    {
    }

    ~CastExpression() override
    {
      delete(expression);
    }

    void traverse(Visitor &visitor) const override
    {
      try
      {
        visitor.accept(*this);
      }
      catch (const Visitor::Exception &)
      {
        visitor.accept(Visitor::Phases::PRE, *this);
        typeName->traverse(visitor);
        expression->traverse(visitor);
        visitor.accept(Visitor::Phases::POST, *this);
      }
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
      try
      {
        visitor.accept(*this);
      }
      catch (const Visitor::Exception &)
      {
        visitor.accept(Visitor::Phases::PRE, *this);
        a->traverse(visitor);
        b->traverse(visitor);
        visitor.accept(Visitor::Phases::POST, *this);
      }
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
      try
      {
        visitor.accept(*this);
      }
      catch (const Visitor::Exception &)
      {
        visitor.accept(Visitor::Phases::PRE, *this);
        a->traverse(visitor);
        b->traverse(visitor);
        visitor.accept(Visitor::Phases::POST, *this);
      }
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
      try
      {
        visitor.accept(*this);
      }
      catch (const Visitor::Exception &)
      {
        visitor.accept(Visitor::Phases::PRE, *this);
        a->traverse(visitor);
        b->traverse(visitor);
        visitor.accept(Visitor::Phases::POST, *this);
      }
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
      try
      {
        visitor.accept(*this);
      }
      catch (const Visitor::Exception &)
      {
        visitor.accept(Visitor::Phases::PRE, *this);
        a->traverse(visitor);
        b->traverse(visitor);
        visitor.accept(Visitor::Phases::POST, *this);
      }
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
      try
      {
        visitor.accept(*this);
      }
      catch (const Visitor::Exception &)
      {
        visitor.accept(Visitor::Phases::PRE, *this);
        a->traverse(visitor);
        b->traverse(visitor);
        visitor.accept(Visitor::Phases::POST, *this);
      }
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
      try
      {
        visitor.accept(*this);
      }
      catch (const Visitor::Exception &)
      {
        visitor.accept(Visitor::Phases::PRE, *this);
        a->traverse(visitor);
        b->traverse(visitor);
        visitor.accept(Visitor::Phases::POST, *this);
      }
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
      try
      {
        visitor.accept(*this);
      }
      catch (const Visitor::Exception &)
      {
        visitor.accept(Visitor::Phases::PRE, *this);
        a->traverse(visitor);
        b->traverse(visitor);
        visitor.accept(Visitor::Phases::POST, *this);
      }
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
      try
      {
        visitor.accept(*this);
      }
      catch (const Visitor::Exception &)
      {
        visitor.accept(Visitor::Phases::PRE, *this);
        a->traverse(visitor);
        b->traverse(visitor);
        visitor.accept(Visitor::Phases::POST, *this);
      }
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
      try
      {
        visitor.accept(*this);
      }
      catch (const Visitor::Exception &)
      {
        visitor.accept(Visitor::Phases::PRE, *this);
        a->traverse(visitor);
        b->traverse(visitor);
        visitor.accept(Visitor::Phases::POST, *this);
      }
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
      try
      {
        visitor.accept(*this);
      }
      catch (const Visitor::Exception &)
      {
        visitor.accept(Visitor::Phases::PRE, *this);
        a->traverse(visitor);
        b->traverse(visitor);
        visitor.accept(Visitor::Phases::POST, *this);
      }
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
      try
      {
        visitor.accept(*this);
      }
      catch (const Visitor::Exception &)
      {
        visitor.accept(Visitor::Phases::PRE, *this);
        condition->traverse(visitor);
        a->traverse(visitor);
        b->traverse(visitor);
        visitor.accept(Visitor::Phases::POST, *this);
      }
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
      try
      {
        visitor.accept(*this);
      }
      catch (const Visitor::Exception &)
      {
        visitor.accept(Visitor::Phases::PRE, *this);
        visitor.accept(Visitor::Phases::POST, *this);
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
      try
      {
        visitor.accept(*this);
      }
      catch (const Visitor::Exception &)
      {
        visitor.accept(Visitor::Phases::PRE, *this);
        expression->traverse(visitor);
        visitor.accept(Visitor::Phases::POST, *this);
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
      try
      {
        visitor.accept(*this);
      }
      catch (const Visitor::Exception &)
      {
        visitor.accept(Visitor::Phases::PRE, *this);
        storageClassDeclarationSpecifiers->traverse(visitor);
        declarator->traverse(visitor);
        if (initializer != nullptr)
        {
          initializer->traverse(visitor);
        }
        visitor.accept(Visitor::Phases::POST, *this);
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
      try
      {
        visitor.accept(*this);
      }
      catch (const Visitor::Exception &)
      {
        visitor.accept(Visitor::Phases::PRE, *this);
        if (storageClassDeclarationSpecifiers != nullptr)
        {
          storageClassDeclarationSpecifiers->traverse(visitor);
        }
        if (initDeclaratorList != nullptr)
        {
          initDeclaratorList->traverse(visitor);
        }
        visitor.accept(Visitor::Phases::POST, *this);
      }
    }
};

class Statement : public DeclarationStatement
{
  public:
};

class DeclarationStatementList : public std::vector<DeclarationStatement*>
{
  public:
    DeclarationStatementList(DeclarationStatement *declarationStatement)
    {
      push_back(declarationStatement);
    }

    virtual ~DeclarationStatementList()
    {
      for (const DeclarationStatement *declarationStatement : *this)
      {
        delete(declarationStatement);
      }
    }

    void add(DeclarationStatement *declarationStatement)
    {
      push_back(declarationStatement);
    }

    void traverse(Visitor &visitor) const
    {
      try
      {
        visitor.accept(*this);
      }
      catch (const Visitor::Exception &)
      {
        visitor.accept(Visitor::Phases::PRE, *this);
        for (const DeclarationStatement *declarationStatement : *this)
        {
          declarationStatement->traverse(visitor);
        }
        visitor.accept(Visitor::Phases::POST, *this);
      }
    }
};

class LabeledStatement : public Location, public Statement
{
  public:
    enum class Type
    {
      CASE,
      DEFAULT
    };

    Type             type;
    const Expression *constantExpression;
    const Statement  *statement;

    LabeledStatement(const Location &location, const Expression *constantExpression, const Statement *statement)
      : type(Type::CASE)
      , constantExpression(constantExpression)
      , statement(statement)
    {
    }

    LabeledStatement(const Location &location, const Statement *statement)
      : type(Type::DEFAULT)
      , constantExpression(nullptr)
      , statement(statement)
    {
    }

    ~LabeledStatement() override
    {
      delete(statement);
      delete(constantExpression);
    }

    void traverse(Visitor &visitor) const override
    {
      try
      {
        visitor.accept(*this);
      }
      catch (const Visitor::Exception &)
      {
        visitor.accept(Visitor::Phases::PRE, *this);
        if (constantExpression != nullptr) constantExpression->traverse(visitor);
        statement->traverse(visitor);
        visitor.accept(Visitor::Phases::POST, *this);
      }
    }
};

class CompoundStatement : public Location, public Statement
{
  public:
    const DeclarationStatementList *declarationStatementList;

    CompoundStatement(const Location &location, const DeclarationStatementList *declarationStatementList)
      : declarationStatementList(declarationStatementList)
    {
    }

    CompoundStatement(const Location &location)
      : declarationStatementList(nullptr)
    {
    }

    ~CompoundStatement() override
    {
      delete(declarationStatementList);
    }

    void traverse(Visitor &visitor) const override
    {
      try
      {
        visitor.accept(*this);
      }
      catch (const Visitor::Exception &)
      {
        visitor.accept(Visitor::Phases::PRE, *this);
        if (declarationStatementList != nullptr)
        {
          declarationStatementList->traverse(visitor);
        }
        visitor.accept(Visitor::Phases::POST, *this);
      }
    }
};

class IfStatement : public Location, public Statement
{
  public:
    const Expression *condition;
    const Statement  *ifStatement;
    const Statement  *elseStatement;

    IfStatement(const Location &location, const Expression *condition, const Statement *ifStatement, const Statement *elseStatement)
      : Location(location)
      , condition(condition)
      , ifStatement(ifStatement)
      , elseStatement(elseStatement)
    {
    }

    IfStatement(const Location &location, const Expression *condition, const Statement *ifStatement)
      : Location(location)
      , condition(condition)
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
      try
      {
        visitor.accept(*this);
      }
      catch (const Visitor::Exception &)
      {
        visitor.accept(Visitor::Phases::PRE, *this);
        condition->traverse(visitor);
        ifStatement->traverse(visitor);
        if (elseStatement != nullptr) elseStatement->traverse(visitor);
        visitor.accept(Visitor::Phases::POST, *this);
      }
    }
};

class SwitchStatement : public Location, public Statement
{
  public:
    const Expression *expression;
    const Statement  *statement;

    SwitchStatement(const Location &location, const Expression *expression, const Statement *statement)
      : Location(location)
      , expression(expression)
      , statement(statement)
    {
    }

    ~SwitchStatement() override
    {
      delete(statement);
      delete(expression);
    }

    virtual void traverse(Visitor &visitor) const
    {
      try
      {
        visitor.accept(*this);
      }
      catch (const Visitor::Exception &)
      {
        visitor.accept(Visitor::Phases::PRE, *this);
        expression->traverse(visitor);
        statement->traverse(visitor);
        visitor.accept(Visitor::Phases::POST, *this);
      }
    }
};

class ForStatement : public Location, public Statement
{
  public:
    const DeclarationStatement *init;
    const Statement            *condition;
    const Expression           *increment;
    const Statement            *statement;

    ForStatement(const Location &location, const DeclarationStatement *init, const Statement *condition, const Expression *increment, const Statement *statement)
      : Location(location)
      , init(init)
      , condition(condition)
      , increment(increment)
      , statement(statement)
    {
    }

    ForStatement(const Location &location, const DeclarationStatement *init, const Statement *condition, const Statement *statement)
      : Location(location)
      , init(init)
      , condition(condition)
      , increment(nullptr)
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
      try
      {
        visitor.accept(*this);
      }
      catch (const Visitor::Exception &)
      {
        visitor.accept(Visitor::Phases::PRE, *this);
        if (init != nullptr) init->traverse(visitor);
        if (condition != nullptr) condition->traverse(visitor);
        if (increment != nullptr) increment->traverse(visitor);
        statement->traverse(visitor);
        visitor.accept(Visitor::Phases::POST, *this);
      }
    }
};

class WhileStatement : public Location, public Statement
{
  public:
    const Expression *condition;
    const Statement  *statement;

    WhileStatement(const Location &location, const Expression *condition, const Statement *statement)
      : Location(location)
      , condition(condition)
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
      try
      {
        visitor.accept(*this);
      }
      catch (const Visitor::Exception &)
      {
        visitor.accept(Visitor::Phases::PRE, *this);
        condition->traverse(visitor);
        statement->traverse(visitor);
        visitor.accept(Visitor::Phases::POST, *this);
      }
    }
};

class DoStatement : public Location, public Statement
{
  public:
    const Statement  *statement;
    const Expression *condition;

    DoStatement(const Location &location, const Statement *statement, const Expression *condition)
      : Location(location)
      , statement(statement)
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
      try
      {
        visitor.accept(*this);
      }
      catch (const Visitor::Exception &)
      {
        visitor.accept(Visitor::Phases::PRE, *this);
        statement->traverse(visitor);
        condition->traverse(visitor);
        visitor.accept(Visitor::Phases::POST, *this);
      }
    }
};

class AssignmentStatement : public Location, public Statement
{
  public:
    const Expression *left;
    const Expression *right;

    AssignmentStatement(const Location &location, const Expression *left, const Expression *right)
      : Location(location)
      , left(left)
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
      try
      {
        visitor.accept(*this);
      }
      catch (const Visitor::Exception &)
      {
        visitor.accept(Visitor::Phases::PRE, *this);
        left->traverse(visitor);
        right->traverse(visitor);
        visitor.accept(Visitor::Phases::POST, *this);
      }
    }
};

class ExpressionStatement : public Location, public Statement
{
  public:
    Expression *expression;

    ExpressionStatement(const Location &location, Expression *expression)
      : Location(location)
      , expression(expression)
    {
    }

#if 0
    ExpressionStatement(const Location &location)
      : Location(location)
      , expression(nullptr)
    {
    }
#endif

    ~ExpressionStatement() override
    {
      delete(expression);
    }

    void traverse(Visitor &visitor) const override
    {
      try
      {
        visitor.accept(*this);
      }
      catch (const Visitor::Exception &)
      {
        visitor.accept(Visitor::Phases::PRE, *this);
        expression->traverse(visitor);
        visitor.accept(Visitor::Phases::POST, *this);
      }
    }
};

class JumpStatement : public Location, public Statement
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

    JumpStatement(const Location &location, Type type, Expression *expression)
      : Location(location)
      , type(type)
      , expression(expression)
    {
    }

    JumpStatement(const Location &location, Type type)
      : Location(location)
      , type(type)
      , expression(nullptr)
    {
    }

    ~JumpStatement() override
    {
      delete(expression);
    }

    void traverse(Visitor &visitor) const override
    {
      try
      {
        visitor.accept(*this);
      }
      catch (const Visitor::Exception &)
      {
        visitor.accept(Visitor::Phases::PRE, *this);
        if (expression != nullptr)
        {
          expression->traverse(visitor);
        }
        visitor.accept(Visitor::Phases::POST, *this);
      }
    }
};

class NewStateStatement : public Location, public Statement
{
  public:
    enum class Type
    {
      START,
      DEFAULT,
      POP,
      CUSTOM
    };

    enum class PrefixOperator
    {
      NONE,
      PUSH,
      RESET
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
    const Options  *options;

    NewStateStatement(const Location &location, const Identifier &name, PrefixOperator prefixOperator, const Options *options)
      : Location(location)
      , type(getType(name))
      , name(name)
      , prefixOperator(prefixOperator)
      , options(options)
    {
    }

    NewStateStatement(const Location &location, const Identifier &name, PrefixOperator prefixOperator)
      : Location(location)
      , type(getType(name))
      , name(name)
      , prefixOperator(prefixOperator)
      , options(new Options())
    {
    }

    NewStateStatement(const Location &location, const Identifier &name, const Options *options)
      : Location(location)
      , type(getType(name))
      , name(name)
      , prefixOperator(PrefixOperator::NONE)
      , options(options)
    {
    }

    NewStateStatement(const Location &location, const Identifier &name)
      : Location(location)
      , type(getType(name))
      , name(name)
      , prefixOperator(PrefixOperator::NONE)
      , options(new Options())
    {
    }
    
    ~NewStateStatement() override
    {
      delete(options);
    }

    virtual void traverse(Visitor &visitor) const
    {
      try
      {
        visitor.accept(*this);
      }
      catch (const Visitor::Exception &)
      {
        visitor.accept(Visitor::Phases::PRE, *this);
        visitor.accept(Visitor::Phases::POST, *this);
      }
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

class Initially : VisitorInterface
{
  public:
    Location                location;
    const CompoundStatement *compoundStatement;

    void traverse(Visitor &visitor) const override
    {
      try
      {
        visitor.accept(*this);
      }
      catch (const Visitor::Exception &)
      {
        visitor.accept(Visitor::Phases::PRE, *this);
        visitor.accept(Visitor::Phases::POST, *this);
      }
    }
};

class Finally : VisitorInterface
{
  public:
    Location                location;
    const CompoundStatement *compoundStatement;

    void traverse(Visitor &visitor) const override
    {
      try
      {
        visitor.accept(*this);
      }
      catch (const Visitor::Exception &)
      {
        visitor.accept(Visitor::Phases::PRE, *this);
        visitor.accept(Visitor::Phases::POST, *this);
      }
    }
};

class State : public Location
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

    State(const Location &location, Type type, const Identifier &name, const CompoundStatement *compoundStatement)
      : Location(location)
      , type(type)
      , name(name)
      , compoundStatement(compoundStatement)
    {
    }

    State(const Location &location, Type type, const CompoundStatement *compoundStatement)
      : Location(location)
      , type(type)
      , name()
      , compoundStatement(compoundStatement)
    {
    }

    virtual ~State()
    {
      delete(compoundStatement);
    }

    void traverse(Visitor &visitor) const
    {
      try
      {
        visitor.accept(*this);
      }
      catch (const Visitor::Exception &)
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

    void traverse(Visitor &visitor) const
    {
      try
      {
        visitor.accept(*this);
      }
      catch (const Visitor::Exception &)
      {
        visitor.accept(Visitor::Phases::PRE, *this);
        for (const State *state : *this)
        {
          state->traverse(visitor);
        }
        visitor.accept(Visitor::Phases::POST, *this);
      }
    }
};
  
class AST
{
  public:
    typedef std::unordered_multimap<Identifier,
                                    const NewStateStatement*,
                                    std::hash<std::string>
                                   > StateTransitionMap;

    /** construct AST
     * @param stateStackSize state stack size of 0
     * @param asserts true to generate asserts
     */
    AST(uint stateStackSize, bool asserts);

    /** clear AST
     */
    void clear();

    /** travers AST
     * @param visitor visitor
     */
    void traverse(Visitor &visitor) const;

    /** get FSM name
     * @return name
     */
    std::string getFSMName() const
    {
      return fsmName;
    }

    /** validate states
     */
    void validateStates() const;

    /** check if stack is required: FSM with push/pop state changes
     * @return true iff stack is required
     */
    bool isStackRequired() const;

    /** check if FSM with state stack
     * @return true iff has FSM state stack
     */
    bool hasStateStack() const
    {
      return stateStackSize > 0;
    }

    /** get FSM state stack size
     * @return state stack size of 0
     */
    uint getStateStackSize() const
    {
      return stateStackSize;
    }

    /** check if asserts should be generated
     * @return true iff asserts should be generated
     */
    uint isAsserts() const
    {
      return asserts;
    }

    /** get FSM state
     * @param name state name
     * @return start state
     */
    const State* getState(const std::string &name) const;

    /** get FSM start state
     * @return start state or nullptr
     */
    const State* getStartState() const;

    /** get FSM default state
     * @return default state or nullptr
     */
    const State* getDefaultState() const;
    
    /** get FSM initially statement
     * @return initially or nullptr
     */
    const Initially *getInitiallyStatement() const
    {
      return (initially.compoundStatement != nullptr) ? &initially : nullptr;
    }

    /** get FSM finally statement
     * @return finally or nullptr
     */
    const Finally *getFinallyStatement() const
    {
      return (finally.compoundStatement != nullptr) ? &finally : nullptr;
    }

    /** get FSM state list
     * @return state list
     */
    const StateList getStateList() const
    {
      StateList stateList;

      for (const std::pair<const std::string,State*> &pair : states)
      {
        stateList.push_back(pair.second);
      }

      return stateList;
    }

    /** iterate over all states which have a transition to state
     * @param state to-state
     * @param handler iteration handler
     */
    void doStates(const State                                                                  *toState,
                  std::function<void(const State*,const NewStateStatement &newStateStatement)> handler
                 ) const;

    /** iterate over all states
     * @param handler iteration handler
     */
    void doStates(std::function<void(const State *state)> handler) const;

    /** iterate over all transitions from state
     * @param state state
     * @param handler iteration handler
     */
    void doStateTransitions(const State                                                     *fromState,
                            std::function<void(const NewStateStatement &newStateStatement)> handler
                           ) const;

    /** iterate over all state transitions
     * @param handler iteration handler
     */
    void doStateTransitions(std::function<void(const NewStateStatement &newStateStatement)> handler) const;

    /** print AST
     */
    void print() const;

  private:
    // allow access to private methods/members
    friend class Parser;
    friend class Visitor;
    friend int ::yyparse();

    void setFSMName(const std::string &fsmName)
    {
      this->fsmName = fsmName;
    }

    void setStartState(const Identifier &startState)
    {
      this->startState = startState;
    }
    
    /** set initially code block
     * @param state state
     */
    void setInitially(const Location &location, const CompoundStatement *compoundStatement);

    /** set finally code block
     * @param state state
     */
    void setFinally(const Location &location, const CompoundStatement *compoundStatement);

    /** add state
     * @param state state
     */
    void addState(State *state);

    /** add state transition
     * @param name state name
     * @param newStateStatement new state statement
     */
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
    class StateMap : public std::unordered_map<const std::string,
                                               State*,
                                               std::hash<std::string>
                                              >
    {
      public:
        virtual ~StateMap()
        {
          for (const std::pair<const std::string,State*> &pair : *this)
          {
            delete(pair.second);
          }
        }

        void traverse(Visitor &visitor) const
        {
          for (const std::pair<const std::string,State*> &state : *this)
          {
            state.second->traverse(visitor);
          }
        }
    };

    uint               stateStackSize;
    bool               asserts;
    std::string        fsmName;

    Identifier         startState;

    Initially          initially;
    Finally            finally;

    StateMap           states;
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

/****************************** Macros *********************************/

/***************************** Functions *******************************/

#endif // AST_H

/* end of file */
