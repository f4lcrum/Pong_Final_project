/*
 * File:   max7219.c
 * Author: kristian
 *
 * Created on April 11, 2025, 7:48 PM
 */


#include <xc.h>
#include "max7219.h"
#include <stdbool.h>

uint8_t led_status[8]; // array of LEDs

static void SPI0_init(void) {
    PORTA.DIR |= PIN4_bm; /* Set MOSI pin direction to output */
    PORTA.DIR &= ~PIN5_bm; /* Set MISO pin direction to input */
    PORTA.DIR |= PIN6_bm; /* Set SCK pin direction to output */
    PORTA.DIR |= PIN7_bm; /* Set SS pin direction to output */

    SPI0.CTRLA = SPI_ENABLE_bm /* Enable module */
            | SPI_MASTER_bm /* SPI module in Master mode */
            | SPI_PRESC_DIV16_gc; /* System Clock divided by 16 */
}

static uint8_t SPI0_exchangeData(uint8_t data) {
    SPI0.DATA = data;

    while (!(SPI0.INTFLAGS & SPI_IF_bm)) /* waits until data is exchanged*/ {
        ;
    }

    return SPI0.DATA;
}

static void client_select(void) {
    PORTA.OUT &= ~PIN7_bm; // Set SS pin value to LOW
}

static void client_deselect(void) {
    PORTA.OUT |= PIN7_bm; // Set SS pin value to HIGH
}

static void set_led_on(uint8_t x, uint8_t y) {
    led_status[y] |= 1 << x;
}

static void set_led_off(uint8_t x, uint8_t y) {
    led_status[y] &= ~(1 << x);
}

void send_to_display(uint8_t reg, uint8_t data) {
    client_select();
    SPI0_exchangeData(reg);
    SPI0_exchangeData(data);
    client_deselect();
}

void set_intensity(uint8_t intensity) {
    send_to_display(INTENSITY, intensity);
}

void clear_display() {
    for (int i = 0; i < 8; i++) {
        led_status[i] = 0;
    }
}

void set_led(uint8_t x, uint8_t y, bool on) {
    if (on) {
        set_led_on(x, y);
    } else {
        set_led_off(x, y);
    }

    for (int i = 0; i < 8; i++) {
        send_to_display(i + 1, led_status[i]);
    }
}

void init(uint8_t intensity) {

    if (intensity > 15) intensity = 15;

    SPI0_init();

    send_to_display(SCANLIMIT, 7); // show all 8 digits
    send_to_display(DECODEMODE, 0); // using an led matrix (not digits)
    send_to_display(DISPLAYTEST, 0); // no display test

    clear_display();

    send_to_display(1, 1);

    set_intensity(intensity);
    send_to_display(SHUTDOWN, 1);
}