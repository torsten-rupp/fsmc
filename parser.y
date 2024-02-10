/***********************************************************************\
*
* Contents: parser
* Systems: all
*
\***********************************************************************/

// definitions
%code top {
}
%code requires {
}
%code provides {
}

%{
  static const std::string *inputFilePath;
  static FSM::Scanner      *scanner;
  static FSM::AST          *ast;

  static FSM::Identifier currentStateName;

  #define YYERROR_DETAILED

  #define YYDELETEVAL(v,n)
  #define YYDELETEPOSN(v,n)

  #define TRUE true
  #define FALSE false

  #if YYBTYACC
  #else
    #define YYVALID
  #endif
%}

// implementation
%{
  static int yylex()
  {
    return scanner->get_next_token();
  }
int yywrap(){
fprintf(stderr,"%s:%d: _\n",__FILE__,__LINE__);
    return 1;
}
  static void yyerror(const YYLTYPE *location, const char *s, ...)
  {
    va_list arguments;

    va_start(arguments,s);
    fprintf(stderr,"ERROR: ");
    vfprintf(stderr,s,arguments);
    va_end(arguments);
    fprintf(stderr," at %d:%d\n",location->first.line,location->first.column);
  }
%}

%locations

%union
{
  int                                    state;
  char                                   literal;
  char*                                  string;
  long                                   integer;
  double                                 float_;
  int                                    enum_;

  FSM::StorageClassSpecifier             *storageClassSpecifier;
  FSM::TypeQualifier                     *typeQualifier;
  FSM::TypeQualifierList                 *typeQualifierList;
  FSM::TypeSpecifier                     *typeSpecifier;
  FSM::TypeSpecifier                     *structUnionSpecifier;
  FSM::TypeSpecifier::Type               structUnion;
  FSM::SpecifierQualifierList            *specifierQualifierList;
  FSM::TypeName                          *typeName;
  FSM::AbstractDeclarator                *abstractDeclarator;
  FSM::StorageClassDeclarationSpecifiers *storageClassDeclarationSpecifiers;
  FSM::Declaration                       *declaration;
  FSM::DirectDeclarator                  *directDeclarator;
  FSM::Declarator                        *declarator;
  FSM::Pointer                           *pointer;
  FSM::Initializer                       *initializer;
  FSM::InitDeclarator                    *initDeclarator;
  FSM::InitDeclaratorList                *initDeclaratorList;
  FSM::Declaration                       *externalDeclaration;
  FSM::DeclarationStatement              *declarationStatement;
  FSM::DeclarationStatementList          *declarationStatementList;

  FSM::Expression                        *primaryExpression;
  FSM::Expression                        *postfixExpression;
  FSM::Expression                        *unaryExpression;
  FSM::UnaryExpression::Operator         unaryExpressionOperator;
  FSM::Expression                        *castExpression;
  FSM::Expression                        *multiplicativeExpression;
  FSM::Expression                        *additiveExpression;
  FSM::Expression                        *shiftExpression;
  FSM::Expression                        *relationalExpression;
  FSM::Expression                        *equalityExpression;
  FSM::Expression                        *andExpression;
  FSM::Expression                        *exclusiveOrExpression;
  FSM::Expression                        *inclusiveOrExpression;
  FSM::Expression                        *logicalAndExpression;
  FSM::Expression                        *logicalOrExpression;
  FSM::Expression                        *conditionalExpression;
  FSM::Expression                        *expression;
  FSM::Expression                        *assignmentExpression;
  FSM::ArgumentExpressionList            *argumentExpressionList;
  FSM::AssignmentExpression::Operator    assignmentExpressionOperator;
  FSM::Expression                        *constantExpression;

  FSM::Statement                         *statement;
  FSM::Statement                         *labeledStatement;
  FSM::CompoundStatement                 *compoundStatement;
  FSM::ExpressionStatement               *assignmentFunctionCallStatement;
  FSM::ExpressionStatement               *expressionStatement;
  FSM::Statement                         *selectionStatement;
  FSM::Statement                         *iterationStatement;
  FSM::JumpStatement                     *jumpStatement;
  FSM::NewStateStatement                 *newStateStatement;
  FSM::NewStateStatement::PrefixOperator newStateStatementPrefixOperator;
  FSM::NewStateStatement::Options        *newStateStatementOptions;
  double                                 number;
}

%token <kewword>     KEYWORD_FSM                      ;
%token <kewword>     KEYWORD_END                      ;

%token <keyword>     KEYWORD_IF                       ;
%token <keyword>     KEYWORD_ELSE                     ;
%token <keyword>     KEYWORD_FOR                      ;
%token <keyword>     KEYWORD_WHILE                    ;
%token <keyword>     KEYWORD_DO                       ;
%token <keyword>     KEYWORD_SWITCH                   ;
%token <keyword>     KEYWORD_CASE                     ;
%token <keyword>     KEYWORD_DEFAULT                  ;
%token <keyword>     KEYWORD_BREAK                    ;
%token <keyword>     KEYWORD_CONTINUE                 ;
%token <keyword>     KEYWORD_RETURN                   ;
%token <keyword>     KEYWORD_SIZEOF                   ;

