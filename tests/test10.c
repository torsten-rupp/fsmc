#include <stdlib.h>
#include <stdio.h>

int main(int argc, const char *argv[])
{
  printf("FSM test10\n");

  #fsm demo
    initially
    {
      printf("initially\n");
    }

    *INIT
    {
      -> push,STATE1;
    }

    STATE1
    {
      -> reset,STATE2;
    }

    STATE2
    {
      -> push,STATE3;
    }

    STATE3
    {
      -> pop;
    }

    DONE
    {
    }

    default
    {
      -> DONE;
    }
  #end

  printf("FSM test10 done\n");

  return 0;
}
