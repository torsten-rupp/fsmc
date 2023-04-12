#include <stdlib.h>
#include <stdio.h>

int main(int argc, const char *argv[])
{
  printf("FSM test4\n");

  #fsm demo
    *INIT
    {
      int xyz;
      int xyz = 0;
      const int xyz = 0;
      const uint16_t xyz = 0;
      static int xyz = 0;
      uint16_t x = 2+3;
      uint16_t x = 1*(2+3);
      bool x = 1 != 0;

      xyz = 1;
      xyz = 1+2;
      xyz = 1+2*3;
      xyz = (1+2)*3;

      if (abc)
      {
        -> INIT;
        return;
      }

      {
        abc = 123;
      }
      
      a = b.c.d;
      foo(a, b, c);
      a = sizeof(b);
      (void)a;

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