%token <keyword>     KEYWORD_AUTO                     ;
%token <keyword>     KEYWORD_REGISTER                 ;
%token <keyword>     KEYWORD_STATIC                   ;
%token <keyword>     KEYWORD_EXTERN                   ;
%token <keyword>     KEYWORD_TYPEDEF                  ;

%token <keyword>     KEYWORD_VOID
%token <keyword>     KEYWORD_CHAR
%token <keyword>     KEYWORD_SHORT
%token <keyword>     KEYWORD_INT
%token <keyword>     KEYWORD_LONG
%token <keyword>     KEYWORD_FLOAT
%token <keyword>     KEYWORD_DOUBLE
%token <keyword>     KEYWORD_SIGNED
%token <keyword>     KEYWORD_UNSIGNED

%token <keyword>     KEYWORD_STRUCT                   ;
%token <keyword>     KEYWORD_UNION                    ;

%token <keyword>     KEYWORD_CONST                    ;
%token <keyword>     KEYWORD_VOLATILE                 ;

%token <void>        END TOKEN_EOF 0

%token <state>       STATE                        ;
%token <string>      TOKEN_IDENTIFIER         ;
%token <char>        TOKEN_CHAR               ;
%token <string>      TOKEN_STRING             ;
%token <float_>      TOKEN_FLOAT              ;
%token <integer>     TOKEN_INTEGER            ;
%token <enum_>       TOKEN_ENUM               ;

%token               TOKEN_MULTIPLY_ASSIGN
%token               TOKEN_DIVIDE_ASSIGN
%token               TOKEN_MODULO_ASSIGN
%token               TOKEN_ADD_ASSIGN
%token               TOKEN_SUB_ASSIGN
%token               TOKEN_SHIFT_LEFT_ASSIGN
%token               TOKEN_SHIFT_RIGHT_ASSIGN
%token               TOKEN_AND_ASSIGN
%token               TOKEN_XOR_ASSIGN
%token               TOKEN_OR_ASSIGN

%token               TOKEN_PLUS
%token               TOKEN_MINUS
%token               TOKEN_NEG
%token               TOKEN_NOT
%token               TOKEN_MULTIPLY
%token               TOKEN_DIVIDE

%token               TOKEN_INCREMENT
%token               TOKEN_DECREMENT
%token               TOKEN_POINTER
%token               TOKEN_EQUALS
%token               TOKEN_NOT_EQUALS
%token               TOKEN_LOWER
%token               TOKEN_GREATER
%token               TOKEN_LOWER_EQUALS
%token               TOKEN_GREATER_EQUALS
%token               TOKEN_AND
%token               TOKEN_OR
%token               TOKEN_XOR
%token               TOKEN_SHIFT_LEFT
%token               TOKEN_SHIFT_RIGHT

%token               TOKEN_ELLIPSIS

%left TOKEN_NEGATIVE
%left TOKEN_PLUS TOKEN_MINUS
%left TOKEN_MULTIPLY TOKEN_DIVIDE

%type <literal> '*' '(' ')', ',', ':';

%type <storageClassSpecifier>             storageClassSpecifier;
%type <typeQualifier>                     typeQualifier;
%type <typeQualifierList>                 typeQualifierList;
%type <typeSpecifier>                     typeSpecifier;
%type <structUnionSpecifier>              structUnionSpecifier;
%type <structUnion>                       structUnion;
%type <specifierQualifierList>            specifierQualifierList;
%type <typeName>                          typeName;
%type <abstractDeclarator>                abstractDeclarator;
%type <storageClassDeclarationSpecifiers> storageClassDeclarationSpecifiers;
%type <declaration>                       declaration;
%type <directDeclarator>                  directDeclarator;
%type <declarator>                        declarator;
%type <pointer>                           pointer;
%type <initializer>                       initializer;
%type <initDeclarator>                    initDeclarator;
%type <initDeclaratorList>                initDeclaratorList;
%type <declaration>                       externalDeclaration;
%type <declarationStatement>              declarationStatement;
%type <declarationStatementList>          declarationStatementList;

%type <expression>                        primaryExpression;
%type <expression>                        postfixExpression;
%type <expression>                        unaryExpression;
%type <unaryExpressionOperator>           unaryExpressionOperator;
%type <expression>                        castExpression;
%type <expression>                        multiplicativeExpression;
%type <expression>                        additiveExpression;
%type <expression>                        shiftExpression;
%type <expression>                        relationalExpression;
%type <expression>                        equalityExpression;
%type <expression>                        andExpression;
%type <expression>                        exclusiveOrExpression;
%type <expression>                        inclusiveOrExpression;
%type <expression>                        logicalAndExpression;
%type <expression>                        logicalOrExpression;
%type <expression>                        conditionalExpression;
%type <expression>                        expression;
%type <expression>                        assignmentExpression;
%type <argumentExpressionList>            argumentExpressionList;
%type <assignmentExpressionOperator>      assignmentExpressionOperator;
%type <expression>                        constantExpression;

%type <statement>                         statement;
%type <statement>                         labeledStatement;
%type <compoundStatement>                 compoundStatement;
%type <expressionStatement>               assignmentFunctionCallStatement;
%type <expressionStatement>               expressionStatement;
%type <statement>                         selectionStatement;
%type <statement>                         iterationStatement;
%type <jumpStatement>                     jumpStatement;
%type <newStateStatement>                 newStateStatement;
%type <newStateStatementPrefixOperator>   newStateStatementPrefixOperator;
%type <newStateStatementOptions>          newStateStatementOptions;
%type <float_>                            number;

