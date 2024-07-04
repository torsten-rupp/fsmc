#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

int main(int argc, const char *argv[])
{
  printf("FSM test4\n");

  int n = (argc > 1) ? atoi(argv[1]) : 3;

  bool quit = false;

  while (!quit)
  {
    #fsm demo
      initially
      {
        printf("initially\n");
        n--;
        if (n < 0)
        -> END;
      }

      finally
      {
      }

      *RUN
      {
        printf("RUN\n");
      }
      
      END
      {
        printf("END\n");
        quit = true;
      }

      default
      {

      }
    #end
  }

  printf("FSM test4 done\n");

  return 0;
}
