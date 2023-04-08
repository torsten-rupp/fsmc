#include <stdlib.h>
#include <stdio.h>

int main(int argc, const char *argv[])
{
  printf("FSM test3\n");

  #fsm demo
    *INIT
    {
      -> STATE1("abc",blue,4);
    }

    STATE1
    {
      -> STATE2;
    }

    STATE2
    {
      -> DONE;
    }

    DONE
    {
     -> start;
    }

    default
    {
     -> start;
    }
  #end

  printf("FSM test3 done\n");

  return 0;
}
