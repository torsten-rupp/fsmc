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

// TODO: locationt type
//%locations
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

  typedef struct
  {
    int first_line;
  } Location;
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
%parse-param { const std::string inputFilePath }
%parse-param { Scanner &scanner }
%parse-param { AST &ast }
%locations
%verbose

%token KEYWORD_FSM                      "#fsm";
%token KEYWORD_END                      "#end";

%token KEYWORD_IF                       "if";
%token KEYWORD_ELSE                     "else";
%token KEYWORD_FOR                      "for";
%token KEYWORD_WHILE                    "while";
%token KEYWORD_DO                       "do";
%token KEYWORD_SWITCH                   "switch";
%token KEYWORD_CASE                     "case";
%token KEYWORD_DEFAULT                  "default";
%token KEYWORD_BREAK                    "break";
%token KEYWORD_CONTINUE                 "continue";
%token KEYWORD_RETURN                   "return";
%token KEYWORD_SIZEOF                   "sizeof";

%token KEYWORD_AUTO                     "auto";
%token KEYWORD_REGISTER                 "register";
%token KEYWORD_STATIC                   "static";
%token KEYWORD_EXTERN                   "extern";
%token KEYWORD_TYPEDEF                  "typedef";

%token KEYWORD_VOID                     "void"
%token KEYWORD_CHAR                     "char"
%token KEYWORD_SHORT                    "short"
%token KEYWORD_INT                      "int"
%token KEYWORD_LONG                     "long"
%token KEYWORD_FLOAT                    "float"
%token KEYWORD_DOUBLE                   "double"
%token KEYWORD_SIGNED                   "signed"
%token KEYWORD_UNSIGNED                 "unsigned"

%token KEYWORD_STRUCT                   "struct";
%token KEYWORD_UNION                    "union";

%token KEYWORD_CONST                    "const";
%token KEYWORD_VOLATILE                 "volatile";

%token END EOF 0                        "end of file"

%token <std::string> STATE              "state";
%token <Identifier>  IDENTIFIER         "identifier";
%token <char>        CHAR               "charxxx";
%token <std::string> STRING             "string";
%token <double>      FLOAT              "floatxxx";
%token <int64_t>     INTEGER            "integer";
%token <int>         ENUM               "enum";

%token               MULTIPLY_ASSIGN    "*="
%token               DIVIDE_ASSIGN      "/="
%token               MODULO_ASSIGN      "%="
%token               ADD_ASSIGN         "+="
%token               SUB_ASSIGN         "-="
%token               SHIFT_LEFT_ASSIGN  "<<="
%token               SHIFT_RIGHT_ASSIGN ">>="
%token               AND_ASSIGN         "&="
%token               XOR_ASSIGN         "^="
%token               OR_ASSIGN          "|="

%token               PLUS               "+"
%token               MINUS              "-"
%token               NEG                "~"
%token               NOT                "!"
%token               MULTIPLY           "*"
%token               DIVIDE             "/"

%token               INCREMENT          "++"
%token               DECREMENT          "--"
%token               DEREFERENCE        "->"
%token               EQUALS             "=="
%token               NOT_EQUALS         "!="
%token               LOWER              "<"
%token               GREATER            ">"
%token               LOWER_EQUALS       "<="
%token               GREATER_EQUALS     ">="
%token               AND                "&&"
%token               OR                 "||"
%token               XOR                "^"
%token               SHIFT_LEFT         "<<"
%token               SHIFT_RIGHT        ">>"

%token               POINTER             "*&"

%token               ELLIPSIS            "..."

%left NEGATIVE
%left PLUS MINUS
%left MULTIPLY DIVIDE

