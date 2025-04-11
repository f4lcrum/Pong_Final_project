/*
 * File:   main.c
 * Author: loady
 *
 * Created on March 18, 2024, 2:43 PM
 */

#include <avr/io.h>
#include <avr/interrupt.h>

int main(void) {
    
    cli(); //disable interrupts global
    
    // set clock prescaler to 0: CPU runs at 20MHz
    CPU_CCP = CCP_IOREG_gc;
    CLKCTRL.MCLKCTRLB = 0x00;
    
    // LEDs out
    PORTC.DIR = 0xff;
    

    
    sei(); //enable interrupts global
    
    while (1) {
        PORTC.OUT = 0xAA;
    }
}