%start start

%%

// ---------------------------------------------------------------------

start:
    {
      // empty
    }
  | start KEYWORD_FSM TOKEN_IDENTIFIER stateDefinitions KEYWORD_END
    {
      ast->setFSMName(std::string($3));
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
    {
    }
  | error
    {
      YYABORT;
    }
  ;

stateDefinition:
    '*' TOKEN_IDENTIFIER
    {
      currentStateName = FSM::Identifier($2);
    }
    compoundStatement
    {
      ast->addState(new FSM::State(@$,FSM::State::Type::START,currentStateName,$4));
    }
  | TOKEN_IDENTIFIER
    {
      currentStateName = FSM::Identifier($1);
    }
    compoundStatement
    {
      if (currentStateName == "initially")
      {
        ast->setInitially(@$,$3);
      }
      if (currentStateName == "finally")
      {
        ast->setFinally(@$,$3);
      }
      else
      {
        ast->addState(new FSM::State(@$,FSM::State::Type::CUSTOM,currentStateName,$3));
      }
    }
    // special case: default keyword
  | KEYWORD_DEFAULT
    {
      currentStateName = FSM::Identifier("default");
    }
    compoundStatement
    {
      ast->addState(new FSM::State(@$,FSM::State::Type::DEFAULT,$3));
    }
  ;

// ---------------------------------------------------------------------

declarationStatementList
  : declarationStatementList declarationStatement
    {
      $1->add($2);
      $$ = $1;
    }
  | declarationStatement
    {
      $$ = new FSM::DeclarationStatementList($1);
    }
  ;

declarationStatement
  : externalDeclaration
    {
      $$ = $1;
    }
  | statement
    {
      $$ = $1;
    }
  ;

externalDeclaration
  : declaration
    {
      $$ = $1;
    }
  // Note: reduced C standard: no function definitions
  // | function_definition
  ;

declaration
  : initDeclaratorList ';' [YYVALID;]
    {
      $$ = new FSM::Declaration($1);
    }
  ;

storageClassDeclarationSpecifiers
  : storageClassDeclarationSpecifiers storageClassSpecifier
    {
//      $2.prepend($1);
      $1->add($2);
      $$ = $1;
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.first.line,@$.first.column);
      YYABORT;
    }
  | storageClassSpecifier
    {
      $$ = new FSM::StorageClassDeclarationSpecifiers($1);
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
      $$ = new FSM::StorageClassDeclarationSpecifiers($1);
    }
//  | typeQualifier storageClassDeclarationSpecifiers
  | storageClassDeclarationSpecifiers typeQualifier
    {
      $1->add($2);
      $$ = $1;
    }
  | typeQualifier
    {
      $$ = new FSM::StorageClassDeclarationSpecifiers($1);
    }
  ;

// ---------------------------------------------------------------------

primaryExpression
  : TOKEN_IDENTIFIER
    {
      $$ = new FSM::PrimaryExpression(FSM::Identifier($1));
    }
  //| CONSTANT
  | TOKEN_INTEGER
    {
      $$ = new FSM::PrimaryExpression($1);
    }
  | TOKEN_STRING
    {
      $$ = new FSM::PrimaryExpression(std::string($1));
    }
  | '(' expression ')'
    {
      $$ = new FSM::PrimaryExpression($2);
    }
  ;

postfixExpression
  : postfixExpression '[' expression ']'
    {
      $$ = new FSM::PostfixExpression(FSM::PostfixExpression::Type::SUBSCRIPT, $1, $3);
    }
  | postfixExpression '(' ')'
    {
      $$ = new FSM::PostfixExpression(FSM::PostfixExpression::Type::FUNCTION_CALL, $1);
    }
  | postfixExpression '(' argumentExpressionList ')'
    {
      $$ = new FSM::PostfixExpression(FSM::PostfixExpression::Type::FUNCTION_CALL, $1, $3);
    }
  | postfixExpression '.' TOKEN_IDENTIFIER
    {
      $$ = new FSM::PostfixExpression(FSM::PostfixExpression::Type::MEMBER,$1,std::string($3));
    }
  | postfixExpression TOKEN_POINTER TOKEN_IDENTIFIER
    {
      $$ = new FSM::PostfixExpression(FSM::PostfixExpression::Type::POINTER,$1,std::string($3));
    }
  | postfixExpression TOKEN_INCREMENT
    {
      $$ = new FSM::PostfixExpression(FSM::PostfixExpression::Type::INCREMENT,$1);
    }
  | postfixExpression TOKEN_DECREMENT
    {
      $$ = new FSM::PostfixExpression(FSM::PostfixExpression::Type::DECREMENT,$1);
    }
  | primaryExpression
    {
      $$ = $1;
    }
  ;

unaryExpression
  : TOKEN_INCREMENT unaryExpression
    {
      $$ = new FSM::UnaryExpression(FSM::UnaryExpression::Operator::INCREMENT, $2);
    }
  | TOKEN_DECREMENT unaryExpression
    {
      $$ = new FSM::UnaryExpression(FSM::UnaryExpression::Operator::DECREMENT, $2);
    }
  | unaryExpressionOperator castExpression
    {
      $$ = new FSM::UnaryExpression($1, $2);
    }
  | KEYWORD_SIZEOF unaryExpression
    {
      $$ = new FSM::UnaryExpression(FSM::UnaryExpression::Operator::SIZEOF, $2);
    }
  | KEYWORD_SIZEOF '(' typeName ')'
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.first.line,@$.first.column);
      YYABORT;
    }
  | postfixExpression
    {
      $$ = $1;
    }
  ;

