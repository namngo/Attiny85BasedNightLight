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

#define _BV(BIT)   (1<<BIT)

//****************************************************************
// 0=16ms, 1=32ms,2=64ms,3=128ms,4=250ms,5=500ms
// 6=1 sec,7=2 sec, 8=4 sec, 9= 8sec
void setup_watchdog(int ii) {
    //byte bb;
    //int ww;
    //if (ii > 9 ) ii=9;
    //bb=ii & 7;
    //if (ii > 7) bb|= (1<<5);
    //bb|= (1<<WDCE);
    //ww=bb;
    //MCUSR &= ~(1<<WDRF);
    //// start timed sequence
    //watchdogRegister |= (1<<WDCE) | (1<<WDE);
    //// set new watchdog timeout value
    //watchdogRegister = bb;
    //watchdogRegister |= _BV(WDIE);
}

void initADC()
{
  /* this function initialises the ADC 

        ADC Prescaler Notes:
	--------------------

	   ADC Prescaler needs to be set so that the ADC input frequency is between 50 - 200kHz.
  
           For more information, see table 17.5 "ADC Prescaler Selections" in 
           chapter 17.13.2 "ADCSRA – ADC Control and Status Register A"
          (pages 140 and 141 on the complete ATtiny25/45/85 datasheet, Rev. 2586M–AVR–07/10)

           Valid prescaler values for various clock speeds
	
	     Clock   Available prescaler values
           ---------------------------------------
             1 MHz   8 (125kHz), 16 (62.5kHz)
             4 MHz   32 (125kHz), 64 (62.5kHz)
             8 MHz   64 (125kHz), 128 (62.5kHz)
            16 MHz   128 (125kHz)

           Below example set prescaler to 128 for mcu running at 8MHz
           (check the datasheet for the proper bit values to set the prescaler)
  */

  // 8-bit resolution
  // set ADLAR to 1 to enable the Left-shift result (only bits ADC9..ADC2 are available)
  // then, only reading ADCH is sufficient for 8-bit results (256 values)

  ADCSRA = 
            (1 << ADPS2) |     // set prescaler to 128, bit 2
            (1 << ADPS1) |     // set prescaler to 128, bit 1
            (1 << ADPS0);      // set prescaler to 128, bit 0
}

void initWatchDog()
{
    if(MCUSR & _BV(WDRF)) // If a reset was caused by the Watchdog Timer...
    {
        MCUSR &= ~_BV(WDRF);                 // Clear the WDT reset flag
        WDTCR |= (1<<WDCE) | (0<<WDE);   // Enable the WD Change Bit
        WDTCR = 0x00;                      // Disable the WDT
    }    
}

uint16_t read_analog(int ch)
{
    ADCSRA |= (1 << ADEN); // turn on adc
    
    ch &= 0b00000111;  // AND operation with 7
    ADMUX = (ADMUX & 0xF8)|ch; // clears the bottom 3 bits before ORing
    
    // start single convertion
    // write ’1? to ADSC
    ADCSRA |= (1<<ADSC);
    
    // wait for conversion to complete
    // ADSC becomes ’0? again
    // till then, run loop continuously
    while(ADCSRA & (1<<ADSC));
    
    uint16_t adc = ADC;
    
    // turn off adc
    ADCSRA &= ~(1 << ADEN); // turn on adc
    
    return (adc);
    
}

#define LED_PORT PB1
#define LDR_PORT PB3

int main(void)
{
    DDRB |= (1 << LED_PORT);
    
    initADC();

    while(1)
    {
        uint16_t raw_adc = read_analog(LDR_PORT);   // add lobyte and hibyte

        if (raw_adc > 512)
        {
            // ADC input voltage is more than half of the internal 1.1V reference voltage
            PORTB |= (1 << LED_PORT);
            PORTB &= ~(1 << LED_PORT);
        } 
        else 
        {
             PORTB &= ~(1 << LED_PORT);

        }

    }
}

