/*
 * Attiny85.c
 *
 * Created: 4/22/2017 10:56:20 PM
 * Author : nam
 */ 

#include <avr/io.h>
#include <util/delay.h>

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

  ADMUX =
            (0 << ADLAR) |     // do not left shift result (for 10-bit values)
            (0 << REFS1) |     // Sets ref. voltage to internal 1.1V, bit 1
            (0 << REFS0) |     // Sets ref. voltage to internal 1.1V, bit 0
            (1 << MUX3)  |     // use ADC3 for input (PB3), MUX bit 3
            (0 << MUX2)  |     // use ADC3 for input (PB3), MUX bit 2
            (0 << MUX1)  |     // use ADC3 for input (PB3), MUX bit 1
            (0 << MUX0);       // use ADC3 for input (PB3), MUX bit 0

  ADCSRA = 
            (1 << ADPS2) |     // set prescaler to 128, bit 2
            (1 << ADPS1) |     // set prescaler to 128, bit 1
            (1 << ADPS0);      // set prescaler to 128, bit 0
}

uint16_t read_analog(int ch)
{
    ADCSRA |= (1 << ADEN);
    ch &= 0b00000111;  // AND operation with 7
    ADMUX = (ADMUX & 0xF8)|ch; // clears the bottom 3 bits before ORing
    
    // start single convertion
    // write ’1? to ADSC
    ADCSRA |= (1<<ADSC);
    
    // wait for conversion to complete
    // ADSC becomes ’0? again
    // till then, run loop continuously
    while(ADCSRA & (1<<ADSC));
    
    return (ADC);
    
}

#define LED_PORT PB1

int main(void)
{
    DDRB |= (1 << LED_PORT);
    
    initADC();
    
    volatile  int8_t adc_lobyte; // to hold the low byte of the ADC register (ADCL)
    volatile  uint16_t raw_adc;

    while(1)
    {

        ADCSRA |= (1 << ADSC);         // start ADC measurement
        while (ADCSRA & (1 << ADSC) ); // wait till conversion complete

        // for 10-bit resolution:
        adc_lobyte = ADCL; // get the sample value from ADCL
        raw_adc = ADCH<<8 | adc_lobyte;   // add lobyte and hibyte

        if (raw_adc > 512)
        {
            // ADC input voltage is more than half of the internal 1.1V reference voltage
            PORTB |= (1 << LED_PORT);
            _delay_ms(1200);
           
        } 
        else 
        {
             PORTB &= ~(1 << LED_PORT);
            // ADC input voltage is less than half of the internal 1.1V reference voltage

        }

    }
}

