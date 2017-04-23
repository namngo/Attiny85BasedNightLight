/*
 * Attiny85.c
 *
 * Created: 4/22/2017 10:56:20 PM
 * Author : nam
 */ 

#include <avr/io.h>

void setupAnalogRead(int pin)
{
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    ADMUX |= (1 << REFS0);

    
}

int main(void)
{
    //setupAnalogRead(
    /* Replace with your application code */
    while (1) 
    {
        analogRead(1);
    }
}

