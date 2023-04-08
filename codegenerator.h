/***********************************************************************\
*
* Contents: C code generator
* Systems: all
*
\***********************************************************************/
#ifndef CODE_GENERATOR_H
#define CODE_GENERATOR_H

/****************************** Includes *******************************/
#include <iostream>
#include <vector>

#include "ast.h"

/****************** Conditional compilation switches *******************/

/***************************** Constants *******************************/

/***************************** Datatypes *******************************/
namespace FSM
{

class CodeGenerator
{
  public:
    CodeGenerator(std::ostream &output, uint indent, const std::string &logFunction)
      : output(output)
      , indent(indent)
      , logFunction(logFunction)
    {
    }

// TODO:const AST
  void generate(AST &ast);

  private:
    std::ostream      &output;
    uint              indent;
    const std::string &logFunction;
};

} // namespace FSM

/***************************** Variables *******************************/

/***************************** Forwards ********************************/

/***************************** Functions *******************************/

#endif // CODE_GENERATOR_H

/* end of file */
