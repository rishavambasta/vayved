/*********************************
 * Author: Rishav Ambasta
 * Date  : 07-June-2016
 * *******************************/

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include "vayved.h"
#include "color.h"


FSM_STATES current_state = OFFLINE;
bool canBlink;

//To keep control of the child process
//and kill it when required
int pid_of_openVPN_child_process;

//start-off by turning the LED off
Color currentColor = BLACK;



/************************************************/
/* This is the main thread of execution
   It realizes a finite state machine of the
  possible states the router can be in         /
/************************************************/

void* vayved_thread (void* threadId)
{
  FSM_STATES next_state;


  while (1)
    {
      switch (current_state)
        {
        case OFFLINE:
          changeColor(RED);
          canBlink = false;
          next_state  = TRYING_TO_GO_ONLINE;
          break;



        case TRYING_TO_GO_ONLINE:
          changeColor(BLUE);
          canBlink = true;
          if (internetConnected())
            if (tunnelExists())
              next_state = ONLINE_TUNNELLED;
            else

              next_state = ONLINE_UNTUNNELLED;
          else
            next_state = OFFLINE;
          break;



        case ONLINE_UNTUNNELLED:
          canBlink = false;
          changeColor(BLUE);
          if (internetConnected())
            next_state = TRYING_TO_CREATE_TUNNEL;
          else
            next_state = OFFLINE;
          break;




        case TRYING_TO_CREATE_TUNNEL:
          changeColor(VIOLET);
          canBlink = true;

          if (internetConnected())
            if (tunnelExists())
              next_state = ONLINE_TUNNELLED;
            else
              {
                if (createTunnel() == 1) //Fire OpenVPN Client
                  {
                    printf ("\nTunnel created\n");
                    fflush(stdout);
                    next_state = ONLINE_TUNNELLED;
                  }
                else{
                    printf ("\nTunnel NOT created");
                    fflush(stdout);
                    next_state = ONLINE_UNTUNNELLED;
                  }
              }
          else
            next_state = OFFLINE;
          break;



        case ONLINE_TUNNELLED:
          changeColor(VIOLET);
          canBlink = false;

          if (tunnelExists())
            {
              if (internetConnected())
                next_state = ONLINE_TUNNELLED;
              else
                {
                  terminate_tunnel();
                  next_state = OFFLINE;
                }
            }
          else
            {
              terminate_tunnel(); //Just a sanity step, we dont want two openVPN sessions in any case
              next_state = OFFLINE;
            }
          break;
        }

      sleep(INTER_POLL_DELAY);

      current_state = next_state; // Transition of state
    }
  return (void*) NULL;
}


/************************************************
 * Thread handling the bliking action of LEDs
 * when applicable
 * *********************************************/
void* ledBlinkerThread(void *threadID)  //trivial right now
{
  Color backupColor;
  while(true)
    {
      backupColor = currentColor;

      if (canBlink)
        {
          changeColor(BLACK);
          usleep(50000);
        }
      if (currentColor == BLACK)
        changeColor(backupColor);
      usleep(50000);

    }
  return (void*) NULL;
}



/*************************************************
 * SIGNAL handler
 * ***********************************************/
void exitSignalHandler (int signum)
{
  printf ("\nExiting with little housekeeping before we go..\n");
  resetLeds();
  closeLedDeviceFiles();
  exit(0);
}





void terminate_tunnel()
{
  int retry = 0;
  printf ("\nDeleting tunnel\n");
  fflush(stdout);
  kill(pid_of_openVPN_child_process, SIGKILL); //TODO:check for boundary cases

  while (retry < 500)
    {
      if (tunnelExists())
        {
          sleep(1);
          retry++;
        }
      else
        return;
    }
}

bool tunnelExists()
{
  if( access( tun0_device_file, F_OK ) != -1 )
    return true;
  else
    return false;
}

//Return value 1 means created successfully
//0 means failure
int createTunnel()
{
  int retry = 0;
  pid_of_openVPN_child_process = fork();
  if (pid_of_openVPN_child_process == -1)
    return 0;

  if (pid_of_openVPN_child_process == 0)
    {
      execlp (openVPN_binary,openVPN_params,ovpn_file_path);
    }
  else
    {
      while (retry < 500)
        {
          if (tunnelExists())
            return 1;
          else
            sleep(1);
          retry++;
        }
      return 0;
    }
}


int main ()
{

  void *end;

  pthread_t vayvedThread_t,bThread_t;
  int vayvedThread,bThread;

  current_state = OFFLINE;
  currentColor = BLACK;
  openLedDeviceFiles();
  resetLeds();

  bThread = pthread_create(&bThread_t, NULL,ledBlinkerThread, NULL);
  if (bThread)
    {
      printf("ERROR; return code from pthread_create() is %d\n", bThread);
      exit(-1);
    }

  vayvedThread = pthread_create(&vayvedThread_t, NULL, vayved_thread, NULL);
  if (vayvedThread)
    {
      printf("ERROR: return code from pthread_create() is %d\n", vayvedThread);
      exit(-1);
    }

  signal(SIGINT, exitSignalHandler);
  signal(SIGKILL, exitSignalHandler);
  signal(SIGABRT, exitSignalHandler);
  signal(SIGPIPE,SIG_IGN);

  /* Last thing that main() should do */
  pthread_join(bThread_t,&end);
  pthread_join(vayvedThread_t,&end);
  return 0;

}
