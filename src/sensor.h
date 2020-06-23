// Author:         Ajay Katoch, Kyle Fortuno, James Rowley, John Merems, Quinton Arnaud
// Net ID:         
// Date:           4/25/19
// Assignment:     Smart Porch Group Final Project
//
// Description: Library using I2C to interact with a si115x sensor
//
// Requirements:Uses I2C to interact with the si115x sensor to measure
//              ambient light levels and how close objects are to the device
//----------------------------------------------------------------------//

#ifndef SENSOR_H
#define SENSOR_H

#include <avr/io.h>

void initSensor();
void ForceMeasurement();
unsigned char ParameterQuery(unsigned char parameterAddress);
void ParameterSet(unsigned char parameterAddress, unsigned char data);
void configureChannel0();
void configureChannel1();

#endif