unaryExpressionOperator
  : '&'
    {
      $$ = FSM::UnaryExpression::Operator::ADDRESS;
    }
  | '*'
    {
      $$ = FSM::UnaryExpression::Operator::DEREFERENCE;
    }
  | '+'
    {
      $$ = FSM::UnaryExpression::Operator::PLUS;
    }
  | '-'
    {
      $$ = FSM::UnaryExpression::Operator::MINUS;
    }
  | '~'
    {
      $$ = FSM::UnaryExpression::Operator::NOT;
    }
  | '!'
    {
      $$ = FSM::UnaryExpression::Operator::LOGICAL_NOT;
    }
  ;

castExpression
  : '(' typeName ')' castExpression
    {
      $$ = new FSM::CastExpression($2, $4);
    }
  | unaryExpression
    {
      $$ = $1;
    }
  ;

multiplicativeExpression
  : multiplicativeExpression '*' castExpression
    {
      $$ = new FSM::MultiplicativeExpression(FSM::MultiplicativeExpression::Type::MULTIPLY, $1, $3);
    }
  | multiplicativeExpression '/' castExpression
    {
      $$ = new FSM::MultiplicativeExpression(FSM::MultiplicativeExpression::Type::DIVIDE, $1, $3);
    }
  | multiplicativeExpression '%' castExpression
    {
      $$ = new FSM::MultiplicativeExpression(FSM::MultiplicativeExpression::Type::MODULO, $1, $3);
    }
  | castExpression
    {
      $$ = $1;
    }
  ;

additiveExpression
  : additiveExpression '+' multiplicativeExpression
    {
      $$ = new FSM::AdditiveExpression(FSM::AdditiveExpression::Type::ADD, $1, $3);
    }
  | additiveExpression '-' multiplicativeExpression
    {
      $$ = new FSM::AdditiveExpression(FSM::AdditiveExpression::Type::SUBTRACT, $1, $3);
    }
  | multiplicativeExpression
    {
      $$ = $1;
    }
  ;

shiftExpression
  : shiftExpression TOKEN_SHIFT_LEFT additiveExpression
    {
      $$ = new FSM::ShiftExpression(FSM::ShiftExpression::Type::LEFT, $1, $3);
    }
  | shiftExpression TOKEN_SHIFT_RIGHT additiveExpression
    {
      $$ = new FSM::ShiftExpression(FSM::ShiftExpression::Type::RIGHT, $1, $3);
    }
  | additiveExpression
    {
      $$ = $1;
    }
  ;

relationalExpression
  : relationalExpression '<' shiftExpression
    {
      $$ = new FSM::RelationalExpression(FSM::RelationalExpression::Type::LOWER, $1, $3);
    }
  | relationalExpression '>' shiftExpression
    {
      $$ = new FSM::RelationalExpression(FSM::RelationalExpression::Type::GREATER, $1, $3);
    }
  | relationalExpression TOKEN_LOWER_EQUALS shiftExpression
    {
      $$ = new FSM::RelationalExpression(FSM::RelationalExpression::Type::LOWER_EQUALS, $1, $3);
    }
  | relationalExpression TOKEN_GREATER_EQUALS shiftExpression
    {
      $$ = new FSM::RelationalExpression(FSM::RelationalExpression::Type::GREATER_EQUALS, $1, $3);
    }
  | shiftExpression
    {
      $$ = $1;
    }
  ;

equalityExpression
  : equalityExpression TOKEN_EQUALS relationalExpression
    {
      $$ = new FSM::EqualityExpression(FSM::EqualityExpression::Type::EQUALS, $1, $3);
    }
  | equalityExpression TOKEN_NOT_EQUALS relationalExpression
    {
      $$ = new FSM::EqualityExpression(FSM::EqualityExpression::Type::NOT_EQUALS, $1, $3);
    }
  | relationalExpression
    {
      $$ = $1;
    }
  ;

andExpression
  : andExpression '&' equalityExpression
    {
      $$ = new FSM::AndExpression($1, $3);
    }
  | equalityExpression
    {
      $$ = $1;
    }
  ;

exclusiveOrExpression
  : exclusiveOrExpression '^' andExpression
    {
      $$ = new FSM::ExclusiveOrExpression($1, $3);
    }
  | andExpression
  {
    $$ = $1;
  }
  ;

inclusiveOrExpression
  : inclusiveOrExpression '|' exclusiveOrExpression
    {
      $$ = new FSM::InclusiveOrExpression($1, $3);
    }
  | exclusiveOrExpression
    {
      $$ = $1;
    }
  ;

logicalAndExpression
  : logicalAndExpression TOKEN_AND inclusiveOrExpression
    {
      $$ = new FSM::LogicalAndExpression($1, $3);
    }
  | inclusiveOrExpression
    {
      $$ = $1;
    }
  ;

logicalOrExpression
  : logicalOrExpression TOKEN_OR logicalAndExpression
    {
      $$ = new FSM::LogicalOrExpression($1, $3);
    }
  | logicalAndExpression
    {
      $$ = $1;
    }
  ;

