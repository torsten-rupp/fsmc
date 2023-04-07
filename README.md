# fsmc
Finite State Machine Compiler

The Finit State Machine Compiler compile a C/C++ source code with embedded
Finite State Machines (FSMs) into C/C++ code and optionally .dot files.

Usage:

```
fsmc [<options>] [<input file>]

Options
-o|--output <file path>               output file
-d|--dot-directory <directory path>   .dot file directory
-l|--log-function <log function>      log function to call on state change
-n|--state-stack-size <n>             state stack size
-a|--asserts                          generate asserts
```

Log function macros: @fromStateName@, @toStateName@

Source code format:

```
<C/C++ source>
#fsm <name>
<fsm definitions>
#end
<C/C++ source>
...
```
FSM definitions

```
[*]<state name>
{
  <statement list>
}
default
{
  <statement list>
}
...
```

The * mark the initial state. &lt;statement list&gt; are C statements to
execute in the given state.

To change to a new state use the syntax

```
-> [push|reset,]<new state name>[(<options>)];
-> [push|reset,]start[(<options>)];
-> [push|reset,]default[(<options>)];
-> pop[(<options>)];
```

```push``` save the current state on a stack and change to the new state.
```reset``` reset state stack.
```pop``` pop the last pushed state and change to that state.

Options
```
"<label>",<color>,<line width>
```

Example:

```
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <assert.h>

#define TRUE  1
#define FALSE 0

#define ON  TRUE
#define OFF FALSE

bool isFailure()
{
  return FALSE;
}

bool isPower(bool enabled)
{
  return FALSE;
}

void logStateChange(const char *fromStateName, const char *toStateName)
{
  printf("DEBUG: state change '%s' -> '%s'\n", fromStateName, toStateName);
}

int main(int argc, const char *argv[])
{
  printf("FSM traffic lights demo\n");

  for (uint i = 0; i < 10; i++)
  {
    #fsm traffic_lights
      *GREEN
      {
        if (isFailure() || isPower(OFF)) -> push,FAILURE;
        printf("green: go\n");
        -> YELLOW("",blue,2);
      }

      YELLOW
      {
        if (isFailure() || isPower(OFF)) -> push,FAILURE;
        printf("yellow: prepare for stop\n");
        -> RED("",blue,2);
      }

      RED
      {
        if (isFailure() || isPower(OFF)) -> push,FAILURE;
        printf("red: stop!\n");
        -> RED_YELLOW("",blue,2);
      }

      RED_YELLOW
      {
        if (isFailure() || isPower(OFF)) -> push,FAILURE;
        printf("red-yellow: ready for start\n");
        -> GREEN("",blue,2);
      }

      FAILURE
      {
        if (isPower(OFF)) -> OFF;
        -> BLINK_ON;
      }

      BLINK_ON
      {
        -> BLINK_OFF;
      }

      BLINK_OFF
      {
        if (!isFailure()) -> pop;
        -> BLINK_ON;
      }

      OFF
      {
        if (isPower(ON)) -> reset,RED;
      }
    #end

    sleep(2);
  }

  printf("FSM traffic lights demo done\n");

  return 0;
}
```

.dot output with Graphviz:

![Traffic lights example](examples/traffic_lights.png "Traffic Lights FSM")
