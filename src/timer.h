// Author:         Ajay Katoch, Kyle Fortuno, James Rowley, John Merems, Quinton Arnaud
// Net ID:         
// Date:           4/25/19
// Assignment:     Smart Porch Group Final Project
//
// Description: millisecond timer using timer1 on the arduino
//
// Requirements:Initilizes timer1 for use as a millisecond timer
//----------------------------------------------------------------------//

#ifndef TIMER_H
#define TIMER_H

#include <avr/io.h>

void initTimer1();
void delayms(unsigned int delay);

#endif