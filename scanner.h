/***********************************************************************\
*
* Contents: 
* Systems: all
*
\***********************************************************************/
#ifndef __SCANNER__
#define __SCANNER__

/****************************** Includes *******************************/

/****************** Conditional compilation switches *******************/

/***************************** Constants *******************************/

/***************************** Datatypes *******************************/

/***************************** Variables *******************************/

/***************************** Forwards ********************************/

/****************************** Macros *********************************/

/***************************** Functions *******************************/

/*
 Generated Flex class name is yyFlexLexer by default. If we want to use more flex-generated
 classes we should name them differently. See scanner.l prefix option.

 Unfortunately the implementation relies on this trick with redefining class name
 with a preprocessor macro. See GNU Flex manual, "Generating C++ Scanners" section
*/
#if ! defined(yyFlexLexerOnce)
  #undef yyFlexLexer
  #define yyFlexLexer FSM_FlexLexer // the trick with prefix; no namespace here :(
  #include <FlexLexer.h>
#endif

/* Scanner method signature is defined by this macro. Original yylex() returns int.
   Sinice Bison 3 uses symbol_type, we must change returned type. We also rename it
   to something sane, since you cannot overload return type.
*/
#undef YY_DECL
#define YY_DECL int FSM::Scanner::get_next_token()

namespace FSM
{

class Scanner : public yyFlexLexer
{
  public:
    virtual int get_next_token();

    void setLineNumber(uint startLineNb, uint endLineNb);

    std::string getFileName();
    uint getLine();
    uint getColumn();

  private:
};

}  // namespace FSM

#endif  // __SCANNER__

/* end of file */
