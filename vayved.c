#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

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

inline red_led (bool ledSwitch)
{
    if (ledSwitch)
        ;//turn on red
    else
        ; //turn off red
}

inline blue_led (bool ledSwitch)
{
    if (ledSwitch)
        ;//turn on red
    else
        ; //turn off red
}

inline yellow_led (bool ledSwitch)
{
    if (ledSwitch)
        ;//turn on red
    else
        ; //turn off red
}



int changeColor (Color color)
{
    switch (color) {
    case RED:
        switch (currentColor)
        {
        case RED:
            break;
        case ORANGE:
            red_led(off);
            yellow_led(on);
            break;
        case VIOLET:
            blue_led(on);
            break;
        case BLUE:
            red_led(off);
            blue_led(on);
            break;
        case BLACK:
            red_led(off);
            break;
        }
        break;

    case ORANGE:
        switch (currentColor)
        {
        case RED:
            yellow_led(off);
            red_led(on);
            break;
        case ORANGE:
            break;
        case VIOLET:
            yellow_led(off);
            blue_led(on);
            red_led(on);
            break;
        case BLUE:
            yellow_led(off);
            blue_led(on);
            break;
        case BLACK:
            yellow_led_led(off);
            break;
        }
        break;
    }
}



void* LedBlinkerThread(void *threadId)
{

}

void* keyboardThread (void* threadId)
{

}

int main(int argc, char *argv[])
{
    pthread_t thread;
    int rc;
    long t;
    //rc = pthread_create(&threads[t], NULL, PrintHello, (void *)t);
    if (rc){
        printf("ERROR; return code from pthread_create() is %d\n", rc);
        exit(-1);
    }
    //  }

    /* Last thing that main() should do */
    pthread_exit(NULL);
}