%type <StorageClassSpecifier*>             storageClassSpecifier;
%type <TypeQualifier*>                     typeQualifier;
%type <TypeQualifierList*>                 typeQualifierList;
%type <TypeSpecifier*>                     typeSpecifier;
%type <StorageClassDeclarationSpecifiers*> storageClassDeclarationSpecifiers;
%type <Declaration*>                       declaration;
%type <Declarator*>                        declarator;
%type <DirectDeclarator*>                  directDeclarator;
%type <Initializer*>                       initializer;
%type <InitDeclarator*>                    initDeclarator;
%type <InitDeclaratorList*>                initDeclaratorList;
%type <Declaration*>                       externalDeclaration;
%type <DeclarationStatementList*>          declarationStatementList;

%type <Expression*>                        primaryExpression;
%type <Expression*>                        postfixExpression;
%type <Expression*>                        unaryExpression;
%type <UnaryExpression::Operator>          unaryOperator;
%type <Expression*>                        castExpression;
%type <Expression*>                        multiplicativeExpression;
%type <Expression*>                        additiveExpression;
%type <Expression*>                        shiftExpression;
%type <Expression*>                        relationalExpression;
%type <Expression*>                        equalityExpression;
%type <Expression*>                        andExpression;
%type <Expression*>                        exclusiveOrExpression;
%type <Expression*>                        inclusiveOrExpression;
%type <Expression*>                        logicalAndExpression;
%type <Expression*>                        logicalOrExpression;
%type <Expression*>                        conditionalExpression;
%type <Expression*>                        assignmentExpression;
%type <ArgumentExpressionList*>            argumentExpressionList;
%type <AssignmentExpression::Operator>     assignmentOperator;
%type <Expression*>                        expression;

%type <Statement*>                         statement;
%type <Statement*>                         labeled_statement;
%type <CompoundStatement*>                 compoundStatement;
%type <ExpressionStatement*>               expressionStatement;
%type <Statement*>                         selectionStatement;
%type <Statement*>                         iterationStatement;
%type <JumpStatement*>                     jumpStatement;
%type <NewStateStatement*>                 newStateStatement;
%type <NewStateStatement::PrefixOperator>  newStateStatementPrefixOperator;
%type <NewStateStatement::Options>         newStateStatementOptions;
%type <double>                             number;

%start start

%%

// ---------------------------------------------------------------------

start:
    {
      // empty
    }
  | start KEYWORD_FSM IDENTIFIER[fsmName] stateDefinitions KEYWORD_END
    {
      ast.setFSMName($fsmName);
    }
  ;

// ---------------------------------------------------------------------

/*

https://www.lysator.liu.se/c/ANSI-C-grammar-y.html#declaration

*/

stateDefinitions:
    {
      // empty
    }
  | stateDefinitions stateDefinition
  | error
    {
      YYABORT;
    }
  ;

stateDefinition:
    '*' IDENTIFIER[stateName]
    {
      currentStateName = $stateName;
    }
    compoundStatement
    {
      ast.addState(new State(State::Type::START,$stateName,$compoundStatement));
    }
  | IDENTIFIER[stateName]
    {
      currentStateName = $stateName;
    }
    compoundStatement
    {
      ast.addState(new State(State::Type::CUSTOM,$stateName,$compoundStatement));
    }
  | KEYWORD_DEFAULT compoundStatement
    {
      ast.addState(new State(State::Type::DEFAULT,$compoundStatement));
    }
  ;

// ---------------------------------------------------------------------

declarationStatementList
  : declarationStatementList externalDeclaration
    {
      $1->add($externalDeclaration);
      $$ = $1;
    }
  | externalDeclaration
    {
      $$ = new DeclarationStatementList($externalDeclaration);
    }
  | declarationStatementList statement
    {
      $1->add($statement);
      $$ = $1;
    }
  | statement
    {
      $$ = new DeclarationStatementList($statement);
    }
  ;

externalDeclaration
  : declaration
    {
      Declaration *p=$declaration;
      $$ = $declaration;
    }
//  | function_definition
  ;

declaration
  : initDeclaratorList ';'
    {
      $$ = new Declaration($initDeclaratorList);
    }
  ;

