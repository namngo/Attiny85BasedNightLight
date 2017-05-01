/*
 * Attiny85.c
 *
 * Created: 4/22/2017 10:56:20 PM
 * Author : nam
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <avr/sleep.h>    // Sleep Modes
#include <avr/power.h>    // Power management
#include <avr/wdt.h>      // Watchdog timer
#include <avr/interrupt.h>      // Watchdog timer

uint16_t read_analog(int ch)
{
     ADCSRA =
     (1 << ADPS2) |     // set prescaler to 128, bit 2
     (1 << ADPS1) |     // set prescaler to 128, bit 1
     (1 << ADPS0);      // set prescaler to 128, bit 0
     
    ADCSRA |= (1 << ADEN); // turn on adc
    
    ch &= 0b00000111;  // AND operation with 7
    ADMUX = (ADMUX & 0xF8)|ch; // clears the bottom 3 bits before ORing
    
    // start single convertion
    // write ’1? to ADSC
    ADCSRA |= (1<<ADSC);
    
    // wait for conversion to complete
    // ADSC becomes ’0? again
    // till then, run loop continuously
    while (ADCSRA & (1<<ADSC));
    
    uint16_t adc = ADC;
    
    // turn off adc
    ADCSRA &= ~(1 << ADEN); // turn off adc
    
    return (adc);
    
}

#define LED_PORT PB1
#define LDR_PORT PB3
#define PIR_PORT PB0
#define LightDarkEnought 650
#define Light_Time_In_Second 120

// this variable represent the state of the led
// 0 - led is off
// > 0 the number of second the led has been on
volatile int led_state = 0; 

void check_light()
{
    cli();
    if (led_state == 0)
    {
        uint16_t light_value = read_analog(LDR_PORT);
        if (light_value < LightDarkEnought)
        {
            int has_movement = PINB & (1 << PIR_PORT);
            if (has_movement)
            {
                led_state = 1;
                // turn on the led
                PORTB |= (1 << LED_PORT);
            }
        }
    }
    else
    {
        led_state ++;
        if (led_state > Light_Time_In_Second)
        {
            led_state = 0;
            // turn off the led
            PORTB &= ~(1 << LED_PORT);
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
    WDTCR = (1 << WDIE) | (1 << WDP3) | (1 << WDP0);    // set WDIE, and 8 seconds delay
    // pat the dog
    wdt_reset();
}  // end of resetWatchdog

void setup()
{
    cli();
    resetWatchdog();
    DDRB |= (1 << LED_PORT);
    sei();
}

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

int main(void)
{
    setup();

    while(1)
    {
        check_light();
        go_to_sleep();
    }
}

ISR( WDT_vect ) {
    /* dummy */
    wdt_disable();
}