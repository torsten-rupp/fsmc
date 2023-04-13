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
%type <SpecifierQualifierList*>            specifierQualifierList;
%type <TypeName*>                          typeName;
%type <AbstractDeclarator*>                abstractDeclarator;
%type <StorageClassDeclarationSpecifiers*> storageClassDeclarationSpecifiers;
%type <Declaration*>                       declaration;
%type <Declarator*>                        declarator;
%type <DirectDeclarator*>                  directDeclarator;
%type <Initializer*>                       initializer;
%type <InitDeclarator*>                    initDeclarator;
%type <InitDeclaratorList*>                initDeclaratorList;
%type <Declaration*>                       externalDeclaration;
%type <DeclarationStatement*>              declarationStatement;
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
%type <Expression*>                        constantExpression;

%type <Statement*>                         statement;
%type <Statement*>                         labeledStatement;
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
  | KEYWORD_DEFAULT
    {
      currentStateName = Identifier("default");
    }
    compoundStatement
    {
      ast.addState(new State(State::Type::DEFAULT,$compoundStatement));
    }
  ;

// ---------------------------------------------------------------------

declarationStatementList
  : declarationStatementList declarationStatement
    {
      $1->add($declarationStatement);
      $$ = $1;
    }
  | declarationStatement
    {
      $$ = new DeclarationStatementList($declarationStatement);
    }
  ;

declarationStatement
  : externalDeclaration
    {
      $$ = $externalDeclaration;
    }
  | statement
    {
      $$ = $statement;
    }
  ;

externalDeclaration
  : declaration
    {
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
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.begin.line,@$.begin.column);
      YYABORT;
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
  | KEYWORD_SIZEOF '(' typeName ')'
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.begin.line,@$.begin.column);
      YYABORT;
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
  : '(' typeName[a] ')' castExpression[b]
    {
      $$ = new CastExpression($a, $b);
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
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.begin.line,@$.begin.column);
      YYABORT;
    }
  | assignmentExpression
    {
      $$ = $assignmentExpression;
    }
  ;

constantExpression
  : conditionalExpression
    {
      $$ = $conditionalExpression;
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
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.begin.line,@$.begin.column);
      YYABORT;
    }
  | enum_specifier
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.begin.line,@$.begin.column);
      YYABORT;
    }
  //| typeName
  | IDENTIFIER[name]
    {
      $$ = new TypeSpecifier($name);
    }
  ;

struct_or_union_specifier
  : struct_or_union IDENTIFIER '{' struct_declarationList '}'
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.begin.line,@$.begin.column);
      YYABORT;
    }
  | struct_or_union '{' struct_declarationList '}'
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.begin.line,@$.begin.column);
      YYABORT;
    }
  | struct_or_union IDENTIFIER
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.begin.line,@$.begin.column);
      YYABORT;
    }
  ;

struct_or_union
  : KEYWORD_STRUCT
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.begin.line,@$.begin.column);
      YYABORT;
    }
  | KEYWORD_UNION
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.begin.line,@$.begin.column);
      YYABORT;
    }
  ;

struct_declarationList
  : struct_declaration
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.begin.line,@$.begin.column);
      YYABORT;
    }
  | struct_declarationList struct_declaration
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.begin.line,@$.begin.column);
      YYABORT;
    }
  ;

struct_declaration
  : specifierQualifierList struct_declarator_list ';'
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.begin.line,@$.begin.column);
      YYABORT;
    }
  ;

specifierQualifierList
  : specifierQualifierList typeSpecifier
    {
      $1->prepend($2);
      $$ = $1;
    }
  | typeSpecifier
    {
      $$ = new SpecifierQualifierList($1);
    }
  | specifierQualifierList typeQualifier
    {
      $1->prepend($2);
      $$ = $1;
    }
  | typeQualifier
    {
      $$ = new SpecifierQualifierList($1);
    }
  ;

struct_declarator_list
  : struct_declarator
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.begin.line,@$.begin.column);
      YYABORT;
    }
  | struct_declarator_list ',' struct_declarator
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.begin.line,@$.begin.column);
      YYABORT;
    }
  ;

struct_declarator
  : declarator
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.begin.line,@$.begin.column);
      YYABORT;
    }
  | ':' constantExpression
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.begin.line,@$.begin.column);
      YYABORT;
    }
  | declarator ':' constantExpression
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.begin.line,@$.begin.column);
      YYABORT;
    }
  ;

enum_specifier
  : ENUM '{' enumerator_list '}'
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.begin.line,@$.begin.column);
      YYABORT;
    }
  | ENUM IDENTIFIER '{' enumerator_list '}'
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.begin.line,@$.begin.column);
      YYABORT;
    }
  | ENUM IDENTIFIER
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.begin.line,@$.begin.column);
      YYABORT;
    }
  ;

enumerator_list
  : enumerator
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.begin.line,@$.begin.column);
      YYABORT;
    }
  | enumerator_list ',' enumerator
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.begin.line,@$.begin.column);
      YYABORT;
    }
  ;

enumerator
  : IDENTIFIER
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.begin.line,@$.begin.column);
      YYABORT;
    }
  | IDENTIFIER '=' constantExpression
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.begin.line,@$.begin.column);
      YYABORT;
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
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.begin.line,@$.begin.column);
      YYABORT;
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
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.begin.line,@$.begin.column);
      YYABORT;
    }
  | directDeclarator '[' constantExpression ']'
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.begin.line,@$.begin.column);
      YYABORT;
    }
  | directDeclarator '[' ']'
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.begin.line,@$.begin.column);
      YYABORT;
    }
  | directDeclarator '(' parameter_type_list ')'
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.begin.line,@$.begin.column);
      YYABORT;
    }
  | directDeclarator '(' identifier_list ')'
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.begin.line,@$.begin.column);
      YYABORT;
    }
  | directDeclarator '(' ')'
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.begin.line,@$.begin.column);
      YYABORT;
    }
  ;

