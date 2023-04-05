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
%parse-param { const std::string &filePath }
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

//%type <Statement*>            statementOrBlock;
//%type <StructUnionSpecifier>  structOrUnion;
//%type <TypeAttributeList>     typeAttributeList;
//%type <TypeDeclaration>       type;
//%type <Statement*>            variable;
//%type <DeclarationList>         variableList;
//%type <Statement*>            labeledStatement;
//%type <Statement*>            expressionStatement;
//%type <Statement*>            selectionStatement;
//%type <Statement*>            iterationStatement;

%type <StorageClassSpecifier*>         storageClassSpecifier;
%type <TypeQualifier*>                 typeQualifier;
%type <TypeQualifierList>              typeQualifierList;
%type <TypeSpecifier*>                 typeSpecifier;
%type <DeclarationSpecifiers>          declarationSpecifiers;
%type <Declaration*>                   declaration;
%type <Declarator>                     declarator;
%type <DirectDeclarator>               directDeclarator;
%type <Initializer>                    initializer;
%type <InitDeclarator*>                initDeclarator;
%type <InitDeclaratorList>             initDeclaratorList;
%type <Declaration*>                   externalDeclaration;
%type <DeclarationStatement*>          declarationStatement;
%type <DeclarationStatementList>       declarationStatementList;

%type <Expression*>                    primaryExpression;
%type <Expression*>                    postfixExpression;
%type <Expression*>                    unaryExpression;
%type <UnaryExpression::Operator>      unaryOperator;
%type <Expression*>                    castExpression;
%type <Expression*>                    multiplicativeExpression;
%type <Expression*>                    additiveExpression;
%type <Expression*>                    shiftExpression;
%type <Expression*>                    relationalExpression;
%type <Expression*>                    equalityExpression;
%type <Expression*>                    andExpression;
%type <Expression*>                    exclusiveOrExpression;
%type <Expression*>                    inclusiveOrExpression;
%type <Expression*>                    logicalAndExpression;
%type <Expression*>                    logicalOrExpression;
%type <Expression*>                    conditionalExpression;
%type <Expression*>                    assignmentExpression;
%type <ArgumentExpressionList>         argumentExpressionList;
%type <AssignmentExpression::Operator> assignmentOperator;
%type <ExpressionList>                 expressionList;
%type <Expression*>                    expression;

%type <Statement*>                     statement;
%type <Statement*>                     labeled_statement;
%type <CompoundStatement*>             compoundStatement;
%type <ExpressionStatement*>           expressionStatement;
%type <Statement*>                     selectionStatement;
%type <Statement*>                     iterationStatement;
%type <NewStateStatement*>             newStateStatement;
%type <JumpStatement*>                 jumpStatement;
%type <Number>                         number;

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
  | '*' IDENTIFIER[stateName]
    {
      currentStateName = $stateName;
    }
    compoundStatement
    {
      ast.addState(State($stateName,$compoundStatement));
      ast.setStartState($stateName);
    }
  | IDENTIFIER[stateName]
    {
      currentStateName = $stateName;
    }
    compoundStatement
    {
      ast.addState(State($stateName,$compoundStatement));
    }
  | KEYWORD_DEFAULT
    {
      currentStateName = Identifier();
    }
    compoundStatement
    {
      ast.addState(State($compoundStatement));
    }
  ;

// ---------------------------------------------------------------------

