// Author:         Ajay Katoch, Kyle Fortuno, James Rowley, John Merems, Quinton Arnaud
// Net ID:         
// Date:           4/25/19
// Assignment:     Smart Porch Group Final Project
//
// Description: I2C library for interfacing with the sensor
//
// Requirements: Contains necessary functions for interacting with an I2C device
//----------------------------------------------------------------------//

#ifndef I2C_H
#define I2C_H

#include <avr/io.h>

void initI2C();
void beginTransmission(int address);
int endTransmission();
int write(unsigned char parameter);
int requestFrom(unsigned char address, unsigned char * buffer, unsigned char quantity);
int writeTo(unsigned char address, unsigned char value);
int availible();
unsigned char read(int nack, unsigned char * buffer);

#endif