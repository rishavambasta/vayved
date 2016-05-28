#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
//#include <sys/types.h>
//#include <sys/stat.h>
//#include <fcntl.h>

#define BLINK_DURATION 1 // One second of sleep

const bool on=true;
const bool off=false;

/******** paths to led device files ******/
const char red_led_file[]= "/sys/class/leds/xiaomi:red:status/brightness";
const char blue_led_file[]= "/sys/class/leds/xiaomi:blue:status/brightness";
const char yellow_led_file[]= "/sys/class/leds/xiaomi:yellow:status/brightness";
/****************************************/

typedef enum
{
    RED,
    ORANGE,
    VIOLET,
    BLUE,
    BLACK
}Color;

Color currentColor = BLACK;

//bool canBlink = false;
bool canBlink = true;
char userInput = '0';
FILE *red, *blue, *yello;

inline void red_led (bool ledSwitch)
{
    if (ledSwitch)
       fwrite("1",1,1,red);
    else
       fwrite("0",1,1,red); ; //turn off red
}

inline void blue_led (bool ledSwitch)
{
    if (ledSwitch)
        ;//turn on blue
    else
        ; //turn off blue
}

inline void yellow_led (bool ledSwitch)
{
    if (ledSwitch)
        ;//turn on yellow
    else
        ; //turn off yellow
}



int changeColor (Color color)
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

    case BLACK:
        switch (currentColor)
        {
        case RED:
            red_led(on);
            break;
        case ORANGE:
            yellow_led(on);
        case VIOLET:
            blue_led(on);
            red_led(on);
            break;
        case BLUE:
            blue_led(on);
            break;
        case BLACK:
            break;
        }
        break;
    }
    currentColor = color;
}



void* ledBlinkerThread(void *threadId)
{
       while(true)
       {
           if (canBlink)
            changeColor(RED);
           sleep(BLINK_DURATION);
           if (canBlink)
           changeColor(BLACK);
           sleep(BLINK_DURATION);
       }
}

void* keyboardThread (void* threadId)
{
    while (true)
    {
        printf ("\nDo you want the LED to blink BLUE ?? (y/n)");
        scanf("%d",&userInput);
        if (userInput == 'y')
            canBlink = true;
        else if (userInput == 'n')
            canBlink = false;
        else
            printf ("\nInvalid answer, enter y or n..");
    }
}

int main(int argc, char *argv[])
{
    pthread_t bThread_t,kThread_t;
    int bThread,kThread;
    long t=0;
    red = fopen(red_led_file,"w");
    ledBlinkerThread(&t);
//    if (red == -1)
//    {
//        printf("Error: Openning device file for RED");
//        exit (-1);
//    }

//    bThread = pthread_create(&bThread_t, NULL, ledBlinkerThread, &t);
//    if (bThread)
//    {
//        printf("ERROR; return code from pthread_create() is %d\n", bThread);
//        exit(-1);
//    }

    /*
    kThread = pthread_create(&kThread_t, NULL, keyboardThread, &t);
    if (kThread)
    {
        printf("ERROR; return code from pthread_create() is %d\n", kThread);
        exit(-1);
    }
    */
 
    sleep(10);
    /* Last thing that main() should do */
    pthread_join(bThread,NULL);
    //pthread_join(kThread,NULL);
    fclose(red);
    return 0;
}
