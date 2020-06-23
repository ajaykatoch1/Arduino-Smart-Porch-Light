// Author:         Ajay Katoch, Kyle Fortuno, James Rowley, John Merems, Quinton Arnaud
// Net ID:         
// Date:           4/25/19
// Assignment:     Smart Porch Group Final Project
//
// Description: millisecond timer using timer1 on the arduino
//
// Requirements: Initilizes timer1 for use as a millisecond timer
//----------------------------------------------------------------------//

#include "timer.h"

/* Initialize timer 1, you should not turn the timer on here. Use CTC mode  .*/
void initTimer1(){
    //set timer 1 to be in CTC mode (p145 on data sheet)
    //WGMn3 on data sheet: Appears to be n = 1 for timer 1
    TCCR1B &= ~(1<< WGM13); 
    TCCR1B |= (1 << WGM12); 
    TCCR1A &= ~(1 << WGM11); 
    TCCR1A &= ~(1 << WGM10); 

    //Set prescaler: 64 prescaler
    TCCR1B &= ~(1 << CS12);  //0
    TCCR1B |= (1 << CS11); //1
    TCCR1B |= (1 << CS10);  //1

    //Set the compare value A to 15
    OCR1A = 250;
    
    return;
}

/* This delays the program an amount specified by unsigned int delay of ms.
*/
void delayms(unsigned int delay){
    unsigned int delayCount = 0;
    //sets the flag down by setting TIFR1 to 1
    TIFR1 |= (1 << OCF1A); 
    TCNT1 = 0;

    //Goes through the while loop until you do the 1 ms timer (delay #) of times
    while (delayCount < delay){ 
        if (TIFR1 & (1 << OCF1A)){
            delayCount++;
            TIFR1 |= (1 << OCF1A); //setting this to 1 will set the flag down
        }
    }

return;
}