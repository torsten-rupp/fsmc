# fsmc
Finite State Machine Compiler

The Finit State Machine Compiler compile a C/C++ source code with embedded
Finite State Machines (FSMs) into C/C++ code and optionally .dot files.

Usage:

fsmc [<options>] [<input file>]

Options
-o|--output <file path>               output file
-d|--dot-directory <directory path>   .dot file directory
-l|--log-function <log function>      log function to call on state change
-n|--state-stack-size <n>             state stack size
-a|--asserts                          generate asserts

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
#include <unistd.h>

int main(int argc, const char *argv[])
{
  printf("FSM traffic lights demo\n");

  for (uint i = 0; i < 10; i++)
  {
    #fsm traffic_lights
      *GREEN
      {
        if (failure) -> push,FAILURE;
        printf("green: go\n");
        -> YELLOW;
      }

      YELLOW
      {
        if (failure) -> push,FAILURE;
        printf("yellow: prepare for stop\n");
        -> RED;
      }

      RED
      {
        if (failure) -> push,FAILURE;
        printf("red: stop!\n");
        -> RED_YELLOW;
      }

      RED_YELLOW
      {
        if (failure) -> push,FAILURE;
        printf("red-yellow: ready for start\n");
        -> GREEN;
      }

      FAILURE
      {
        -> BLINK_ON
      }

      BLINK_ON
      {
        -> BLINK_OFF
      }

      BLINK_OFF
      {
        if (!failure) -> pop;
        -> BLINK_ON
      }
    #end

    sleep(2);
  }

  printf("FSM traffic lights demo done\n");

  return 0;
}
```
