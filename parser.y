%skeleton "lalr1.cc" /* -*- C++ -*- */
%require "3.0"

%defines

%define parser_class_name { Parser }

%define api.token.constructor
%define api.value.type variant
%define api.namespace { FSM }
%define api.token.prefix {TOKEN_}
//%define api.pure full
//%define api.value.type union
//%define api.location.type { Location }

%define parse.assert
%define parse.trace
%define parse.error verbose

// top of header before YYTYPE
%code requires
{
  #include <iostream>
  #include <string>
  #include <vector>
  #include <stdint.h>

  #include "ast.h"

  namespace FSM
  {
    class Scanner;
  }
}

// top of header after YYTYPE
%code provides
{
}

// top of implementation
%code top
{
  #include <iostream>

  #include "scanner.h"
  #include "parser.hpp"
  #include "location.hh"
  #include "ast.h"

  static FSM::Parser::symbol_type yylex(FSM::Scanner &scanner)
  {
    return scanner.get_next_token();
  }
  
  FSM::Identifier currentStateName;
}

// implementation
%code
{
}

%lex-param { Scanner &scanner }
%parse-param { Scanner &scanner }
%parse-param { AST &ast }
%locations
%verbose

%token KEYWORD_FSM                  "#fsm";
%token KEYWORD_END                  "#end";
%token KEYWORD_IF                   "if";
%token KEYWORD_ELSE                 "else";
%token KEYWORD_FOR                  "for";
%token KEYWORD_WHILE                "while";
%token KEYWORD_DO                   "do";

%token END EOF 0                    "end of file"

%token <std::string> STATE          "state";
%token <Identifier>  IDENTIFIER     "identifier";
%token <std::string> STRING         "string";
%token <double>      FLOAT          "float";
%token <int64_t>     INTEGER        "integer";
%token               ASSIGN         "="
%token               PLUS           "+"
%token               MINUS          "-"
%token               MULTIPLY       "*"
%token               DIVIDE         "/"
%token               INCREMENT      "++"
%token               DECREMENT      "--"
%token               DEREFERENCE    "->"
%token               EQUALS         "=="
%token               NOT_EQUALS     "!="
%token               LOWER          "<"
%token               GREATER        ">"
%token               LOWER_EQUALS   "<="
%token               GREATER_EQUALS ">="
%token               NOT            "!"
%token               AND            "&&"
%token               OR             "||"
%token               XOR            "^"

%left NEGATIVE
%left PLUS MINUS
%left MULTIPLY DIVIDE

%type <Statement*>         statement;
%type <StatementList>      statementList;
%type <Statement*>         block;
%type <Statement*>         statementOrBlock;
%type <TypeDeclaration>    type;
%type <Statement*>         variable;
%type <Statement*>         if;
%type <Statement*>         for;
%type <Statement*>         while;
%type <Statement*>         do;
%type <Statement*>         assign;
%type <Statement*>         newState;
%type <Number>             number;
%type <Expression>         expression;
%type <ExpressionList>     expressionList;

%start start

%%

start:
    {
      // empty
    }
  | start KEYWORD_FSM IDENTIFIER[fsmName] definitions KEYWORD_END
    {
      ast.setFSMName($fsmName);
    }
  ;

// ---------------------------------------------------------------------

definitions:
    {
      // empty
    }
  | definitions MULTIPLY IDENTIFIER[stateName]
    {
      currentStateName = $stateName;
    }
    '{' statementList '}'
    {
      ast.addState(State($stateName,$statementList));
      ast.setStartState($stateName);      
    }
  | definitions IDENTIFIER[stateName] 
    {
      currentStateName = $stateName;
    }
    '{' statementList '}'
    {
      ast.addState(State($stateName,$statementList));
    }
  | error
    {
      YYABORT;
    }
  ;

// ---------------------------------------------------------------------

block:
    '{' statementList '}'
    {
      $$ = new BlockStatement($statementList);
    }
  ;