conditionalExpression
  : logicalOrExpression '?' expression ':' expression
    {
      $$ = new FSM::ConditionalExpression($1, $3, $5);
    }
  | logicalOrExpression
    {
      $$ = $1;
    }
  ;

assignmentExpression
  : unaryExpression assignmentExpressionOperator assignmentExpression
    {
      $$ = new FSM::AssignmentExpression($2,$1,$3);
    }
  | conditionalExpression
    {
      $$ = $1;
    }
  ;

assignmentExpressionOperator
  : '='
    {
      $$ = FSM::AssignmentExpression::Operator::ASSIGN;
    }
  | TOKEN_MULTIPLY_ASSIGN
    {
      $$ = FSM::AssignmentExpression::Operator::MULTIPLY_ASSIGN;
    }
  | TOKEN_DIVIDE_ASSIGN
    {
      $$ = FSM::AssignmentExpression::Operator::DIVIDE_ASSIGN;
    }
  | TOKEN_MODULO_ASSIGN
    {
      $$ = FSM::AssignmentExpression::Operator::MODULO_ASSIGN;
    }
  | TOKEN_ADD_ASSIGN
    {
      $$ = FSM::AssignmentExpression::Operator::ADD_ASSIGN;
    }
  | TOKEN_SUB_ASSIGN
    {
      $$ = FSM::AssignmentExpression::Operator::SUB_ASSIGN;
    }
  | TOKEN_SHIFT_LEFT_ASSIGN
    {
      $$ = FSM::AssignmentExpression::Operator::SHIFT_LEFT_ASSIGN;
    }
  | TOKEN_SHIFT_RIGHT_ASSIGN
    {
      $$ = FSM::AssignmentExpression::Operator::SHIFT_RIGHT_ASSIGN;
    }
  | TOKEN_AND_ASSIGN
    {
      $$ = FSM::AssignmentExpression::Operator::AND_ASSIGN;
    }
  | TOKEN_XOR_ASSIGN
    {
      $$ = FSM::AssignmentExpression::Operator::XOR_ASSIGN;
    }
  | TOKEN_OR_ASSIGN
    {
      $$ = FSM::AssignmentExpression::Operator::OR_ASSIGN;
    }
  ;

constantExpression
  : expression
    {
      $$ = $1;
    }
  ;

expression
  : expression ',' assignmentExpression
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.first.line,@$.first.column);
      YYABORT;
    }
  | assignmentExpression
    {
      $$ = $1;
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
      $$ = new FSM::ArgumentExpressionList($1);
    }
  ;

initDeclaratorList
  : initDeclaratorList ',' initDeclarator
    {
      $1->add($3);
      $$ = $1;
    }
  | initDeclarator
    {
      $$ = new FSM::InitDeclaratorList($1);
    }
  ;

initDeclarator
  : storageClassDeclarationSpecifiers declarator '=' initializer
    {
      $$ = new FSM::InitDeclarator($1, $2, $4);
    }
  | storageClassDeclarationSpecifiers declarator
    {
      $$ = new FSM::InitDeclarator($1, $2);
    }
  ;

storageClassSpecifier
  : KEYWORD_TYPEDEF
    {
      $$ = new FSM::StorageClassSpecifier(FSM::StorageClassSpecifier::Type::TYPEDEF);
    }
  | KEYWORD_EXTERN
    {
      $$ = new FSM::StorageClassSpecifier(FSM::StorageClassSpecifier::Type::EXTERN);
    }
  | KEYWORD_STATIC
    {
      $$ = new FSM::StorageClassSpecifier(FSM::StorageClassSpecifier::Type::STATIC);
    }
  | KEYWORD_AUTO
    {
      $$ = new FSM::StorageClassSpecifier(FSM::StorageClassSpecifier::Type::AUTO);
    }
  | KEYWORD_REGISTER
    {
      $$ = new FSM::StorageClassSpecifier(FSM::StorageClassSpecifier::Type::REGISTER);
    }
  ;

typeSpecifier
  : KEYWORD_VOID
    {
      $$ = new FSM::TypeSpecifier(FSM::TypeSpecifier::Type::VOID);
    }
  | KEYWORD_CHAR
    {
      $$ = new FSM::TypeSpecifier(FSM::TypeSpecifier::Type::CHAR);
    }
  | KEYWORD_SHORT
    {
      $$ = new FSM::TypeSpecifier(FSM::TypeSpecifier::Type::SHORT);
    }
  | KEYWORD_INT
    {
      $$ = new FSM::TypeSpecifier(FSM::TypeSpecifier::Type::INT);
    }
  | KEYWORD_LONG
    {
      $$ = new FSM::TypeSpecifier(FSM::TypeSpecifier::Type::LONG);
    }
  | KEYWORD_FLOAT
    {
      $$ = new FSM::TypeSpecifier(FSM::TypeSpecifier::Type::FLOAT);
    }
  | KEYWORD_DOUBLE
    {
      $$ = new FSM::TypeSpecifier(FSM::TypeSpecifier::Type::DOUBLE);
    }
  | KEYWORD_SIGNED
    {
      $$ = new FSM::TypeSpecifier(FSM::TypeSpecifier::Type::SIGNED);
    }
  | KEYWORD_UNSIGNED
    {
      $$ = new FSM::TypeSpecifier(FSM::TypeSpecifier::Type::UNSIGNED);
    }
  | structUnionSpecifier
    {
      // nothing to do
    }
  | enum_specifier
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.first.line,@$.first.column);
      YYABORT;
    }
  //| typeName
  | TOKEN_IDENTIFIER
    {
      $$ = new FSM::TypeSpecifier(std::string($1));
    }
  ;

