/***********************************************************************\
*
* Contents: .dot file generator
* Systems: all
*
\***********************************************************************/
#ifndef DOT_GENERATOR_H
#define DOT_GENERATOR_H

/****************************** Includes *******************************/
#include <iostream>
#include <vector>

#include "ast.h"

/****************** Conditional compilation switches *******************/

/***************************** Constants *******************************/

/***************************** Datatypes *******************************/
namespace FSM
{

class DotGenerator
{
  public:
    DotGenerator(std::ostream &output)
      : output(output)
    {
    }

  void generate(const AST &ast);

  private:
    std::ostream &output;
};

} // namespace FSM

/***************************** Variables *******************************/

/***************************** Forwards ********************************/

/***************************** Functions *******************************/

#endif // DOT_GENERATOR_H

/* end of file */