statementOrBlock:
    statement
    {
      $$ = $statement;
    }
  | block
    {
      $$ = $block;
    }
  ;

statement:
    variable
    {
      $$ = $1;
    }
  | if
    {
      $$ = $1;
    }
  | for
    {
      $$ = $1;
    }
  | while
    {
      $$ = $1;
    }
  | do
    {
      $$ = $1;
    }
  | assign
    {
      $$ = $1;
    }
  | newState ';'
    {
      $$ = $1;
    }
  | block
    {
      $$ = $1;
    }
  | expression ';'
    {
      $$ = new ExpressionStatement($expression);
    }
  ;

statementList:
    statementList statement
    {
      $1.add($statement);
      $$ = $1;
    }
  | statement
    {
      $$ = StatementList($statement);
    }
  | {
      $$ = StatementList();
    }
  ;

// ---------------------------------------------------------------------

variable:
    type IDENTIFIER[name] ASSIGN expression ';'
    {
// TODO:
      $$ = new VariableDeclarationStatement($type,$name,$expression);
    }
  | type IDENTIFIER[name]';'
    {
      $$ = new VariableDeclarationStatement($type,$name);
    }
  ;

// ---------------------------------------------------------------------

if:
    KEYWORD_IF '(' expression ')' statementOrBlock[a] KEYWORD_ELSE statementOrBlock[b]
    {
      $$ = new IfStatement($expression, $a, $b);
    }
  | KEYWORD_IF '(' expression ')' statementOrBlock
    {
      $$ = new IfStatement($expression, $statementOrBlock);
    }
  ;

// ---------------------------------------------------------------------

for:
    KEYWORD_FOR '(' expression[init] ';' expression[condition] ';' expression[increment] ')' statementOrBlock
    {
      $$ = new ForStatement($init, $condition, $increment, $statementOrBlock);
    }
  ;

// ---------------------------------------------------------------------

do:
    KEYWORD_DO statementOrBlock KEYWORD_WHILE '(' expression[condition] ')' ';'
    {
      $$ = new DoStatement($condition, $statementOrBlock);
    }
  ;

// ---------------------------------------------------------------------

while:
    KEYWORD_WHILE '(' expression[condition] ')' statementOrBlock
    {
      $$ = new WhileStatement($condition, $statementOrBlock);
    }
  ;

// ---------------------------------------------------------------------

assign:
    expression[left] ASSIGN expression[right] ';'
    {
      $$ = new AssignmentStatement($left, $right);
    }
    ;

// ---------------------------------------------------------------------

newState:
    DEREFERENCE IDENTIFIER[stateName] '{' STRING[label] ',' IDENTIFIER[color] ',' number[lineWidth] '}'
    {
      NewStateStatement *newStateStatement = new NewStateStatement($stateName,$label,$color,$lineWidth);
      ast.addStateTransition(currentStateName,*newStateStatement);
      $$ = newStateStatement;
    }
  | DEREFERENCE IDENTIFIER[stateName] '{' STRING[label] ',' IDENTIFIER[color] '}'
    {
      NewStateStatement *newStateStatement = new NewStateStatement($stateName,$label,$color);
      ast.addStateTransition(currentStateName,*newStateStatement);
      $$ = newStateStatement;
    }
  | DEREFERENCE IDENTIFIER[stateName] '{' STRING[label] '}'
    {
      NewStateStatement *newStateStatement = new NewStateStatement($stateName,$label);
      ast.addStateTransition(currentStateName,*newStateStatement);
      $$ = newStateStatement;
    }
  | DEREFERENCE IDENTIFIER[stateName] '{' '}'
    {
      NewStateStatement *newStateStatement = new NewStateStatement($stateName);
      ast.addStateTransition(currentStateName,*newStateStatement);
      $$ = newStateStatement;
    }
  | DEREFERENCE IDENTIFIER[stateName]
    {
      NewStateStatement *newStateStatement = new NewStateStatement($stateName);
      ast.addStateTransition(currentStateName,*newStateStatement);
      $$ = newStateStatement;
    }
  ;

