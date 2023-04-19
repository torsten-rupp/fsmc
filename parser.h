/***********************************************************************\
*
* Contents: 
* Systems: all
*
\***********************************************************************/

/****************************** Includes *******************************/

/****************** Conditional compilation switches *******************/

/***************************** Constants *******************************/

/***************************** Datatypes *******************************/

/***************************** Variables *******************************/

/***************************** Forwards ********************************/

/***************************** Functions *******************************/

#ifndef __PARSER__
#define __PARSER__

#include "scanner.h"
#include "location.h"
#include "ast.h"

namespace FSM
{

class Parser
{
  public:
    Parser(const std::string &inputFilePath, FSM::Scanner &scanner, FSM::AST &ast, bool debug);

    void parse();

  private:
};

}  // namespace FSM

#endif // __PARSER__

/* end of file */