storageClassDeclarationSpecifiers
  : storageClassDeclarationSpecifiers storageClassSpecifier
    {
//      $2.prepend($1);
      $1->add($2);
      $$ = $1;
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  | storageClassSpecifier
    {
      $$ = new StorageClassDeclarationSpecifiers($storageClassSpecifier);
    }
//  : typeSpecifier storageClassDeclarationSpecifiers
  | storageClassDeclarationSpecifiers typeSpecifier
    {
//      $2.prepend($1);
      $1->add($2);
      $$ = $1;
    }
  | typeSpecifier
    {
      $$ = new StorageClassDeclarationSpecifiers($typeSpecifier);
    }
//  | typeQualifier storageClassDeclarationSpecifiers
  | storageClassDeclarationSpecifiers typeQualifier
    {
      $1->add($2);
      $$ = $1;
    }
  | typeQualifier
    {
      $$ = new StorageClassDeclarationSpecifiers($typeQualifier);
    }
  ;

// ---------------------------------------------------------------------

primaryExpression
  : IDENTIFIER[name]
    {
      $$ = new PrimaryExpression($name);
    }
  //| CONSTANT
  | INTEGER[n]
    {
      $$ = new PrimaryExpression($n);
    }
  | STRING[string]
    {
      $$ = new PrimaryExpression($string);
    }
  | '(' expression ')'
    {
      $$ = new PrimaryExpression($expression);
    }
  ;

postfixExpression
  : postfixExpression[a] '[' expression[b] ']'
    {
      $$ = new PostfixExpression(PostfixExpression::Type::SUBSCRIPT, $a, $b);
    }
  | postfixExpression[a] '(' ')'
    {
      $$ = new PostfixExpression(PostfixExpression::Type::FUNCTION_CALL, $a);
    }
  | postfixExpression[a] '(' argumentExpressionList[b] ')'
    {
      $$ = new PostfixExpression(PostfixExpression::Type::FUNCTION_CALL, $a, $b);
    }
  | postfixExpression[a] '.' IDENTIFIER[name]
    {
      $$ = new PostfixExpression(PostfixExpression::Type::MEMBER,$a,$name);
    }
  | postfixExpression[a] POINTER IDENTIFIER[name]
    {
      $$ = new PostfixExpression(PostfixExpression::Type::POINTER,$a,$name);
    }
  | postfixExpression[a] INCREMENT
    {
      $$ = new PostfixExpression(PostfixExpression::Type::INCREMENT,$a);
    }
  | postfixExpression[a] DECREMENT
    {
      $$ = new PostfixExpression(PostfixExpression::Type::DECREMENT,$a);
    }
  | primaryExpression
    {
      $$ = $primaryExpression;
    }
  ;

unaryExpression
  : INCREMENT unaryExpression[a]
    {
      $$ = new UnaryExpression(UnaryExpression::Operator::INCREMENT, $a);
    }
  | DECREMENT unaryExpression[a]
    {
      $$ = new UnaryExpression(UnaryExpression::Operator::DECREMENT, $a);
    }
  | unaryOperator castExpression[a]
    {
      $$ = new UnaryExpression($unaryOperator, $a);
    }
  | KEYWORD_SIZEOF unaryExpression[a]
    {
      $$ = new UnaryExpression(UnaryExpression::Operator::SIZEOF, $a);
    }
  | KEYWORD_SIZEOF '(' type_name ')'
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  | postfixExpression[a]
    {
      $$ = $a;
    }
  ;

unaryOperator
  : '&'
    {
      $$ = UnaryExpression::Operator::ADDRESS;
    }
  | '*'
    {
      $$ = UnaryExpression::Operator::DEREFERENCE;
    }
  | '+'
    {
      $$ = UnaryExpression::Operator::PLUS;
    }
  | '-'
    {
      $$ = UnaryExpression::Operator::MINUS;
    }
  | '~'
    {
      $$ = UnaryExpression::Operator::NOT;
    }
  | '!'
    {
      $$ = UnaryExpression::Operator::LOGICAL_NOT;
    }
  ;

castExpression
  : '(' type_name ')' castExpression
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  | unaryExpression
    {
      $$ = $unaryExpression;
    }
  ;

multiplicativeExpression
  : multiplicativeExpression[a] '*' castExpression[b]
    {
      $$ = new MultiplicativeExpression(MultiplicativeExpression::Type::MULTIPLY, $a, $b);
    }
  | multiplicativeExpression[a] '/' castExpression[b]
    {
      $$ = new MultiplicativeExpression(MultiplicativeExpression::Type::DIVIDE, $a, $b);
    }
  | multiplicativeExpression[a] '%' castExpression[b]
    {
      $$ = new MultiplicativeExpression(MultiplicativeExpression::Type::MODULO, $a, $b);
    }
  | castExpression
    {
      $$ = $castExpression;
    }
  ;

additiveExpression
  : additiveExpression[a] '+' multiplicativeExpression[b]
    {
      $$ = new AdditiveExpression(AdditiveExpression::Type::ADD, $a, $b);
    }
  | additiveExpression[a] '-' multiplicativeExpression[b]
    {
      $$ = new AdditiveExpression(AdditiveExpression::Type::SUBTRACT, $a, $b);
    }
  | multiplicativeExpression
    {
      $$ = $multiplicativeExpression;
    }
  ;

shiftExpression
  : shiftExpression[a] SHIFT_LEFT additiveExpression[b]
    {
      $$ = new ShiftExpression(ShiftExpression::Type::LEFT, $a, $b);
    }
  | shiftExpression[a] SHIFT_RIGHT additiveExpression[b]
    {
      $$ = new ShiftExpression(ShiftExpression::Type::RIGHT, $a, $b);
    }
  | additiveExpression
    {
      $$ = $additiveExpression;
    }
  ;

relationalExpression
  : relationalExpression[a] '<' shiftExpression[b]
    {
      $$ = new RelationalExpression(RelationalExpression::Type::LOWER, $a, $b);
    }
  | relationalExpression[a] '>' shiftExpression[b]
    {
      $$ = new RelationalExpression(RelationalExpression::Type::GREATER, $a, $b);
    }
  | relationalExpression[a] LOWER_EQUALS shiftExpression[b]
    {
      $$ = new RelationalExpression(RelationalExpression::Type::LOWER_EQUALS, $a, $b);
    }
  | relationalExpression[a] GREATER_EQUALS shiftExpression[b]
    {
      $$ = new RelationalExpression(RelationalExpression::Type::GREATER_EQUALS, $a, $b);
    }
  | shiftExpression
    {
      $$ = $shiftExpression;
    }
  ;

equalityExpression
  : equalityExpression[a] EQUALS relationalExpression[b]
    {
      $$ = new EqualityExpression(EqualityExpression::Type::EQUALS, $a, $b);
    }
  | equalityExpression[a] NOT_EQUALS relationalExpression[b]
    {
      $$ = new EqualityExpression(EqualityExpression::Type::NOT_EQUALS, $a, $b);
    }
  | relationalExpression
    {
      $$ = $relationalExpression;
    }
  ;

andExpression
  : andExpression[a] '&' equalityExpression[b]
    {
      $$ = new AndExpression($a, $b);
    }
  | equalityExpression
    {
      $$ = $equalityExpression;
    }
  ;

exclusiveOrExpression
  : exclusiveOrExpression[a] '^' andExpression[b]
    {
      $$ = new ExclusiveOrExpression($a, $b);
    }
  | andExpression
  {
    $$ = $andExpression;
  }
  ;

inclusiveOrExpression
  : inclusiveOrExpression[a] '|' exclusiveOrExpression[b]
    {
      $$ = new InclusiveOrExpression($a, $b);
    }
  | exclusiveOrExpression
    {
      $$ = $exclusiveOrExpression;
    }
  ;

logicalAndExpression
  : logicalAndExpression[a] AND inclusiveOrExpression[b]
    {
      $$ = new LogicalAndExpression($a, $b);
    }
  | inclusiveOrExpression
    {
      $$ = $inclusiveOrExpression;
    }
  ;

logicalOrExpression
  : logicalOrExpression[a] OR logicalAndExpression[b]
    {
      $$ = new LogicalOrExpression($a, $b);
    }
  | logicalAndExpression
    {
      $$ = $logicalAndExpression;
    }
  ;

conditionalExpression
  : logicalOrExpression[a] '?' expression[b] ':' conditionalExpression[c]
    {
      $$ = new ConditionalExpression($a, $b, $c);
    }
  | logicalOrExpression
    {
      $$ = $logicalOrExpression;
    }
  ;

argumentExpressionList
  : argumentExpressionList ',' assignmentExpression
    {
      $1->add($3);
      $$ = $1;
    }
  | assignmentExpression
    {
      $$ = new ArgumentExpressionList($1);
    }
  ;

assignmentExpression
  : unaryExpression[a] assignmentOperator assignmentExpression[b]
    {
      $$ = new AssignmentExpression($assignmentOperator,$a,$b);
    }
  | conditionalExpression
    {
      $$ = $conditionalExpression;
    }
  ;

assignmentOperator
  : '='
    {
      $$ = AssignmentExpression::Operator::ASSIGN;
    }
  | MULTIPLY_ASSIGN
    {
      $$ = AssignmentExpression::Operator::MULTIPLY_ASSIGN;
    }
  | DIVIDE_ASSIGN
    {
      $$ = AssignmentExpression::Operator::DIVIDE_ASSIGN;
    }
  | MODULO_ASSIGN
    {
      $$ = AssignmentExpression::Operator::MODULO_ASSIGN;
    }
  | ADD_ASSIGN
    {
      $$ = AssignmentExpression::Operator::ADD_ASSIGN;
    }
  | SUB_ASSIGN
    {
      $$ = AssignmentExpression::Operator::SUB_ASSIGN;
    }
  | SHIFT_LEFT_ASSIGN
    {
      $$ = AssignmentExpression::Operator::SHIFT_LEFT_ASSIGN;
    }
  | SHIFT_RIGHT_ASSIGN
    {
      $$ = AssignmentExpression::Operator::SHIFT_RIGHT_ASSIGN;
    }
  | AND_ASSIGN
    {
      $$ = AssignmentExpression::Operator::AND_ASSIGN;
    }
  | XOR_ASSIGN
    {
      $$ = AssignmentExpression::Operator::XOR_ASSIGN;
    }
  | OR_ASSIGN
    {
      $$ = AssignmentExpression::Operator::OR_ASSIGN;
    }
  ;

expression
  : expression ',' assignmentExpression
    {
      fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  | assignmentExpression
    {
      $$ = $assignmentExpression;
    }
  ;

constant_expression
  : conditionalExpression
    {
      fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  ;

initDeclaratorList
  : initDeclaratorList ',' initDeclarator
    {
      $1->add($initDeclarator);
      $$ = $1;
    }
  | initDeclarator
    {
      $$ = new InitDeclaratorList($initDeclarator);
    }
  ;

initDeclarator
  : storageClassDeclarationSpecifiers declarator '=' initializer
    {
      $$ = new InitDeclarator($storageClassDeclarationSpecifiers, $declarator, $initializer);
    }
  | storageClassDeclarationSpecifiers declarator
    {
      $$ = new InitDeclarator($storageClassDeclarationSpecifiers, $declarator);
    }
  ;

storageClassSpecifier
  : KEYWORD_TYPEDEF
    {
      $$ = new StorageClassSpecifier(StorageClassSpecifier::Type::TYPEDEF);
    }
  | KEYWORD_EXTERN
    {
      $$ = new StorageClassSpecifier(StorageClassSpecifier::Type::EXTERN);
    }
  | KEYWORD_STATIC
    {
      $$ = new StorageClassSpecifier(StorageClassSpecifier::Type::STATIC);
    }
  | KEYWORD_AUTO
    {
      $$ = new StorageClassSpecifier(StorageClassSpecifier::Type::AUTO);
    }
  | KEYWORD_REGISTER
    {
      $$ = new StorageClassSpecifier(StorageClassSpecifier::Type::REGISTER);
    }
  ;

typeSpecifier
  : KEYWORD_VOID
    {
      $$ = new TypeSpecifier(TypeSpecifier::Type::VOID);
    }
  | KEYWORD_CHAR
    {
      $$ = new TypeSpecifier(TypeSpecifier::Type::CHAR);
    }
  | KEYWORD_SHORT
    {
      $$ = new TypeSpecifier(TypeSpecifier::Type::SHORT);
    }
  | KEYWORD_INT
    {
      $$ = new TypeSpecifier(TypeSpecifier::Type::INT);
    }
  | KEYWORD_LONG
    {
      $$ = new TypeSpecifier(TypeSpecifier::Type::LONG);
    }
  | KEYWORD_FLOAT
    {
      $$ = new TypeSpecifier(TypeSpecifier::Type::FLOAT);
    }
  | KEYWORD_DOUBLE
    {
      $$ = new TypeSpecifier(TypeSpecifier::Type::DOUBLE);
    }
  | KEYWORD_SIGNED
    {
      $$ = new TypeSpecifier(TypeSpecifier::Type::SIGNED);
    }
  | KEYWORD_UNSIGNED
    {
      $$ = new TypeSpecifier(TypeSpecifier::Type::UNSIGNED);
    }
  | struct_or_union_specifier
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  | enum_specifier
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  //| TYPE_NAME
  | IDENTIFIER[name]
    {
      $$ = new TypeSpecifier($name);
    }
  ;

struct_or_union_specifier
  : struct_or_union IDENTIFIER '{' struct_declarationList '}'
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  | struct_or_union '{' struct_declarationList '}'
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  | struct_or_union IDENTIFIER
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  ;

struct_or_union
  : KEYWORD_STRUCT
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  | KEYWORD_UNION
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  ;

struct_declarationList
  : struct_declaration
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  | struct_declarationList struct_declaration
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  ;

struct_declaration
  : specifier_qualifier_list struct_declarator_list ';'
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  ;

specifier_qualifier_list
  : typeSpecifier specifier_qualifier_list
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  | typeSpecifier
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  | typeQualifier specifier_qualifier_list
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  | typeQualifier
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  ;

struct_declarator_list
  : struct_declarator
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  | struct_declarator_list ',' struct_declarator
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  ;

struct_declarator
  : declarator
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  | ':' constant_expression
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  | declarator ':' constant_expression
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  ;

enum_specifier
  : ENUM '{' enumerator_list '}'
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  | ENUM IDENTIFIER '{' enumerator_list '}'
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  | ENUM IDENTIFIER
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  ;

enumerator_list
  : enumerator
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  | enumerator_list ',' enumerator
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  ;

enumerator
  : IDENTIFIER
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  | IDENTIFIER '=' constant_expression
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  ;

typeQualifier
  : KEYWORD_CONST
    {
      $$ = new TypeQualifier(TypeQualifier::Type::CONST);
    }
  | KEYWORD_VOLATILE
    {
      $$ = new TypeQualifier(TypeQualifier::Type::VOLATILE);
    }
  ;

typeQualifierList
  : typeQualifierList typeQualifier
    {
      $1->add($typeQualifier);
      $$ = $1;
    }
  | typeQualifier
    {
      $$ = new TypeQualifierList($typeQualifier);
    }
  ;

declarator
  : pointer directDeclarator
    {
// TODO:
      $$ = new Declarator($directDeclarator);
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  | directDeclarator
    {
      $$ = new Declarator($directDeclarator);
    }
  ;

directDeclarator
  : IDENTIFIER[name]
    {
      $$ = new DirectDeclarator($name);
    }
  | '(' declarator ')'
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  | directDeclarator '[' constant_expression ']'
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  | directDeclarator '[' ']'
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  | directDeclarator '(' parameter_type_list ')'
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  | directDeclarator '(' identifier_list ')'
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  | directDeclarator '(' ')'
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  ;

pointer
  : '*'
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  | '*' typeQualifierList
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  | '*' pointer
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  | '*' typeQualifierList pointer
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  ;

parameter_type_list
  : parameter_list
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  | parameter_list ',' ELLIPSIS
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  ;

parameter_list
  : parameter_declaration
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  | parameter_list ',' parameter_declaration
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  ;

parameter_declaration
  : storageClassDeclarationSpecifiers declarator
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  | storageClassDeclarationSpecifiers abstract_declarator
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  | storageClassDeclarationSpecifiers
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  ;

identifier_list
  : identifier_list ',' IDENTIFIER
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  | IDENTIFIER
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  ;

type_name
  : specifier_qualifier_list
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  | specifier_qualifier_list abstract_declarator
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  ;

abstract_declarator
  : pointer
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  | direct_abstract_declarator
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  | pointer direct_abstract_declarator
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  ;

direct_abstract_declarator
  : '(' abstract_declarator ')'
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  | '[' ']'
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  | '[' constant_expression ']'
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  | direct_abstract_declarator '[' ']'
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  | direct_abstract_declarator '[' constant_expression ']'
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  | '(' ')'
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  | '(' parameter_type_list ')'
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  | direct_abstract_declarator '(' ')'
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  | direct_abstract_declarator '(' parameter_type_list ')'
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  ;

initializer
  : assignmentExpression
    {
      $$ = new Initializer($assignmentExpression);
    }
  | '{' initializer_list '}'
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  | '{' initializer_list ',' '}'
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  ;

initializer_list
  : initializer_list ',' initializer
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  | initializer
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  ;

statement
  : labeled_statement
    {
      $$ = $labeled_statement;
    }
  | compoundStatement
    {
      $$ = $compoundStatement;
    }
  | expressionStatement
    {
      $$ = $expressionStatement;
    }
  | selectionStatement
    {
      $$ = $selectionStatement;
    }
  | iterationStatement
    {
      $$ = $iterationStatement;
    }
  | jumpStatement
    {
      $$ = $jumpStatement;
    }
  | newStateStatement
    {
      $$ = $newStateStatement;
    }
  ;

labeled_statement
  : IDENTIFIER ':' statement
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  | KEYWORD_CASE constant_expression ':' statement
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  | KEYWORD_DEFAULT ':' statement
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  ;

compoundStatement
  : '{' '}'
    {
      $$ = new CompoundStatement();
    }
  | '{' declarationStatementList '}'
    {
      $$ = new CompoundStatement($declarationStatementList);
    }
  ;

expressionStatement
  : expression ';'
    {
      $$ = new ExpressionStatement($expression);
    }
  | ';'
    {
      $$ = new ExpressionStatement();
    }
  ;

selectionStatement
  : KEYWORD_IF '(' expression ')' statement
    {
      $$ = new IfStatement($expression,$statement);
    }
  | KEYWORD_IF '(' expression ')' statement[ifStatement] KEYWORD_ELSE statement[elseStatement]
    {
      $$ = new IfStatement($expression,$ifStatement,$elseStatement);
    }
  | KEYWORD_SWITCH '(' expression ')' statement
    {
      fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  ;

iterationStatement
  : KEYWORD_WHILE '(' expression ')' statement
    {
      fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  | KEYWORD_DO statement KEYWORD_WHILE '(' expression ')' ';'
    {
      $$ = new DoStatement($statement, $expression);
    }
  | KEYWORD_FOR '(' expressionStatement expressionStatement ')' statement
    {
      fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  | KEYWORD_FOR '(' expressionStatement expressionStatement expression ')' statement
    {
      fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  ;

jumpStatement
//  : GOTO IDENTIFIER ';'
  : KEYWORD_CONTINUE ';'
    {
      $$ = new JumpStatement(JumpStatement::Type::CONTINUE);
    }
  | KEYWORD_BREAK ';'
    {
      $$ = new JumpStatement(JumpStatement::Type::BREAK);
    }
  | KEYWORD_RETURN ';'
    {
      $$ = new JumpStatement(JumpStatement::Type::RETURN);
    }
  | KEYWORD_RETURN expression ';'
    {
      $$ = new JumpStatement(JumpStatement::Type::RETURN, $expression);
    }
  ;

newStateStatement
  : DEREFERENCE newStateStatementPrefixOperator IDENTIFIER[stateName] '(' newStateStatementOptions ')' ';'
    {
      NewStateStatement *newStateStatement = new NewStateStatement($stateName,$newStateStatementPrefixOperator,$newStateStatementOptions);
      ast.addStateTransition(currentStateName,newStateStatement);
      $$ = newStateStatement;
    }
  | DEREFERENCE newStateStatementPrefixOperator IDENTIFIER[stateName] '(' ')' ';'
    {
      NewStateStatement *newStateStatement = new NewStateStatement($stateName,$newStateStatementPrefixOperator);
      ast.addStateTransition(currentStateName,newStateStatement);
      $$ = newStateStatement;
    }
  | DEREFERENCE newStateStatementPrefixOperator IDENTIFIER[stateName] ';'
    {
      NewStateStatement *newStateStatement = new NewStateStatement($stateName,$newStateStatementPrefixOperator);
      ast.addStateTransition(currentStateName,newStateStatement);
      $$ = newStateStatement;
    }
  | DEREFERENCE IDENTIFIER[stateName] '(' newStateStatementOptions ')' ';'
    {
      NewStateStatement *newStateStatement = new NewStateStatement($stateName,$newStateStatementOptions);
      ast.addStateTransition(currentStateName,newStateStatement);
      $$ = newStateStatement;
    }
  | DEREFERENCE IDENTIFIER[stateName] '(' ')' ';'
    {
      NewStateStatement *newStateStatement = new NewStateStatement($stateName);
      ast.addStateTransition(currentStateName,newStateStatement);
      $$ = newStateStatement;
    }
  | DEREFERENCE IDENTIFIER[stateName] ';'
    {
      NewStateStatement *newStateStatement = new NewStateStatement($stateName);
      ast.addStateTransition(currentStateName,newStateStatement);
      $$ = newStateStatement;
    }
  ;

newStateStatementPrefixOperator
  : IDENTIFIER[name] ','
    {
      if      ($name == "push")
      {
        $$ = NewStateStatement::PrefixOperator::PUSH;
      }
      else if ($name == "reset")
      {
        $$ = NewStateStatement::PrefixOperator::RESET;
      }
      else
      {
        std::stringstream buffer;
        buffer << "unknown new state prefix operator '" << $name << "'";
        error(@$,buffer.str());
      }
    }
  ;

newStateStatementOptions
  : STRING[label] ',' IDENTIFIER[color] ',' number[lineWidth]
    {
      $$ = NewStateStatement::Options($label, $color, $lineWidth);
    }
  | STRING[label] ',' IDENTIFIER[color]
    {
      $$ = NewStateStatement::Options($label, $color);
    }
  | STRING[label]
    {
      $$ = NewStateStatement::Options($label);
    }
  ;

//function_definition
//: storageClassDeclarationSpecifiers declarator declarationList compoundStatement
//| storageClassDeclarationSpecifiers declarator compoundStatement
//| declarator declarationList compoundStatement
//| declarator compoundStatement
//;

// ---------------------------------------------------------------------

number:
    INTEGER[n]
    {
      $$ = (double)$n;
    }
  | FLOAT[n]
    {
      $$ = $n;
    }
  ;

%%

void FSM::Parser::error(const location_type &location, const std::string &message)
{
  std::stringstream buffer;
  buffer << inputFilePath << ":" << location.begin.line << ":" << location.begin.column << ": " << message;
  throw std::runtime_error(buffer.str());
}
