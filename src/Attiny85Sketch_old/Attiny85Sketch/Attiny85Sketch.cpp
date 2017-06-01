#include "arduino.h"
/*
 * Attiny85Sketch.ino
 *
 * Created: 5/29/2017 10:36:27 PM
 * Author: nam
 */ 


#define LED_PORT PB1
#define LDR_PORT PB3
#define PIR_PORT PB0
#define LightDarkEnought 650
#define Light_On_In_Second 120
#define Light_Off_In_Second 30


void setup()
{

    /* add setup code here, setup code runs once when the processor starts */
    pinMode(PB1, OUTPUT);
}

void loop()
{

    /* add main program code here, this code starts again each time it ends */
    digitalWrite(PB1, HIGH);
    delay(1000);
    digitalWrite(PB1, LOW);
    delay(1000);
}


