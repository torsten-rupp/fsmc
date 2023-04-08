#include <stdlib.h>
#include <stdio.h>

int main(int argc, const char *argv[])
{
  printf("FSM demo\n");

  #fsm demo
    *INIT
    {
/*      {
        abc = 123;
      }*/
//      const int xyz = 0;
//      const uint16_t xyz = 0;
//      int xyz = 0;
      static int xyz = 0;
//      bool x = 2+3;
//      bool x = 1*(2+3);
//      bool x = 1 != 0;
/*
          if (1)
          {
//            -> INITIAL_STATE;
//            return;
          }*/
      int xyz;
      xyz = 0;
      x = foo();
      -> STATE1("abc",blue,4);
    }


    EMPTY
    {
    }

    DONE
    {
      printf("xxx");
    -> EMPTY;
    }
    STATE1
    {
      -> STATE2;
    }

    STATE2
    {
      -> DONE;
    }

    STATE3
    {
      -> push,STATE4;
    }

    STATE4
    {
      -> pop;
    }

    default
    {
      -> EMPTY;
    }
  #end

  printf("FSM demo done\n");

  return 0;
}
