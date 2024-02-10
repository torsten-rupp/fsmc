#include <stdlib.h>
#include <stdio.h>

int main(int argc, const char *argv[])
{
  printf("FSM test4\n");

  #fsm demo
    *INIT
    {
      int a;
      static int a;
      uint16_t a;
      bool a;
      uint8_t* a;

      struct a b;
      union a b;

      int a = 0;
      const int a = 0;
      const uint16_t a = 0;
      static int a = 0;
      uint16_t a = 2+3;
      uint16_t a = 1*(2+3);
      bool a = 1 != 0;
      uint8_t* a = 0;
      uint8_t a = b[i];
      uint8_t a = &b[i];
      uint8_t a = &b[i].c[d];
      uint8_t* a = &b[i].c[d];

      a = 0;
      a = b.c;
      a = b->c;
      a = b();
      a = b[i];
      a = b.c.d;
      a = 1;
      a = 1+2;
      a = 1+2*3;
      a = (1+2)*3;

      foo();
      foo(a, b, c);
      a = sizeof(b);

      (void)a();

      {
        a = 123;
      }

      if (a)
      {
        -> INIT;
        return;
      }

      switch (a)
      {
        case b:
          break;
        default:
          break;
      }

      for (i = 0; i < n; i++)
      {
      }
      for (int i = 0; i < n; i++)
      {
      }

      while (a)
      {
      }

      do
      {
      }
      while (a);

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
