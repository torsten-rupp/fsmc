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

namespace FSM
{

class Identifier : public std::string
{
  public:
    Identifier()
    {
    }

    Identifier(char * &identifier)
      : std::string(identifier)
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

class Number
{
  public:
    enum class Types
    {
      INTEGER,
      FLOAT
    };

    Types   type;
    int64_t i;
    double  d;

    Number()
      : type(Types::INTEGER)
      , i(0)
    {
    }

    Number(int64_t i)
      : type(Types::INTEGER)
      , i(i)
    {
    }

    Number(double d)
      : type(Types::FLOAT)
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
        case Types::INTEGER: return std::to_string(i);
        case Types::FLOAT:   return std::to_string(d);
      }
    }
};

class TypeDeclaration
{
  public:
    Identifier name;
    
    TypeDeclaration()
    {
    }

    TypeDeclaration(const Identifier &name)
      : name(name)
    {
    }

    void accept(Visitor &visitor) const
    {
      visitor.accept(Visitor::Phases::ENTER, *this);
      visitor.accept(Visitor::Phases::LEAVE, *this);
    }

    std::string toString() const
    {
// TODO:
      return "XXX";
    }
};

class Expression;

class ExpressionList : public std::vector<Expression>
{
  public:
    ExpressionList()
    {
    }

    ExpressionList(const Expression &expression)
    {
      push_back(expression);
    }

    virtual ~ExpressionList()
    {
      // TODO: free
    }

    void add(const Expression &expression)
    {
      push_back(expression);
    }
};

class Expression
{
  public:
    enum class Operators
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

    Expression()
      : operator_(Operators::NONE)
      , nodes{nullptr, nullptr}
    {
    }

    Expression(const Expression &other)
      : operator_(other.operator_)
//      , nodes{new Expression(other.nodes[0]),new Expression(other.nodes[1])}
      , nodes{other.nodes[0],other.nodes[1]}
      , identifier(other.identifier)
      , number(other.number)
      , string(other.string)
      , functionName(other.functionName)
      , argumentList(other.argumentList)
    {
    }

    Expression(const Identifier &identifier)
      : operator_(Operators::IDENTIFIER)
      , nodes{nullptr, nullptr}
      , identifier(identifier)
    {
    }

    Expression(const std::string &string)
      : operator_(Operators::STRING)
      , nodes{nullptr, nullptr}
      , string(string)
    {
    }

    Expression(const Number &number)
      : operator_(Operators::NUMBER)
      , nodes{nullptr, nullptr}
      , number(number)
    {
    }

    Expression(Operators operator_, const Expression &expression1, const Expression &expression2)
      : operator_(operator_)
      , nodes{new Expression(expression1), new Expression(expression2)}
    {
    }

    Expression(Operators operator_, const Expression &expression)
      : operator_(operator_)
      , nodes{new Expression(expression), nullptr}
    {
    }

    Expression(const Identifier &functionName, ExpressionList &argumentList)
      : operator_(Operators::FUNCTION_CALL)
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

    void accept(Visitor &visitor) const
    {
      visitor.accept(Visitor::Phases::ENTER, *this);
      visitor.accept(Visitor::Phases::LEAVE, *this);
    }

