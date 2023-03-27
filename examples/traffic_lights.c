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
        printf("green: go\n");
        -> YELLOW;
      }

      YELLOW
      {
        printf("yellow: prepare for stop\n");
        -> RED;
      }

      RED
      {
        printf("red: stop!\n");
        -> RED_YELLOW;
      }
      
      RED_YELLOW
      {
        printf("red-yellow: ready for start\n");
        -> GREEN;
      }
    #end
    
    sleep(2);
  }

  printf("FSM traffic lights demo done\n");

  return 0;
}
