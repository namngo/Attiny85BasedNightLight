#include "arduino.h"
#include <avr/sleep.h>    // Sleep Modes
#include <avr/power.h>    // Power management
#include <avr/wdt.h>      // Watchdog timer
#include <SoftwareSerial.h>

/*
 * Attiny85Sketch.ino
 *
 * Created: 5/29/2017 10:52:34 PM
 * Author: nam
 */ 

#define LED_PORT PB1
#define LDR_PORT PB3
#define PIR_PORT PB0

#define TxPin PB2
#define RxPin PB4

#define Sleep_Time_Second 2

#define Light_Dark_Value 650
#define Light_On_In_Second 120 / Sleep_Time_Second
#define Light_Off_In_Second 30 / Sleep_Time_Second


SoftwareSerial serial(RxPin, TxPin);

// this variable represent the state of the led
// 0 - led is off
// > 0 the number of second the led has been on
volatile int led_state = 0;

void setup()
{

      /* add setup code here, setup code runs once when the processor starts */
    pinMode(LED_PORT, OUTPUT);
    pinMode(LDR_PORT, INPUT);
    pinMode(PIR_PORT, INPUT);
    
    serial.begin(9600);
}

void check_light()
{
    cli();
    
    if (led_state == 0)
    {
        int light_value = analogRead(LDR_PORT);
        serial.print("light value =");
        serial.println(light_value);
        
        if (light_value < Light_Dark_Value)
        {
            int has_movement = digitalRead(PIR_PORT);
            if (has_movement)
            {
                serial.print("movement! ");
                serial.println(light_value);
                led_state = 1;
                digitalWrite(LED_PORT, HIGH);
            }
        }
    }
    else
    {
        serial.print("led state=");
        serial.println(led_state);
        led_state ++;
        if (led_state == Light_On_In_Second)
        {
            digitalWrite(LED_PORT, LOW);
            serial.println("off light");
        }
        else if (led_state == Light_On_In_Second + Light_Off_In_Second)
        {
            led_state = 0;
            serial.println("reset");
        }
    }
    
    sei();
}

void resetWatchdog ()
{
    // clear various "reset" flags
    MCUSR = 0;
    // allow changes, disable reset, clear existing interrupt
    WDTCR = (1 << WDCE) | (1 << WDE) | (1 << WDIF);
    // set interrupt mode and an interval (WDE must be changed from 1 to 0 here)
    WDTCR = (1 << WDIE) | (1 << WDP2) | (1 << WDP1) | (1 << WDP0);    // set WDIE, and 1 seconds delay
    // pat the dog
    wdt_reset();
}  // end of resetWatchdog

void go_to_sleep()
{
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    power_all_disable();
    resetWatchdog();
    sleep_enable();
    sei();
    sleep_cpu();
    sleep_disable();
    power_all_enable();
}

ISR( WDT_vect ) {
    /* dummy */
    wdt_disable();
}

void loop()
{
    check_light();
    go_to_sleep();
}