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
   /** indent
   * @param output output stream
   * @param startIndentCount start indention
   * @param indentCount indention
   * @param logFunction log function with macros
   */
   CodeGenerator(std::ostream &output, uint startIndentCount, uint indentCount, const std::string &logFunction)
      : output(output)
      , startIndentCount(startIndentCount)
      , indentCount(indentCount)
      , logFunction(logFunction)
    {
    }

// TODO:const AST
  void generate(AST &ast);

  private:
    std::ostream      &output;
    uint              startIndentCount;
    uint              indentCount;
    const std::string &logFunction;
};

} // namespace FSM

/***************************** Variables *******************************/

/***************************** Forwards ********************************/

/***************************** Functions *******************************/

#endif // CODE_GENERATOR_H

/* end of file */
