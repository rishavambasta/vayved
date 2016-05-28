#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define ON 1
#define OFF 0

/******** paths to led device files ******/
const char red_led_file[]= "/sys/class/leds/xiaomi:red:status/brightness";
const char blue_led_file[]= "/sys/class/leds/xiaomi:blue:status/brightness";
const char yellow_led_file[]= "/sys/class/leds/xiaomi:yellow:status/brightness";
/****************************************/

enum Colors
{
	RED,
	ORANGE,
	VIOLET,
	BLUE,
	BLACK
};

int changeColor (Color);


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
