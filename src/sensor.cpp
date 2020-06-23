// Author:         Ajay Katoch, Kyle Fortuno, James Rowley, John Merems, Quinton Arnaud
// Net ID:         
// Date:           4/25/19
// Assignment:     Smart Porch Group Final Project
//
// Description: Library using I2C to interact with a si115x sensor
//
// Requirements: Uses I2C to interact with the si115x sensor to measure
//              ambient light levels and how close objects are to the device
//----------------------------------------------------------------------//

#include "sensor.h"
#include "i2c.h"
#include <Arduino.h>
#include "timer.h"

/*  initSensor initilizes the si115x sensor
    It initilizes the CHAN_LIST using writeParameter
    The CHAN_LIST configures which output channels (0 through 5) are used as
    I2C outputs (I2C registers 0x13 through 0x2C)

    Currently initilizes Channel 0 and Channel 1
*/
void initSensor(){
    //Forces the sensor to reset
    //This is to ensure sensor is in default state whenever program is uploaded to the arduino
    writeTo(0x0B, 0x01); 
    
    delayms(1); //ensures sensor finishes reseting itself (time needed unknown)
    /*  CHAN_LIST is parameter 0x01, see datasheet p16
        CHAN_LIST is configured based on datasheet p26
        to enable channel 0 and channel 1 (bit 7, bit 6 are unused)*/
    ParameterSet(0x01, 0b00000011);

    //Configure global parameter MEASRATE
    ParameterSet(0x1A, 0x00);
    ParameterSet(0x1B, 0x01);

    //Configure global parameter MEASCOUNT (not currently used)
    //ParameterSet(0x1C, 0x05);

    //configure the individual measurement channels
    configureChannel0();
    configureChannel1();
}

/*  This function uses I2C to issue a FORCE command to the sensor.
    It will initiates a set of measurements specified in teh CHAN_LIST
    parameter. A FORCE command will only execute the measurements which
    do not have a meas counter index configured in MEAS-CONFIGx
*/
void ForceMeasurement(){
    writeTo(0x0B, 0x11);
}

/*  The si115x sensor uses I2C to interact with it's array of I2C registers
    The I2C registers can be used to interact with the sensor parameter
    registers. Sensor parameters registers can not otherwise be directly
    accessed. This function issues a command to store the value of a
    parameter register in the I2C register RESPONSE1
    See datasheet page 18 for more information
    See datasheet page 19 for explanation of how we check the command counter
*/
unsigned char ParameterQuery(unsigned char parameterAddress){
    unsigned char buff = 0;
    unsigned char commandCounter = 0;
    unsigned char initCommandCounter = 0;
    //initial request for response 0
    requestFrom(0x11, &buff, 1); //read response 0, datasheet p19
    initCommandCounter = buff & 0x0f;
    writeTo(0x0B, (0b01000000 | parameterAddress)); //command
    do{ //this checks to see if the command counter has incremented
        //and continues to check until the command counter increments
        //(indicating the operation is complete)
        requestFrom(0x11, &buff, 1);
        commandCounter = buff & 0x0f;
    }
    while(initCommandCounter == commandCounter); //successful command counter increment
    
    requestFrom(0x10, &buff, 1); //read response 1, its the desired data
    return buff;
}

/*  The si115x sensor uses I2C to interact with it's array of I2C registers
    The I2C registers can be used to interact with the sensor parameter
    registers. Sensor parameter registers can not be directly accessed, so we
    must use I2C registers to interact with parameters
    This function writes data to a parameter at parameterAddress

    NOTE: parameterAddress is a 6 bit binary number
    NOTE: this function currently does not check for errors resulting from
    parameter writes, or the size of parameterAddress*/
void ParameterSet(unsigned char parameterAddress, unsigned char data){
    /*writes data to INPUT0, then issues PARAM_SET command
        to write data to the parameter
        see page 18 of si115x datahseet */
    //Serial.println(parameterAddress, HEX); debugging currently unused

    unsigned char buff = 0;
    unsigned char commandCounter = 0;
    unsigned char initCommandCounter = 0;
    //initial request for response 0
    requestFrom(0x11, &buff, 1); //read response 0, datasheet p19
    initCommandCounter = buff & 0x0f;


    /*0x0A is I2C address of HOSTIN0 (datasheet p16).
        It's value is written to a parameter during the command PARAM SET*/
    writeTo(0x0A, data); //writes data to input0

    /*writes PARAM_SET (0b10XXXXXX) to I2C Register COMMAND (address 0x0B)
        XXXXXX in PARAM_SET is the parameter address in the sensor parameter
        table (see datasheet p 16) */
    writeTo(0x0B, (0b10000000 | parameterAddress));

    //read response 0 until command counter is incremented
    //int i = 0;
    do{ //this checks to see if the command counter has incremented
        //and continues to check until the command counter increments
        //(indicating the operation is complete)
        requestFrom(0x11, &buff, 1);
        commandCounter = buff & 0x0f;
        /*i++;
        if(i > 10000) {
            
            Serial.println()

        }*/
    }
    while(initCommandCounter == commandCounter);
    
}