structUnionSpecifier
  : structUnion TOKEN_IDENTIFIER '{' struct_declarationList '}'
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.first.line,@$.first.column);
      YYABORT;
    }
  | structUnion '{' struct_declarationList '}'
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.first.line,@$.first.column);
      YYABORT;
    }
  | structUnion TOKEN_IDENTIFIER
    {
      $$ = new FSM::TypeSpecifier($1,std::string($2));
    }
  ;

structUnion
  : KEYWORD_STRUCT
    {
      $$ = FSM::TypeSpecifier::Type::STRUCT;
    }
  | KEYWORD_UNION
    {
      $$ = FSM::TypeSpecifier::Type::UNION;
    }
  ;

struct_declarationList
  : struct_declaration
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.first.line,@$.first.column);
      YYABORT;
    }
  | struct_declarationList struct_declaration
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.first.line,@$.first.column);
      YYABORT;
    }
  ;

struct_declaration
  : specifierQualifierList struct_declarator_list ';' [YYVALID;]
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.first.line,@$.first.column);
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
      $$ = new FSM::SpecifierQualifierList($1);
    }
  | specifierQualifierList typeQualifier
    {
      $1->prepend($2);
      $$ = $1;
    }
  | typeQualifier
    {
      $$ = new FSM::SpecifierQualifierList($1);
    }
  ;

struct_declarator_list
  : struct_declarator
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.first.line,@$.first.column);
      YYABORT;
    }
  | struct_declarator_list ',' struct_declarator
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.first.line,@$.first.column);
      YYABORT;
    }
  ;

struct_declarator
  : declarator
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.first.line,@$.first.column);
      YYABORT;
    }
  | ':' constantExpression
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.first.line,@$.first.column);
      YYABORT;
    }
  | declarator ':' constantExpression
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.first.line,@$.first.column);
      YYABORT;
    }
  ;

enum_specifier
  : TOKEN_ENUM '{' enumerator_list '}'
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.first.line,@$.first.column);
      YYABORT;
    }
  | TOKEN_ENUM TOKEN_IDENTIFIER '{' enumerator_list '}'
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.first.line,@$.first.column);
      YYABORT;
    }
  | TOKEN_ENUM TOKEN_IDENTIFIER
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.first.line,@$.first.column);
      YYABORT;
    }
  ;

enumerator_list
  : enumerator
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.first.line,@$.first.column);
      YYABORT;
    }
  | enumerator_list ',' enumerator
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.first.line,@$.first.column);
      YYABORT;
    }
  ;

enumerator
  : TOKEN_IDENTIFIER
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.first.line,@$.first.column);
      YYABORT;
    }
  | TOKEN_IDENTIFIER '=' constantExpression
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.first.line,@$.first.column);
      YYABORT;
    }
  ;

typeQualifier
  : KEYWORD_CONST
    {
      $$ = new FSM::TypeQualifier(FSM::TypeQualifier::Type::CONST);
    }
  | KEYWORD_VOLATILE
    {
      $$ = new FSM::TypeQualifier(FSM::TypeQualifier::Type::VOLATILE);
    }
  ;

typeQualifierList
  : typeQualifierList typeQualifier
    {
      $1->add($2);
      $$ = $1;
    }
  | typeQualifier
    {
      $$ = new FSM::TypeQualifierList($1);
    }
  ;

declarator
  : pointer directDeclarator
    {
      $$ = new FSM::Declarator($1, $2);
    }
  | directDeclarator
    {
      $$ = new FSM::Declarator($1);
    }
  ;

directDeclarator
  : TOKEN_IDENTIFIER
    {
      $$ = new FSM::DirectDeclarator(std::string($1));
    }
  | '(' declarator ')'
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.first.line,@$.first.column);
      YYABORT;
    }
  | directDeclarator '[' constantExpression ']'
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.first.line,@$.first.column);
      YYABORT;
    }
  | directDeclarator '[' ']'
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.first.line,@$.first.column);
      YYABORT;
    }
  | directDeclarator '(' parameter_type_list ')'
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.first.line,@$.first.column);
      YYABORT;
    }
  | directDeclarator '(' identifier_list ')'
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.first.line,@$.first.column);
      YYABORT;
    }
  | directDeclarator '(' ')'
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.first.line,@$.first.column);
      YYABORT;
    }
  ;

pointer
  : '*'
    {
      $$ = new FSM::Pointer();
    }
  | '*' typeQualifierList
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.first.line,@$.first.column);
      YYABORT;
    }
  | '*' pointer
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.first.line,@$.first.column);
      YYABORT;
    }
  | '*' typeQualifierList pointer
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.first.line,@$.first.column);
      YYABORT;
    }
  ;

parameter_type_list
  : parameter_list
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.first.line,@$.first.column);
      YYABORT;
    }
  | parameter_list ',' TOKEN_ELLIPSIS
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.first.line,@$.first.column);
      YYABORT;
    }
  ;