// ---------------------------------------------------------------------

type:
    IDENTIFIER[name]
    {
      $$ = TypeDeclaration($name);
    }
  ;

// ---------------------------------------------------------------------

number:
    INTEGER[n]
    {
      $$ = Number($n);
    }
  | FLOAT[n]
    {
      $$ = Number($n);
    };

expression:
    '(' expression[a] ')'
    {
      fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__);

      $$ = Expression($a);
    }
  | MINUS expression[a] %prec NEGATIVE
    {
      $$ = Expression(Expression::Operators::NEGATIVE, $a);
    }

  | expression[a] ASSIGN expression[b]
    {
      $$ = Expression(Expression::Operators::ASSIGN, $a, $b);
    }

  | expression[a] PLUS expression[b]
    {
      $$ = Expression(Expression::Operators::PLUS, $a, $b);
    }
  | expression[a] MINUS expression[b]
    {
      $$ = Expression(Expression::Operators::MINUS, $a, $b);
    }
  | expression[a] MULTIPLY expression[b]
    {
      $$ = Expression(Expression::Operators::MULTIPLY, $a, $b);
    }
  | expression[a] DIVIDE expression[b]
    {
      $$ = Expression(Expression::Operators::DIVIDE, $a, $b);
    }

  | expression[a] INCREMENT
    {
      $$ = Expression(Expression::Operators::INCREMENT, $a);
    }
  | expression[a] DECREMENT
    {
      $$ = Expression(Expression::Operators::DECREMENT, $a);
    }

  | expression[a] EQUALS expression[b]
    {
      $$ = Expression(Expression::Operators::EQUALS, $a, $b);
    }
  | expression[a] NOT_EQUALS expression[b]
    {
      $$ = Expression(Expression::Operators::NOT_EQUALS, $a, $b);
    }
  | expression[a] LOWER expression[b]
    {
      $$ = Expression(Expression::Operators::LOWER, $a, $b);
    }
  | expression[a] GREATER expression[b]
    {
      $$ = Expression(Expression::Operators::GREATER, $a, $b);
    }
  | expression[a] LOWER_EQUALS expression[b]
    {
      $$ = Expression(Expression::Operators::LOWER_EQUALS, $a, $b);
    }
  | expression[a] GREATER_EQUALS expression[b]
    {
      $$ = Expression(Expression::Operators::GREATER_EQUALS, $a, $b);
    }

  | NOT expression[a]
    {
      $$ = Expression(Expression::Operators::NOT, $a);
    }
  | expression[a] AND expression[b]
    {
      $$ = Expression(Expression::Operators::AND, $a, $b);
    }
  | expression[a] OR expression[b]
    {
      $$ = Expression(Expression::Operators::OR, $a, $b);
    }
  | expression[a] XOR expression[b]
    {
      $$ = Expression(Expression::Operators::XOR, $a, $b);
    }

  | '(' expression[a] ')' expression[b]
    {
      $$ = Expression(Expression::Operators::CAST, $a, $b);
    }
  | IDENTIFIER[name] '(' expressionList ')'
    {
      $$ = Expression($name, $expressionList);
    }

  | IDENTIFIER[name]
    {
      $$ = Expression($name);
    }

  | STRING[string]
    {
      $$ = Expression($string);
    }

  | number[n]
    {
      $$ = Expression($n);
    }
  ;

expressionList:
    expressionList ',' expression
    {
      $1.add($expression);
      $$ = $1;
    }
  | expression
    {
      $$ = ExpressionList($expression);
    }
  | {
      $$ = ExpressionList();
    }
  ;

%%

void FSM::Parser::error(const location_type &location, const std::string &message)
{
  std::stringstream buffer;
  buffer << message << " at " << location;
  throw std::runtime_error(buffer.str());
}
