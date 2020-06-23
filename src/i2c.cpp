// Author:         Ajay Katoch, Kyle Fortuno, James Rowley, John Merems, Quinton Arnaud
// Net ID:         
// Date:           4/25/19
// Assignment:     Smart Porch Group Final Project
//
// Description: I2C library for interfacing with the sensor
//
// Requirements: Contains necessary functions for interacting with an I2C device
//----------------------------------------------------------------------//

#include "i2c.h"
#include "timer.h"
#include <Wire.h>

//default address = 0x52, if LED is NOT pulled to ground it is 0x53
#define deviceAddress 0x53

//initilizes the device, was accelerometer
void initI2C(){
    DDRD |= (1 << DDD0) | (1 << DDD1);
    TWSR = 0;
    TWBR = 18;
    TWCR = (1 << TWEN) | (1 << TWINT);
}

void beginTransmission(int read){
    //concatenated with read (1) or write (0)

    // Send start bit
    TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT);
    while (!(TWCR & (1 << TWINT)));

    // Send address with R/W bit
    TWDR = (deviceAddress << 1) | read; 
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));
}

int endTransmission(){
    // Send stop bit
    TWCR = (1 << TWSTO) | (1 << TWINT) | (1 << TWEN);
    return 1;
}

int write(unsigned char parameter){
    if (TWSR == 0x18 || TWSR == 0x28)
    {
        // If SLA+W or data transmitted and ACK recieved, transmit data.
        TWDR = parameter;
        TWCR = (1 << TWINT) | (1 << TWEN);
        while (!(TWCR & (1 << TWINT)));
        return 1;
    }
    else
    {
        // Not in proper state to transmit.
        return 0;
    }
}

int requestFrom(unsigned char address, unsigned char * buffer, unsigned char quantity){
    beginTransmission(0);
    write(address);
    endTransmission();

    beginTransmission(1);
    for (int i = 0; i < quantity; i++)
    {
        read(i == (quantity - 1), buffer + i);
    }
    endTransmission();

    return 1;
}

int writeTo(unsigned char address, unsigned char value){
    beginTransmission(0);
    write(address);
    write(value);
    endTransmission();

    return 1;
}

unsigned char read(int nack, unsigned char * buffer){
    if (TWSR == 0x40 || TWSR == 0x50)
    {
        // If SLA+R transmitted or data received and ACK recieved, receive data.
        if (nack)
        {
            TWCR = (1 << TWINT) | (1 << TWEN);
        }
        else
        {
            TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
        }
        while (!(TWCR & (1 << TWINT)));
        *buffer = TWDR;
        return 1;
    }
    else
    {
        // Not in proper state to receive.
        return 0;
    }
}