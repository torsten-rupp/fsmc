#include <stdlib.h>
#include <stdio.h>

int main(int argc, const char *argv[])
{
  printf("FSM test1\n");

  #fsm demo
    *INIT
    {
      -> DONE;
    }

    DONE
    {
    }
  #end

  printf("FSM test1 done\n");

  return 0;
}
