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
    CodeGenerator(std::ostream &output)
      : output(output)
    {
    }

  void generate(const AST &ast, uint indent);

  private:
    std::ostream &output;
};

} // namespace FSM

/***************************** Variables *******************************/

/***************************** Forwards ********************************/

/***************************** Functions *******************************/

#endif // CODE_GENERATOR_H

/* end of file */