    std::string toString() const
    {
      switch (operator_)
      {
        case Operators::NONE:           return "";

        case Operators::IDENTIFIER:     return identifier;
        case Operators::STRING:         return "\""+string+"\"";
        case Operators::NUMBER:         return number.toString();

        case Operators::ASSIGN:         return nodes[0]->toString()+" = "+nodes[1]->toString();

        case Operators::NEGATIVE:       return "-"+nodes[0]->toString();
        case Operators::PLUS:           return nodes[0]->toString()+" + "+nodes[1]->toString();
        case Operators::MINUS:          return nodes[0]->toString()+" - "+nodes[1]->toString();
        case Operators::MULTIPLY:       return nodes[0]->toString()+" * "+nodes[1]->toString();
        case Operators::DIVIDE:         return nodes[0]->toString()+" / "+nodes[1]->toString();
        case Operators::INCREMENT:      return nodes[0]->toString()+"++";
        case Operators::DECREMENT:      return nodes[0]->toString()+"--";

        case Operators::EQUALS:         return nodes[0]->toString()+" == "+nodes[1]->toString();
        case Operators::NOT_EQUALS:     return nodes[0]->toString()+" != "+nodes[1]->toString();
        case Operators::LOWER:          return nodes[0]->toString()+" < "+nodes[1]->toString();
        case Operators::GREATER:        return nodes[0]->toString()+" > "+nodes[1]->toString();
        case Operators::LOWER_EQUALS:   return nodes[0]->toString()+" <= "+nodes[1]->toString();
        case Operators::GREATER_EQUALS: return nodes[0]->toString()+" >= "+nodes[1]->toString();

        case Operators::NOT:            return "!"+nodes[0]->toString();
        case Operators::AND:            return nodes[0]->toString()+" && "+nodes[1]->toString();
        case Operators::OR:             return nodes[0]->toString()+" || "+nodes[1]->toString();
        case Operators::XOR:            return nodes[0]->toString()+" ^ "+nodes[1]->toString();

        case Operators::FUNCTION_CALL:
          {
            std::stringstream buffer;
            bool              first = true;

            buffer << functionName << "(";
            for (const Expression &argument : argumentList)
            {
              if (!first) buffer << ", ";
              buffer << argument;

              first = false;
            }
            buffer << ")";

            return buffer.str();
          }

        case Operators::CAST:           return "("+nodes[0]->toString()+")"+nodes[1]->toString();

        default:                        return "unknown operator "+std::to_string((int)operator_);
      }

      return "";
    }

    friend std::ostream& operator<<(std::ostream& outputStream, const Expression &expression)
    {
      outputStream << expression.toString();

      return outputStream;
    }
};

class StatementList;

class Statement
{
  public:
#if 1
    virtual std::string toString() const = 0;
#else
    virtual std::string toString() const
    {
      fprintf(stderr,"%s:%d: upppps!\n",__FILE__,__LINE__);

    }
#endif
    virtual void accept(Visitor &visitor) const = 0;

    friend std::ostream& operator<<(std::ostream& outputStream, const Statement *statement)
    {
      outputStream << statement->toString();

      return outputStream;
    }
};

class StatementList : public std::vector<Statement*>
{
  public:
    StatementList()
    {
    }

    StatementList(Statement *statement)
    {
      push_back(statement);
    }

    virtual ~StatementList()
    {
      // TODO: free
    }

    void add(Statement *statement)
    {
      push_back(statement);
    }

    void accept(Visitor &visitor) const
    {
      for (const Statement *statement : *this)
      {
        statement->accept(visitor);
      }
    }
};

class BlockStatement : public Statement
{
  public:
    StatementList statementList;

    BlockStatement(const StatementList &statementList)
      : statementList(statementList)
    {
    }

    virtual void accept(Visitor &visitor) const
    {
      visitor.accept(Visitor::Phases::ENTER, *this);
      visitor.accept(Visitor::Phases::LEAVE, *this);
    }

    std::string toString() const override
    {
      std::stringstream buffer;

      buffer << " {";
      for (const Statement *statement : statementList)
      {
        buffer << statement;
      }
      buffer << "}";

      return buffer.str();
    }
};

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

    virtual void accept(Visitor &visitor) const
    {
      visitor.accept(Visitor::Phases::ENTER, *this);
      visitor.accept(Visitor::Phases::LEAVE, *this);
    }

    std::string toString() const override
    {
      std::stringstream buffer;

      buffer << typeDeclaration.toString() << name << ";";

      return buffer.str();
    }
};

class IfStatement : public Statement
{
  public:
    Expression condition;
    Statement  *statement, *elseStatement;

    IfStatement(const Expression &condition, Statement *ifStatement, Statement *elseStatement)
      : condition(condition)
      , statement(ifStatement)
      , elseStatement(elseStatement)
    {
    }

    IfStatement(const Expression &condition, Statement *ifStatement)
      : condition(condition)
      , statement(ifStatement)
      , elseStatement(nullptr)
    {
    }

    virtual void accept(Visitor &visitor) const
    {
      visitor.accept(Visitor::Phases::ENTER, *this);
      visitor.accept(Visitor::Phases::LEAVE, *this);
    }

