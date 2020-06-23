// Author:         Ajay Katoch, Kyle Fortuno, James Rowley, John Merems, Quinton Arnaud
// Net ID:         
// Date:           4/25/19
// Assignment:     Smart Porch Group Final Project
//
// Description: Main code for a smart porch light sensor using an
//              Arduino Mega 2560 and SI115x sensor for ambient light sensing
//              and proximity sensing
//
// Requirements: When something is close in front of the sensor as determined
//              by the proximity sensor, if the ambient light levels are low
//              enough (ie: it is dark out), the light (a red LED) turns on
//----------------------------------------------------------------------//

#include <Arduino.h> 
#include <avr/io.h>
#include "timer.h"
#include "i2c.h"
#include "sensor.h"

#define NUM_BYTES 4

int main(){
  //initilizing functions
  initTimer1();
  initI2C();
  Serial.begin(9600);
  //Serial.println("Initilizing Sensor"); //for debugging
  initSensor();
  //Serial.println("Initilizing Complete"); //for debugging
  //initilize the LED
  DDRA = (1 << PORTA0);
  PORTA &= ~(1 << PORTA0);

  //DDRB &= ~(1 << DDB0); //set interrupt, no longer used
  unsigned char buf[NUM_BYTES] = {0};

  //command counter loop variables to ensure sensor is ready
  unsigned char buff = 0;
  unsigned char commandCounter = 0;
  unsigned char initCommandCounter = 0;

  while(1){
    
    requestFrom(0x11, &buff, 1); //read response 0, datasheet p19
    initCommandCounter = buff & 0x0f;

    ForceMeasurement();
    //verifying sensor is ready after forcing measurement
    do{ //this checks to see if the command counter has incremented
        //and continues to check until the command counter increments
        //(indicating the operation is complete)
        requestFrom(0x11, &buff, 1);
        commandCounter = buff & 0x0f;
    }
    while(initCommandCounter == commandCounter);

    requestFrom(0x13, buf, NUM_BYTES);
    short unsigned int AmbientLight = (short unsigned int)(((unsigned short int)buf[0] << 8) + buf[1]);
    short unsigned int Proximity = (short unsigned int)(((unsigned short int)buf[3] << 8) + buf[4]);

    /*
    //Serial Output of values for debugging
    Serial.print("---\n");
    Serial.println(AmbientLight, DEC);
    Serial.println(Proximity, DEC);
    */
    
    if ((Proximity > 0x4FFF) & (AmbientLight < 0x000F)){
      PORTA |= (1 << PORTA0); //turns led on
      delayms(10000); //for 10 seconds
      PORTA &= ~(1 << PORTA0); //turns led off
    }
    else{
      PORTA &= ~(1 << PORTA0);
    }
    
    
    delayms(100);
  }

  return 0;
}