/***********************************************************************\
*
* Contents: C code generator
* Systems: all
*
\***********************************************************************/
#ifndef __CODE_GENERATOR__
#define __CODE_GENERATOR__

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
  void generate(const AST &ast);

  private:
    std::ostream      &output;
    uint              startIndentCount;
    uint              indentCount;
    const std::string &logFunction;
};

} // namespace FSM

/***************************** Variables *******************************/

/***************************** Forwards ********************************/

/****************************** Macros *********************************/

/***************************** Functions *******************************/

#endif // __CODE_GENERATOR__

/* end of file */
