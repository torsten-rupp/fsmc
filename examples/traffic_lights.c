#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <assert.h>

#define TRUE  1
#define FALSE 0

#define ON  TRUE
#define OFF FALSE

bool isFailure()
{
  return FALSE;
}

bool isPower(bool enabled)
{
  return FALSE;
}

void logStateChange(const char *fromStateName, const char *toStateName)
{
  printf("DEBUG: state change '%s' -> '%s'\n", fromStateName, toStateName);
}

int main(int argc, const char *argv[])
{
  printf("FSM traffic lights demo\n");

  for (uint i = 0; i < 10; i++)
  {
    #fsm traffic_lights
      initially
      {
        printf("FSM step\n");
      }

      finally
      {
      }

      *GREEN
      {
        if (isFailure() || isPower(OFF)) -> push,FAILURE;
        printf("green: go\n");
        -> YELLOW("",blue,2);
      }

      YELLOW
      {
        if (isFailure() || isPower(OFF)) -> push,FAILURE;
        printf("yellow: prepare for stop\n");
        -> RED("",blue,2);
      }

      RED
      {
        if (isFailure() || isPower(OFF)) -> push,FAILURE;
        printf("red: stop!\n");
        -> RED_YELLOW("",blue,2);
      }

      RED_YELLOW
      {
        if (isFailure() || isPower(OFF)) -> push,FAILURE;
        printf("red-yellow: ready for start\n");
        -> GREEN("",blue,2);
      }

      FAILURE
      {
        if (isPower(OFF)) -> OFF;
        -> BLINK_ON;
      }

      BLINK_ON
      {
        -> BLINK_OFF;
      }

      BLINK_OFF
      {
        if (!isFailure()) -> pop;
        -> BLINK_ON;
      }

      OFF
      {
        if (isPower(ON)) -> reset,RED;
      }
    #end

    sleep(2);
  }

  printf("FSM traffic lights demo done\n");

  return 0;
}