parameter_list
  : parameter_list ',' parameter_declaration
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.first.line,@$.first.column);
      YYABORT;
    }
  | parameter_declaration
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.first.line,@$.first.column);
      YYABORT;
    }
  ;

parameter_declaration
  : storageClassDeclarationSpecifiers declarator
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.first.line,@$.first.column);
      YYABORT;
    }
  | storageClassDeclarationSpecifiers abstractDeclarator
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.first.line,@$.first.column);
      YYABORT;
    }
  | storageClassDeclarationSpecifiers
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.first.line,@$.first.column);
      YYABORT;
    }
  ;

identifier_list
  : identifier_list ',' TOKEN_IDENTIFIER
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.first.line,@$.first.column);
      YYABORT;
    }
  | TOKEN_IDENTIFIER
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.first.line,@$.first.column);
      YYABORT;
    }
  ;

typeName
  : specifierQualifierList abstractDeclarator
    {
      $$ = new FSM::TypeName($1,$2);
    }
  | specifierQualifierList
    {
      $$ = new FSM::TypeName($1);
    }
  ;

abstractDeclarator
  : pointer
    {
      $$ = new FSM::AbstractDeclarator($1);
    }
  | direct_abstractDeclarator
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.first.line,@$.first.column);
      YYABORT;
    }
  | pointer direct_abstractDeclarator
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.first.line,@$.first.column);
      YYABORT;
    }
  ;

direct_abstractDeclarator
  : '(' abstractDeclarator ')'
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.first.line,@$.first.column);
      YYABORT;
    }
  | '[' ']'
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.first.line,@$.first.column);
      YYABORT;
    }
  | '[' constantExpression ']'
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.first.line,@$.first.column);
      YYABORT;
    }
  | direct_abstractDeclarator '[' ']'
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.first.line,@$.first.column);
      YYABORT;
    }
  | direct_abstractDeclarator '[' constantExpression ']'
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.first.line,@$.first.column);
      YYABORT;
    }
  | '(' ')'
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.first.line,@$.first.column);
      YYABORT;
    }
  | '(' parameter_type_list ')'
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.first.line,@$.first.column);
      YYABORT;
    }
  | direct_abstractDeclarator '(' ')'
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.first.line,@$.first.column);
      YYABORT;
    }
  | direct_abstractDeclarator '(' parameter_type_list ')'
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.first.line,@$.first.column);
      YYABORT;
    }
  ;

initializer
  : assignmentExpression
    {
      $$ = new FSM::Initializer($1);
    }
  | expression
    {
      $$ = new FSM::Initializer($1);
    }
  | '{' initializer_list '}'
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.first.line,@$.first.column);
      YYABORT;
    }
  | '{' initializer_list ',' '}'
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.first.line,@$.first.column);
      YYABORT;
    }
  ;

initializer_list
  : initializer_list ',' initializer
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.first.line,@$.first.column);
      YYABORT;
    }
  | initializer
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.first.line,@$.first.column);
      YYABORT;
    }
  ;

statement
  : labeledStatement
    {
      $$ = $1;
    }
  | compoundStatement
    {
      $$ = $1;
    }
//  | assignmentFunctionCallStatement
  | expressionStatement
    {
      $$ = $1;
    }/**/
  | selectionStatement
    {
      $$ = $1;
    }
  | iterationStatement
    {
      $$ = $1;
    }
  | jumpStatement
    {
      $$ = $1;
    }
  | newStateStatement
    {
      $$ = $1;
    }
  ;

labeledStatement
  : TOKEN_IDENTIFIER ':' statement
    {
fprintf(stderr,"%s:%d: at %d,%d\n",__FILE__,__LINE__,@$.first.line,@$.first.column);
      YYABORT;
    }
  | KEYWORD_CASE constantExpression ':' statement
    {
      $$ = new FSM::LabeledStatement(@$,$2, $4);
    }
  | KEYWORD_DEFAULT ':' statement
    {
      $$ = new FSM::LabeledStatement(@$,$3);
    }
  ;

compoundStatement
  : '{' declarationStatementList '}' [YYVALID;]
    {
      $$ = new FSM::CompoundStatement(@$,$2);
    }
  | '{' '}' [YYVALID;]
    {
      $$ = new FSM::CompoundStatement(@$);
    }
  ;

assignmentFunctionCallStatement
  : assignmentExpression ';' [YYVALID;]
    {
      $$ = new FSM::ExpressionStatement(@$,$1);
    }
  | expression ';' [YYVALID;]
    {
      $$ = new FSM::ExpressionStatement(@$,$1);
    }
  ;

expressionStatement
  : expression ';' [YYVALID;]
    {
      $$ = new FSM::ExpressionStatement(@$,$1);
    }
  ;

selectionStatement
  : KEYWORD_IF '(' expression ')' statement
    {
      $$ = new FSM::IfStatement(@$,$3,$5);
    }
  | KEYWORD_IF '(' expression ')' statement KEYWORD_ELSE statement
    {
      $$ = new FSM::IfStatement(@$,$3,$5,$7);
    }
  | KEYWORD_SWITCH '(' expression ')' statement
    {
      $$ = new FSM::SwitchStatement(@$,$3,$5);
    }
  ;

