#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <string.h>
#include <netdb.h>


#define BLINK_DURATION 1 // One second of sleep
#define MAX_LINE 500

const bool on=true;
const bool off=false;

/******** paths to led device files ******/
const char red_led_file[]= "/sys/class/leds/xiaomi:red:status/brightness";
const char blue_led_file[]= "/sys/class/leds/xiaomi:blue:status/brightness";
const char yellow_led_file[]= "/sys/class/leds/xiaomi:yellow:status/brightness";
/****************************************/

/*** possible colors we can generate *****/
typedef enum
{
    RED,
    ORANGE,
    VIOLET,
    BLUE,
    BLACK
}Color;
/*****************************************/

Color currentColor = BLACK;
bool canBlink = false;
int userInput = 0;
FILE *red, *blue, *yellow;

/******** functions to turn switch led state ***************/
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

/*****************************************************************/

void resetLeds ()
{
    red_led(off);
    yellow_led(off);
    blue_led(off);
}

void changeColor (Color color)
{
    switch (color)
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
        case ORANGE:currentColor
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
    currentColor = color;
}



void* ledBlinkerThread(void *color)
{
    Color* blinkColor = (Color *)color;
    while(true)
    {
        if (canBlink)
            changeColor(*blinkColor);
        usleep(90000);
        if (canBlink)
            changeColor(BLACK);
        usleep(90000);
    }

    return (void*) NULL;
}


void report_error (char message[])
{
    printf ("\n%s",message);
    changeColor(RED);
}

void* internetConnectivityCheckerThread (void* threadId)
{

    //        printf ("\nDo you want the LED to blink BLUE ?? (1/0)");
    //        //fflush(STDIN);
    //        scanf("%d",&userInput);
    //        if (userInput == 1)
    //            canBlink = true;
    //        else if (userInput == 0)
    //            canBlink = false;
    //        else
    //            printf ("\nInvalid answer, enter 0 or 1..");


    int sockfd,val;
    char buffer[MAX_LINE];
    struct sockaddr_in ipAddress;
    struct hostent *entity=NULL;

    sockfd = -1;
    

    while (true)
    {
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
                                printf ("\nAll OKAY !!");
                                close(sockfd);
                                changeColor(BLUE);
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
            report_error("cannot resolve IP for mom Google."); // this is is the most common error.

        if(sockfd!=-1)
            close(sockfd); // no internet
        sleep(5);
    }
}


void exitSignalHandler (int signum)
{
    printf ("\nExiting with little housekeeping before we go..\n");
    fclose(red);
    fclose(blue);
    fclose(yellow);
    exit(0);
}

void showOffLeds (void)
{
    int i;
    for (i = 0; i < 3 ; i++)
    {
        changeColor(RED);\
        usleep(90000);
        changeColor(BLUE);\
        usleep(90000);
        changeColor(ORANGE);\
        usleep(90000);
        changeColor(VIOLET);\
        usleep(90000);
        changeColor(BLACK);
    }

}


int main()
{
    pthread_t bThread_t,iThread_t;
    int bThread,iThread;

    Color blinkColor=BLUE;

    void *end;

    red = fopen(red_led_file,"w");
    blue = fopen(blue_led_file,"w");
    yellow = fopen(yellow_led_file,"w");

    resetLeds();
    //showOffLeds();


    bThread = pthread_create(&bThread_t, NULL, ledBlinkerThread,&blinkColor);
    if (bThread)
    {
        printf("ERROR; return code from pthread_create() is %d\n", bThread);
        exit(-1);
    }

    //Show user that LED blinker is intialized
    canBlink = true;
    sleep (3);
    canBlink = false;

    iThread = pthread_create(&iThread_t, NULL, internetConnectivityCheckerThread, &blinkColor);
    if (iThread)
    {
        printf("ERROR; return code from pthread_create() is %d\n", iThread);
        exit(-1);
    }

    signal(SIGINT, exitSignalHandler);
    signal(SIGKILL, exitSignalHandler);
    signal(SIGABRT, exitSignalHandler);
    signal(SIGPIPE,SIG_IGN);

    /* Last thing that main() should do */
    pthread_join(bThread_t,&end);
    pthread_join(iThread_t,&end);

    return 0;
}
