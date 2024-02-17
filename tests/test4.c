#include <stdlib.h>
#include <stdio.h>

int main(int argc, const char *argv[])
{
  printf("FSM test4\n");

  #fsm demo
    *INIT
    {
      #ifdef a
        aaa();
      #endif

      #ifdef b
        bbb1();
      #else
        bbb2();
      #endif

      #ifndef c
        ccc();
      #endif

      #ifndef d
        ddd1();
      #else
        ddd2();
      #endif

      -> DONE("abc",blue,4);
    }

    DONE
    {
      printf("foo");
     -> start;
    }

    default
    {

    }
  #end

  printf("FSM test4 done\n");

  return 0;
}