    std::string toString() const override
    {
      std::stringstream buffer;

      buffer << "if" << " (" << condition.toString() << ")";
      buffer << statement;
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
    Expression init;
    Expression condition;
    Expression increment;
    Statement  *statement;

    ForStatement(const Expression &init, const Expression &condition, const Expression &increment, Statement *statement)
      : init(init)
      , condition(condition)
      , increment(increment)
      , statement(statement)
    {
    }

    virtual void accept(Visitor &visitor) const
    {
      visitor.accept(Visitor::Phases::ENTER, *this);
      visitor.accept(Visitor::Phases::LEAVE, *this);
    }

    std::string toString() const override
    {
      std::stringstream buffer;


      buffer << "for" << " (" << init.toString() << ";" << condition.toString() << ";" << increment.toString() << ")";
      buffer << "{";
      buffer << statement;
      buffer << "}";

      return buffer.str();
    }
};

class WhileStatement : public Statement
{
  public:
    Expression condition;
    Statement  *statement;

    WhileStatement(const Expression &condition, Statement *statement)
      : condition(condition)
      , statement(statement)
    {
    }

    virtual void accept(Visitor &visitor) const
    {
      visitor.accept(Visitor::Phases::ENTER, *this);
      visitor.accept(Visitor::Phases::LEAVE, *this);
    }

    std::string toString() const override
    {
      std::stringstream buffer;

      buffer << "while" << " (" << condition.toString() << ")";
      buffer << statement;

      return buffer.str();
    }
};

class DoStatement : public Statement
{
  public:
    Expression condition;
    Statement  *statement;

    DoStatement(const Expression &condition, Statement *statement)
      : condition(condition)
      , statement(statement)
    {
    }

    virtual void accept(Visitor &visitor) const
    {
      visitor.accept(Visitor::Phases::ENTER, *this);
      visitor.accept(Visitor::Phases::LEAVE, *this);
    }

    std::string toString() const override
    {
      std::stringstream buffer;

      buffer << "do";
      buffer << statement;
      buffer << "while" << " (" << condition.toString() << ")";

      return buffer.str();
    }
};

class AssignmentStatement : public Statement
{
  public:
    Expression left;
    Expression right;

    AssignmentStatement(const Expression &left, const Expression &right)
      : left(left)
      , right(right)
    {
    }

    virtual void accept(Visitor &visitor) const
    {
      visitor.accept(Visitor::Phases::ENTER, *this);
      visitor.accept(Visitor::Phases::LEAVE, *this);
    }

    std::string toString() const override
    {
      std::stringstream buffer;

      buffer << left.toString() << " = " << right.toString() << ";";

      return buffer.str();
    }

};

class ExpressionStatement : public Statement
{
  public:
    Expression expression;

    ExpressionStatement(const Expression &expression)
      : expression(expression)
    {
    }

    virtual void accept(Visitor &visitor) const
    {
      visitor.accept(Visitor::Phases::ENTER, *this);
      expression.accept(visitor);
      visitor.accept(Visitor::Phases::LEAVE, *this);
    }

    std::string toString() const override
    {
      std::stringstream buffer;

      buffer << expression.toString()  << ";";

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

    virtual void accept(Visitor &visitor) const
    {
      visitor.accept(Visitor::Phases::ENTER, *this);
      visitor.accept(Visitor::Phases::LEAVE, *this);
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
    Identifier    name;
    StatementList statementList;

    State()
      : name()
    {
    }

    State(const Identifier &name, const StatementList &statementList)
      : name(name)
      , statementList(statementList)
    {
    }

    void accept(Visitor &visitor) const
    {
      visitor.accept(Visitor::Phases::ENTER, *this);
      for (const FSM::Statement *statement : statementList)
      {
        statement->accept(visitor);
      }
      visitor.accept(Visitor::Phases::LEAVE, *this);
    }
};

class StateList : public std::vector<State>
{
  public:
    void add(const State &state)
    {
      push_back(state);
    }

    void accept(Visitor &visitor) const
    {
      visitor.accept(Visitor::Phases::ENTER, *this);
      for (const FSM::State state : *this)
      {
        state.accept(visitor);
      }
      visitor.accept(Visitor::Phases::LEAVE, *this);
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
