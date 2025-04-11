/*
 * File:   max7219.c
 * Author: kristian
 *
 * Created on April 11, 2025, 7:48 PM
 */


#include <xc.h>
#include "max7219.h"

static void SPI0_init(void)
{
    PORTA.DIR |= PIN4_bm; /* Set MOSI pin direction to output */
    PORTA.DIR &= ~PIN5_bm; /* Set MISO pin direction to input */
    PORTA.DIR |= PIN6_bm; /* Set SCK pin direction to output */
    PORTA.DIR |= PIN7_bm; /* Set SS pin direction to output */

    SPI0.CTRLA = SPI_ENABLE_bm          /* Enable module */
               | SPI_MASTER_bm          /* SPI module in Master mode */
               | SPI_PRESC_DIV16_gc;    /* System Clock divided by 16 */
}

static uint8_t SPI0_exchangeData(uint8_t data)
{
    SPI0.DATA = data;

    while (!(SPI0.INTFLAGS & SPI_IF_bm))  /* waits until data is exchanged*/
    {
        ;
    }

    return SPI0.DATA;
}

static void clientSelect(void)
{
    PORTA.OUT &= ~PIN7_bm; // Set SS pin value to LOW
}

static void clientDeselect(void)
{
    PORTA.OUT |= PIN7_bm; // Set SS pin value to HIGH
}


void sendToDisplay(uint8_t reg, uint8_t data) {
    clientSelect();
    SPI0_exchangeData(reg);
    SPI0_exchangeData(data);
    clientDeselect();
}

void init(uint8_t intensity) {
    if (intensity > 15) intensity = 15;
    
    SPI0_init();
    
    sendToDisplay(SCANLIMIT, 7);   // show all 8 digits
    sendToDisplay(DECODEMODE, 0);  // using an led matrix (not digits)
    sendToDisplay(DISPLAYTEST, 0); // no display test
    
    for (uint8_t i = 0; i < 8; i++ ) {
        sendToDisplay(i + 1, 255);
    }
    
    sendToDisplay(INTENSITY, intensity);
    sendToDisplay(SHUTDOWN, 1);
}