void configureChannel0(){
    /*  configures channel 0 for measuring ambient light levels
    */

    /*ADCCONFIG0 (parameter 0x02) set to 0bX00?????
        bit 7) unused
        bit 6:5) Decimation rate
            I don't know much about decimation rate so I am setting it to the 'normal' value
        bit 4:0) Photodiode used for measurement select
            Ambient light sensing doesn't use a red LED so I am setting it to use a white led */
   ParameterSet(0x02, 0b00001011);

    /*ADCSENS0 (parameter 0x03) set to 0b0XXX0000
        bit 7) HSIG: not completely sure, leaving it at 0 for normal gain
        bit 6:4) SW_GAIN[2:0]: has to do with forced mode. Not totally sure what value it should be
        bit 3:0) HW_GAIN[3:0]: 48.8us, based on common decimation rate (were using default)
            hence we will use 0b001 which corresponds to 48.8us
    */
    ParameterSet(0x03, 0b00000000);

    /*ADCPOST0 (parameter 0x04) set to 0bX0???X00, datasheet p40
        bit 7: reserved (unused)
        bit 6) 0 to set to 16 bit output (not using threshold for ambeint light sensing, so can be either)
        but 5:3) Uncertain, may have something to do with SW_GAIN in ADCSENSx
        bit 2) threshold polarity. Defaulting to 0 because ambient light sensor is not using thresholds
        bit 1:0) set to 0 (0b00) to disable thresholds
    */
    ParameterSet(0x04, 0b00000000);

    /*MEASCONFIG0 (parameter 0x05) set to 0b00000000
        bit 7:6: Selects measurement count; 01 corresponds to MEASCOUNT0
            set to 0 (0b00) to only measure in BURST mode or when forced
        bit 5:4: nominal current values 
        bit 3: Bank select, selecting bank A (arbitrarily, its how we wrote the rest of the code)
        bit 2: LED2_EN: enables LED 2; this order of 2 3 1 for LEDs is CORRECT
        bit 1: LED3_EN: enables LED 3; this order of 2 3 1 for LEDs is CORRECT
        bit 0: LED1_EN: enables LED 1; this order of 2 3 1 for LEDs is CORRECT
    */
    ParameterSet(0x05, 0b00000000);

}

void configureChannel1(){
    /*  configures channel 1 for proximity sensing
        NOTE: threshold based interrupt is ONLY availible for 16 bit output mode
            do NOT set 24 bit mode when using threshold (datasheet p31)*/

    /*Configure Upper and Lower Threshold
        Upper Threshold parameter address: 0x29, 0x2A
        Lower Threshold parameter address: 0x2C, 0x2D
        NOTE: The current data are just guesses. They are the threshold
            window's upper and lower bounds, but I don't know how
            the window bounds correlate to the parameter values
        NOTE: These are global parameters, but how the code is written for
            the original project, only 1 proximity sensing channel is used
            so 
    */
    //ParameterSet(0x29, 0x0F);
    //ParameterSet(0x2A, 0x0F);
    //ParameterSet(0x2C, 0x0F); //These parameters (0x2C and 0x2D) cause an error for some reason
    //ParameterSet(0x2D, 0x0F); //so we are not currently using thresholds to trigger interrupts

    /*LED current (100mA MAX, which corresponds to 2A)
        SO DONT SET THIS ABOVE 2A
        table of LED current codes (datasheet p49)
        LED 1a is parameter 0x1f (datasheet p37)*/

    ParameterSet(0x1f, 0x2A);

    /* ADCCONFIG1 (parameter 0x06) set to 0b????????, datasheet p38
        bit 7) unused
        bit 6:5) Decimation rate
            I don't know much about decimation rate so I am setting it to the 'normal' value
        bit 4:0) Photodiode used for measurement select
            Channel 1 is (currently) being used for proximity sensing, so it will use IR LED
            We are using 1 IR LED, hence 0b00000
    */
    ParameterSet(0x06, 0b00000010); 
   
    /*ADCSENS1 (parameter 0x07) set to 0b????????, datahseet p39
        bit 7) HSIG: not completely sure, leaving it at 0 for normal gain
        bit 6:4) SW_GAIN[2:0]: has to do with forced mode, we are not using forced mode for
            proximity sensor, hence 0000
        bit 3:0) HW_GAIN[3:0]: 48.8us, based on common decimation rate (were using default)
            hence we will use 0b001 which corresponds to 48.8us
    */
    ParameterSet(0x07, 0b00100001);

    /*ADCPOST1 (parameter 0x08) set to 0bX0???011, datasheet p40
        bit 7: unused
        bit 6) 0 to set to 16 bit output
        but 5:3) Uncertain, may have something to do with SW_GAIN in ADCSENSx
        bit 2) THRESHOLD POLARITY: 0 for interrupt to be triggered when sample exits threshold
        bit 1:0) set to 3 (0b11) Interrupt when sample exit/enters window between UPPER_THRESHOLD and LOWER_THESHOLD
    */
    ParameterSet(0x08, 0b00111000);

    /*MEASCONFIG1
        bit 7:6: Selects measurement count; 01 corresponds to MEASCOUNT0
        bit 5:4: nominal current values 
        bit 3: Bank select, selecting bank A (arbitrarily, its how we wrote the rest of the code)
        bit 2: LED2_EN: enables LED 2; this order of 2 3 1 for LEDs is CORRECT
        bit 1: LED3_EN: enables LED 3; this order of 2 3 1 for LEDs is CORRECT
        bit 0: LED1_EN: enables LED 1; this order of 2 3 1 for LEDs is CORRECT
    */
    ParameterSet(0x09, 0b00000001);
    
}