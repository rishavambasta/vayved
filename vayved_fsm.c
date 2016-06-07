#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <string.h>


#define INTER_POLL_DELAY 1 //seconds
#define MAX_LINE 500
#define VERBOSE 1
/******** paths to led device files ***********************************/
const char red_led_file[]= "/sys/class/leds/xiaomi:red:status/brightness";
const char blue_led_file[]= "/sys/class/leds/xiaomi:blue:status/brightness";
const char yellow_led_file[]= "/sys/class/leds/xiaomi:yellow:status/brightness";
/**********************************************************************/

//TODO: implement a state called TUNNELLED_BUT_OFFLINE
//currently its being assumed that tunnelled traffic always findes route to Internet
typedef enum
{
  OFFLINE,
  TRYING_TO_GO_ONLINE,
  ONLINE_UNTUNNELLED,
  TRYING_TO_CREATE_TUNNEL,
  ONLINE_TUNNELLED
}FSM_STATES;

FSM_STATES current_state = OFFLINE;
bool canBlink;
FILE *red, *blue, *yellow;
int mutex=0;


int dummy_fail_count = 0;


const bool on=true;
const bool off=false;

bool internetConnected (void);
bool tunnelExists (void);
int createTunnel(void);
void resetLeds(void);



typedef enum
{
  RED,
  ORANGE,
  VIOLET,
  BLUE,
  BLACK
}Color;

Color currentColor = BLACK;

inline void red_led (bool ledSwitch)
{
  if (ledSwitch)
    {
      fwrite("1",1,1,red); //turn off red
      fflush(red);
    }
  else
    {
      fwrite("0",1,1,red); ; //turn off red
      fflush(red);
    }
}

inline void blue_led (bool ledSwitch)
{
  if (ledSwitch)
    {
      fwrite("1",1,1,blue);
      fflush(blue);//turn on blue
    }
  else
    {
      fwrite("0",1,1,blue);
      fflush(blue);//turn off blue
    }
}

inline void yellow_led (bool ledSwitch)
{
  if (ledSwitch)
    {
      fwrite("1",1,1,yellow);
      fflush(yellow);//turn on yellow
    }
  else
    {
      fwrite("0",1,1,yellow);
      fflush(yellow);//turn off yellow
    }
}

void changeColor(Color colorToBeUpdated)
{
  if (mutex > 0)
    printf ("Atomicity violated");
  ++mutex;
  switch (colorToBeUpdated)
    {
    case RED:
      switch (currentColor)
        {
        case RED:
          break;
        case ORANGE:
          yellow_led(off);
          red_led(on);
          break;
        case VIOLET:
          blue_led(off);
          break;
        case BLUE:
          blue_led(off);
          red_led(on);
          break;
        case BLACK:
          red_led(on);
          break;
        }
      break;

    case ORANGE:
      switch (currentColor)
        {
        case RED:
          red_led(off);
          yellow_led(on);
          break;
        case ORANGE:
          break;
        case VIOLET:
          blue_led(off);
          red_led(off);
          yellow_led(on);
          break;
        case BLUE:
          blue_led(off);
          yellow_led(on);
          break;
        case BLACK:
          yellow_led(on);
          break;
        }
      break;

    case VIOLET:
      switch (currentColor)
        {
        case RED:
          blue_led(on);
          break;
        case ORANGE:
          yellow_led(off);
          blue_led(on);
          red_led(on);
          break;
        case VIOLET:
          break;
        case BLUE:
          red_led(on);
          break;
        case BLACK:
          blue_led(on);
          red_led(on);
          break;
        }
      break;

    case BLUE:
      switch (currentColor)
        {
        case RED:
          red_led(off);
          blue_led(on);
          break;
        case ORANGE:
          yellow_led(off);
          blue_led(on);
          break;
        case VIOLET:
          red_led(off);
          break;
        case BLUE:
          break;
        case BLACK:
          blue_led(on);
          break;
        }
      break;

    case BLACK:
      switch (currentColor)
        {
        case RED:
          red_led(off);
          break;
        case ORANGE:
          yellow_led(off);
        case VIOLET:
          blue_led(off);
          red_led(off);
          break;
        case BLUE:
          blue_led(off);
          break;
        case BLACK:
          break;
        }
      break;
    }
  currentColor = colorToBeUpdated;
  --mutex;
}

