/***********************************************************************\
*
* Contents: scanner location tracking
* Systems: all
*
\***********************************************************************/
#ifndef __LOCATION__
#define __LOCATION__

/****************************** Includes *******************************/
#include <stdlib.h>
#include <stdint.h>

/****************** Conditional compilation switches *******************/

/***************************** Constants *******************************/
#define YYLTYPE Location
#define YYLTYPE_IS_DEFINED

#define YYLLOC_DEFAULT(location, rhs, n) \
  do \
  { \
    if (n == 0) \
    { \
      (location).first.line   = YYRHSLOC(rhs, 0).last.line; \
      (location).first.column = YYRHSLOC(rhs, 0).last.column; \
      (location).last.line    = YYRHSLOC(rhs, 0).last.line; \
      (location).last.column  = YYRHSLOC(rhs, 0).last.column; \
    } \
    else \
    { \
      (location).first.line   = YYRHSLOC(rhs, 1).first.line; \
      (location).first.column = YYRHSLOC(rhs, 1).first.column; \
      (location).last.line    = YYRHSLOC(rhs, n).last.line; \
      (location).last.column  = YYRHSLOC(rhs, n).last.column; \
    } \
  } while (0)

/***************************** Datatypes *******************************/
class Location
{
  public:
    struct
    {
      uint line;
      uint column;
    } first;
    struct
    {
      uint line;
      uint column;
    } last;

    Location(uint firstLine,
             uint firstColumn,
             uint lastLine,
             uint lastColumn
            )
    {
      first.line   = firstLine;
      first.column = firstColumn;
      last.line    = lastLine;
      last.column  = lastColumn;
    }
    
    Location() = default;
};

/***************************** Variables *******************************/

/***************************** Forwards ********************************/

/****************************** Macros *********************************/

/***************************** Functions *******************************/

#endif // __LOCATION__

/* end of file */