iterationStatement
  : KEYWORD_WHILE '(' expression ')' statement
    {
      $$ = new FSM::WhileStatement(@$,$3, $5);
    }
  | KEYWORD_DO statement KEYWORD_WHILE '(' expression ')' ';' [YYVALID;]
    {
      $$ = new FSM::DoStatement(@$,$2, $5);
    }
  | KEYWORD_FOR '(' declarationStatement expressionStatement expression ')' statement
    {
      $$ = new FSM::ForStatement(@$,$3,$4,$5,$7);
    }
  | KEYWORD_FOR '(' expressionStatement expressionStatement ')' statement
    {
      $$ = new FSM::ForStatement(@$,$3,$4,$6);
    }
  | KEYWORD_FOR '(' expressionStatement expressionStatement expression ')' statement
    {
      $$ = new FSM::ForStatement(@$,$3,$4,$5,$7);
    }
  ;

jumpStatement
//  : GOTO TOKEN_IDENTIFIER ';'
  : KEYWORD_CONTINUE ';' [YYVALID;]
    {
      $$ = new FSM::JumpStatement(@$,FSM::JumpStatement::Type::CONTINUE);
    }
  | KEYWORD_BREAK ';' [YYVALID;]
    {
      $$ = new FSM::JumpStatement(@$,FSM::JumpStatement::Type::BREAK);
    }
  | KEYWORD_RETURN ';' [YYVALID;]
    {
      $$ = new FSM::JumpStatement(@$,FSM::JumpStatement::Type::RETURN);
    }
  | KEYWORD_RETURN expression ';' [YYVALID;]
    {
      $$ = new FSM::JumpStatement(@$,FSM::JumpStatement::Type::RETURN, $2);
    }
  ;

newStateStatement
  : TOKEN_POINTER newStateStatementPrefixOperator TOKEN_IDENTIFIER '(' newStateStatementOptions ')' ';' [YYVALID;]
    {
      FSM::NewStateStatement *newStateStatement = new FSM::NewStateStatement(@$,std::string($3),$2,$5);
      ast->addStateTransition(currentStateName,newStateStatement);
      $$ = newStateStatement;
    }
  | TOKEN_POINTER newStateStatementPrefixOperator TOKEN_IDENTIFIER '(' ')' ';' [YYVALID;]
    {
      FSM::NewStateStatement *newStateStatement = new FSM::NewStateStatement(@$,std::string($3),$2);
      ast->addStateTransition(currentStateName,newStateStatement);
      $$ = newStateStatement;
    }
  | TOKEN_POINTER newStateStatementPrefixOperator TOKEN_IDENTIFIER ';' [YYVALID;]
    {
      FSM::NewStateStatement *newStateStatement = new FSM::NewStateStatement(@$,std::string($3),$2);
      ast->addStateTransition(currentStateName,newStateStatement);
      $$ = newStateStatement;
    }
  | TOKEN_POINTER TOKEN_IDENTIFIER '(' newStateStatementOptions ')' ';' [YYVALID;]
    {
      FSM::NewStateStatement *newStateStatement = new FSM::NewStateStatement(@$,std::string($2),$4);
      ast->addStateTransition(currentStateName,newStateStatement);
      $$ = newStateStatement;
    }
  | TOKEN_POINTER TOKEN_IDENTIFIER '(' ')' ';' [YYVALID;]
    {
      FSM::NewStateStatement *newStateStatement = new FSM::NewStateStatement(@$,std::string($2));
      ast->addStateTransition(currentStateName,newStateStatement);
      $$ = newStateStatement;
    }
  | TOKEN_POINTER TOKEN_IDENTIFIER ';' [YYVALID;]
    {
      FSM::NewStateStatement *newStateStatement = new FSM::NewStateStatement(@$,std::string($2));
      ast->addStateTransition(currentStateName,newStateStatement);
      $$ = newStateStatement;
    }
  ;

newStateStatementPrefixOperator
  : TOKEN_IDENTIFIER ','
    {
      if      (std::string($1) == "push")
      {
        $$ = FSM::NewStateStatement::PrefixOperator::PUSH;
      }
      else if (std::string($1) == "reset")
      {
        $$ = FSM::NewStateStatement::PrefixOperator::RESET;
      }
      else
      {
        std::stringstream buffer;
        buffer << "unknown new state prefix operator '" << $1 << "'";
        yyerror(&@$,buffer.str().c_str());
      }
    }
  ;

newStateStatementOptions
  : TOKEN_STRING ',' TOKEN_IDENTIFIER ',' number
    {
      $$ = new FSM::NewStateStatement::Options(std::string($1), std::string($3), $5);
    }
  | TOKEN_STRING ',' TOKEN_IDENTIFIER
    {
      $$ = new FSM::NewStateStatement::Options(std::string($1), std::string($3));
    }
  | TOKEN_STRING
    {
      $$ = new FSM::NewStateStatement::Options(std::string($1));
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
    TOKEN_INTEGER
    {
      $$ = (double)($1);
    }
  | TOKEN_FLOAT
    {
      $$ = $1;
    }
  ;

%%

#if 0
void FSM::Parser::error(const location_type &location, const std::string &message)
{
  std::stringstream buffer;
  buffer << inputFilePath << ":" << location.begin.line << ":" << location.begin.column << ": " << message;
  throw std::runtime_error(buffer.str());
}
#endif

//}
