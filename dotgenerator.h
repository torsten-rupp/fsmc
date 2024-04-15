/***********************************************************************\
*
* Contents: .dot file generator
* Systems: all
*
\***********************************************************************/
#ifndef __DOT_GENERATOR__
#define __DOT_GENERATOR__

/****************************** Includes *******************************/
#include <iostream>
#include <set>
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
    DotGenerator(std::ostream &output, const std::set<std::string> &ignoreStates)
      : output(output)
      , ignoreStates(ignoreStates)
    {
    }

  void generate(const AST &ast);

  private:
    std::ostream                &output;
    const std::set<std::string> &ignoreStates;

    std::vector<const State*> getPushStates(const AST &ast, const State *state) const;
};

} // namespace FSM

/***************************** Variables *******************************/

/***************************** Forwards ********************************/

/***************************** Functions *******************************/

#endif // __DOT_GENERATOR__

/* end of file */
