#include <stdbool.h>
#include <stdio.h>
#include "color.h"


/******** paths to led device files ***********************************/
const char red_led_file[]= "/sys/class/leds/xiaomi:red:status/brightness";
const char blue_led_file[]= "/sys/class/leds/xiaomi:blue:status/brightness";
const char yellow_led_file[]= "/sys/class/leds/xiaomi:yellow:status/brightness";
/**********************************************************************/

//Makes more sense for LEDs
const bool on=true;
const bool off=false;
/*************************/

FILE *red, *blue, *yellow;//Device file pointers

int mutex=0; //debug integer to check atomicity of changeColor();

/***********************************
 * Functions to Turn ON/OFF the LEDs
 * **********************************/
void red_led (bool ledSwitch)
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

void blue_led (bool ledSwitch)
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

void yellow_led (bool ledSwitch)
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
/*******************************************/




/***************************************************
 * Determines the necessary actions to bring up
 * a required color on the LED, by keeping
 * previous color in mind. Minimum possible file i/o.
 * ************************************************/
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

void openLedDeviceFiles ()
{
  red = fopen(red_led_file,"w");
  blue = fopen(blue_led_file,"w");
  yellow = fopen(yellow_led_file,"w");
}


void closeLedDeviceFiles ()
{
  fclose(red);
  fclose(blue);
  fclose(yellow);
}

void resetLeds ()
{
    red_led(off);
    yellow_led(off);
    blue_led(off);
}
