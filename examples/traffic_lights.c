#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

#define TRUE  1
#define FALSE 0

bool failure()
{
  return FALSE;
}

int main(int argc, const char *argv[])
{
  printf("FSM traffic lights demo\n");

  for (uint i = 0; i < 10; i++)
  {
    #fsm traffic_lights
      *GREEN
      {
        if (failure()) -> push,FAILURE;
        printf("green: go\n");
        -> YELLOW;
      }

      YELLOW
      {
        if (failure()) -> push,FAILURE;
        printf("yellow: prepare for stop\n");
        -> RED;
      }

      RED
      {
        if (failure()) -> push,FAILURE;
        printf("red: stop!\n");
        -> RED_YELLOW;
      }

      RED_YELLOW
      {
        if (failure()) -> push,FAILURE;
        printf("red-yellow: ready for start\n");
        -> GREEN;
      }

      FAILURE
      {
        -> BLINK_ON;
      }

      BLINK_ON
      {
        if (!failure()) -> pop;
        -> BLINK_OFF;
      }

      BLINK_OFF
      {
        -> BLINK_ON;
      }
    #end

    sleep(2);
  }

  printf("FSM traffic lights demo done\n");

  return 0;
}