char* getColorName (int code)
{
  switch (code) {
    case 0:
      return "RED";
      break;
    case 1:
      return "ORANGE";
      break;
    case 2:
      return "VIOLET";
      break;
    case 3:
      return "BLUE";
      break;
    case 4:
      return "OFF";
      break;
    }
}

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
          printf ("\nOnline");
          fflush(stdout);
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
                  next_state = ONLINE_TUNNELLED;
                else
                  next_state = ONLINE_UNTUNNELLED;
              }
          else
            next_state = OFFLINE;
          break;



        case ONLINE_TUNNELLED:
          changeColor(VIOLET);
          canBlink = false;

          if (internetConnected())
            if (tunnelExists())
              next_state = ONLINE_TUNNELLED;
            else
              next_state = ONLINE_UNTUNNELLED;
          else
            next_state = OFFLINE;
          break;
        }
      sleep(INTER_POLL_DELAY);
      current_state = next_state; // Transition of state
    }
  return (void*) NULL;
}


void* ledBlinkerThread(void *threadID)  //vestigial right now
{
  Color backupColor;
  while(true)
    {
      backupColor = currentColor;

      if (canBlink)
        {
          changeColor(BLACK);
          //dirty flag on
          usleep(50000);
          //dirty flag off
        }
      if (currentColor == BLACK)
        changeColor(backupColor);
      usleep(50000);

    }
  return (void*) NULL;
}




void exitSignalHandler (int signum)
{
  printf ("\nExiting with little housekeeping before we go..\n");
  resetLeds();
  fclose(red);
  fclose(blue);
  fclose(yellow);
  exit(0);
}


inline void report_error (char message[])
{
  printf ("\n%s",message);
}


bool internetConnected ()
{
  int sockfd,val;
  char buffer[MAX_LINE];
  struct sockaddr_in ipAddress;
  struct hostent *entity=NULL;

  sockfd = -1;

  if((entity = gethostbyname("www.google.com")) != NULL)
    {
      if((sockfd = socket(entity->h_addrtype,SOCK_STREAM,IPPROTO_TCP)) != -1)
        {
          val = 1;
          if(setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR, (char *) &val, sizeof(val)) == 0 && setsockopt(sockfd,IPPROTO_TCP,TCP_NODELAY, (char *) &val, sizeof(val)) == 0)
            {
              ipAddress.sin_family = entity->h_addrtype;
              memcpy(&(ipAddress.sin_addr), entity->h_addr, entity->h_length);
              ipAddress.sin_port = htons(80);

              if(connect(sockfd,(struct sockaddr *) &ipAddress,sizeof(ipAddress)) == 0)
                {
                  if(write(sockfd,"GET /index.html HTTP/1.1\r\n\r\n", 29) >= 28)
                    {
                      shutdown(sockfd, SHUT_WR);
                      if(read(sockfd, buffer, MAX_LINE) != -1) // all right!
                        {
                          //printf ("\nAll OKAY !!");
                          close(sockfd);
                          return true;
                        }
                      else
                        report_error("read()");
                    }
                  else
                    report_error("write()");
                }
              else
                report_error("connect()");
            }
          else
            report_error("setsockopt()");
        }
      else
        report_error("socket()");
    }
  else
    report_error("cannot resolve IP for Google."); // this is is the most common error.

  if(sockfd!=-1)
    close(sockfd); // no internet
  return false;
}


bool tunnelExists()
{
  if (dummy_fail_count < 2)
    return false;
  else
    return true;
}

//Return value 1 means created successfully
//0 means no success
int createTunnel()
{
  sleep(1);//no op
  if (dummy_fail_count < 2)
    {
      dummy_fail_count++;
      return 0;
    }
  else
    {  return 1; //
    }
}

void resetLeds ()
{
  red_led(off);
  yellow_led(off);
  blue_led(off);
}


int main ()
{
  red = fopen(red_led_file,"w");
  blue = fopen(blue_led_file,"w");
  yellow = fopen(yellow_led_file,"w");
  void *end;



  pthread_t vayvedThread_t,bThread_t;
  int vayvedThread,bThread;

  current_state = OFFLINE;
  currentColor = BLACK;
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