pointer
  : '*'
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.begin.line,@$.begin.column);
      YYABORT;
    }
  | '*' typeQualifierList
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.begin.line,@$.begin.column);
      YYABORT;
    }
  | '*' pointer
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.begin.line,@$.begin.column);
      YYABORT;
    }
  | '*' typeQualifierList pointer
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.begin.line,@$.begin.column);
      YYABORT;
    }
  ;

parameter_type_list
  : parameter_list
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.begin.line,@$.begin.column);
      YYABORT;
    }
  | parameter_list ',' ELLIPSIS
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.begin.line,@$.begin.column);
      YYABORT;
    }
  ;

parameter_list
  : parameter_declaration
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.begin.line,@$.begin.column);
      YYABORT;
    }
  | parameter_list ',' parameter_declaration
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.begin.line,@$.begin.column);
      YYABORT;
    }
  ;

parameter_declaration
  : storageClassDeclarationSpecifiers declarator
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.begin.line,@$.begin.column);
      YYABORT;
    }
  | storageClassDeclarationSpecifiers abstractDeclarator
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.begin.line,@$.begin.column);
      YYABORT;
    }
  | storageClassDeclarationSpecifiers
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.begin.line,@$.begin.column);
      YYABORT;
    }
  ;

identifier_list
  : identifier_list ',' IDENTIFIER
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.begin.line,@$.begin.column);
      YYABORT;
    }
  | IDENTIFIER
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.begin.line,@$.begin.column);
      YYABORT;
    }
  ;

typeName
  : specifierQualifierList abstractDeclarator
    {
      $$ = new TypeName($specifierQualifierList,$abstractDeclarator);
    }
  | specifierQualifierList
    {
      $$ = new TypeName($specifierQualifierList);
    }
  ;

abstractDeclarator
  : pointer
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.begin.line,@$.begin.column);
      YYABORT;
    }
  | direct_abstractDeclarator
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.begin.line,@$.begin.column);
      YYABORT;
    }
  | pointer direct_abstractDeclarator
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.begin.line,@$.begin.column);
      YYABORT;
    }
  ;

direct_abstractDeclarator
  : '(' abstractDeclarator ')'
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.begin.line,@$.begin.column);
      YYABORT;
    }
  | '[' ']'
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.begin.line,@$.begin.column);
      YYABORT;
    }
  | '[' constantExpression ']'
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.begin.line,@$.begin.column);
      YYABORT;
    }
  | direct_abstractDeclarator '[' ']'
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.begin.line,@$.begin.column);
      YYABORT;
    }
  | direct_abstractDeclarator '[' constantExpression ']'
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.begin.line,@$.begin.column);
      YYABORT;
    }
  | '(' ')'
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.begin.line,@$.begin.column);
      YYABORT;
    }
  | '(' parameter_type_list ')'
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.begin.line,@$.begin.column);
      YYABORT;
    }
  | direct_abstractDeclarator '(' ')'
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.begin.line,@$.begin.column);
      YYABORT;
    }
  | direct_abstractDeclarator '(' parameter_type_list ')'
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.begin.line,@$.begin.column);
      YYABORT;
    }
  ;

initializer
  : assignmentExpression
    {
      $$ = new Initializer($assignmentExpression);
    }
  | '{' initializer_list '}'
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.begin.line,@$.begin.column);
      YYABORT;
    }
  | '{' initializer_list ',' '}'
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.begin.line,@$.begin.column);
      YYABORT;
    }
  ;

initializer_list
  : initializer_list ',' initializer
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.begin.line,@$.begin.column);
      YYABORT;
    }
  | initializer
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.begin.line,@$.begin.column);
      YYABORT;
    }
  ;

statement
  : labeledStatement
    {
      $$ = $labeledStatement;
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

labeledStatement
  : IDENTIFIER ':' statement
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.begin.line,@$.begin.column);
      YYABORT;
    }
  | KEYWORD_CASE constantExpression ':' statement
    {
      $$ = new LabeledStatement($constantExpression, $statement);
    }
  | KEYWORD_DEFAULT ':' statement
    {
      $$ = new LabeledStatement($statement);
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
      $$ = new SwitchStatement($expression,$statement);
    }
  ;

iterationStatement
  : KEYWORD_WHILE '(' expression ')' statement
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.begin.line,@$.begin.column);
      YYABORT;
    }
  | KEYWORD_DO statement KEYWORD_WHILE '(' expression ')' ';'
    {
      $$ = new DoStatement($statement, $expression);
    }
  | KEYWORD_FOR '(' declarationStatement[init] expressionStatement[condition] expression[increment] ')' statement
    {
      $$ = new ForStatement($init,$condition,$statement);
    }
  | KEYWORD_FOR '(' expressionStatement[init] expressionStatement[condition] ')' statement
    {
      $$ = new ForStatement($init,$condition,$statement);
    }
  | KEYWORD_FOR '(' expressionStatement[init] expressionStatement[condition] expression[increment] ')' statement
    {
      $$ = new ForStatement($init,$condition,$increment,$statement);
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