declarationStatementList
  : declarationStatementList externalDeclaration
    {
      $1.add($externalDeclaration);
      $$ = $1;
    }
  | externalDeclaration
    {
      $$ = DeclarationStatementList($externalDeclaration);
    }
  | declarationStatementList statement
    {
      $1.add($statement);
      $$ = $1;
    }
  | statement
    {
      $$ = DeclarationStatementList($statement);
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

declarationSpecifiers
  : declarationSpecifiers storageClassSpecifier
    {
//      $2.prepend($1);
      $1.add($2);
      $$ = $1;
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  | storageClassSpecifier
    {
      $$ = DeclarationSpecifiers($storageClassSpecifier);
    }
//  : typeSpecifier declarationSpecifiers
  | declarationSpecifiers typeSpecifier
    {
//      $2.prepend($1);
      $1.add($2);
      $$ = $1;
    }
  | typeSpecifier
    {
      $$ = DeclarationSpecifiers($typeSpecifier);
    }
//  | typeQualifier declarationSpecifiers
  | declarationSpecifiers typeQualifier
    {
      $1.add($2);
      $$ = $1;
    }
  | typeQualifier
    {
      $$ = DeclarationSpecifiers($typeQualifier);
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
  | postfixExpression[a] '.' IDENTIFIER[b]
    {
      $$ = new PostfixExpression(PostfixExpression::Type::MEMBER,$a,$b);
    }
  | postfixExpression POINTER IDENTIFIER
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  | postfixExpression INCREMENT
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  | postfixExpression DECREMENT
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  | primaryExpression
    {
      $$ = $1;
    }
  ;

unaryExpression
  : INCREMENT unaryExpression
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  | DECREMENT unaryExpression
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  | unaryOperator castExpression
    {
      $$ = new UnaryExpression($unaryOperator, $castExpression);
    }
  | KEYWORD_SIZEOF unaryExpression
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  | KEYWORD_SIZEOF '(' type_name ')'
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  | postfixExpression
    {
      $$ = $postfixExpression;
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
      $1.add($3);
      $$ = $1;
    }
  | assignmentExpression
    {
      $$ = ArgumentExpressionList($1);
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
      $1.add($initDeclarator);
      $$ = $1;
    }
  | initDeclarator
    {
      $$ = InitDeclaratorList($initDeclarator);
    }
  ;

initDeclarator
  : declarationSpecifiers declarator '=' initializer
    {
      $$ = new InitDeclarator($declarationSpecifiers, $declarator, $initializer);
    }
  | declarationSpecifiers declarator
    {
      $$ = new InitDeclarator($declarationSpecifiers, $declarator);
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
      $1.add($typeQualifier);
      $$ = $1;
    }
  | typeQualifier
    {
      $$ = TypeQualifierList($typeQualifier);
    }
  ;

declarator
  : pointer directDeclarator
  | directDeclarator
    {
      $$ = Declarator($directDeclarator);
    }
  ;

directDeclarator
  : IDENTIFIER[name]
    {
      $$ = DirectDeclarator($name);
    }
  | '(' declarator ')'
  | directDeclarator '[' constant_expression ']'
  | directDeclarator '[' ']'
  | directDeclarator '(' parameter_type_list ')'
  | directDeclarator '(' identifier_list ')'
  | directDeclarator '(' ')'
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
  : declarationSpecifiers declarator
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  | declarationSpecifiers abstract_declarator
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  | declarationSpecifiers
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  ;

identifier_list
  : IDENTIFIER
    {
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__); asm("int3");
    }
  | identifier_list ',' IDENTIFIER
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
      $$ = Initializer($assignmentExpression);
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

newStateStatement:
    DEREFERENCE IDENTIFIER[stateName] '{' STRING[label] ',' IDENTIFIER[color] ',' number[lineWidth] '}' ';'
    {
      NewStateStatement *newStateStatement = new NewStateStatement($stateName,$label,$color,$lineWidth);
      ast.addStateTransition(currentStateName,*newStateStatement);
      $$ = newStateStatement;
    }
  | DEREFERENCE IDENTIFIER[stateName] '{' STRING[label] ',' IDENTIFIER[color] '}' ';'
    {
      NewStateStatement *newStateStatement = new NewStateStatement($stateName,$label,$color);
      ast.addStateTransition(currentStateName,*newStateStatement);
      $$ = newStateStatement;
    }
  | DEREFERENCE IDENTIFIER[stateName] '{' STRING[label] '}' ';'
    {
      NewStateStatement *newStateStatement = new NewStateStatement($stateName,$label);
      ast.addStateTransition(currentStateName,*newStateStatement);
      $$ = newStateStatement;
    }
  | DEREFERENCE IDENTIFIER[stateName] '{' '}' ';'
    {
      NewStateStatement *newStateStatement = new NewStateStatement($stateName);
      ast.addStateTransition(currentStateName,*newStateStatement);
      $$ = newStateStatement;
    }
  | DEREFERENCE IDENTIFIER[stateName] ';'
    {
      NewStateStatement *newStateStatement = new NewStateStatement($stateName);
      ast.addStateTransition(currentStateName,*newStateStatement);
      $$ = newStateStatement;
    }
  ;

//function_definition
//: declarationSpecifiers declarator declarationList compoundStatement
//| declarationSpecifiers declarator compoundStatement
//| declarator declarationList compoundStatement
//| declarator compoundStatement
//;

// ---------------------------------------------------------------------

number:
    INTEGER[n]
    {
      $$ = Number($n);
    }
  | FLOAT[n]
    {
      $$ = Number($n);
    }
  ;

%%

void FSM::Parser::error(const location_type &location, const std::string &message)
{
  std::stringstream buffer;
  buffer << filePath << ":" << location.begin.line << ":" << location.begin.column << ": " << message;
  throw std::runtime_error(buffer.str());
}
