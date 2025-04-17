/*
 * File:   pong.c
 * Author: kristian
 *
 * Created on April 17, 2025, 5:40 PM
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/builtins.h>
#include <avr/wdt.h>
#include "pong.h"
#include "max7219.h"


volatile bool paddle_down = false;
volatile bool paddle_up = false;
volatile bool start_game = false;
volatile bool stop_game = true;
volatile uint8_t seed = 0;

static void set_adc() {
    // ADC is for potentiometer, need to set brightness of matrix display;
    ADC0.CTRLC = ADC_SAMPCAP_bm | ADC_REFSEL_VDDREF_gc
            | ADC_PRESC_DIV256_gc;
    // select input pin, POT1 for brightness settings
    ADC0.MUXPOS = ADC_MUXPOS_AIN1_gc;
    ADC0.CTRLA |= ADC_ENABLE_bm | ADC_RESSEL_bm;

}

static void set_timer() {
    // Timer for pseudo-random seed -- generating initial direction for a ball
    TCB0.CCMP = 100;
    TCB0.INTCTRL = 1;
    TCB0.CTRLA = 1;
}

static void set_buttons() {
    // PORTE interrupts -
    // SW1 SW4 --> controlling paddle
    // SW2 SW3 --> RESTART/PAUSE game
    PORTE.PIN0CTRL = PORT_ISC_FALLING_gc;
    PORTE.PIN1CTRL = PORT_ISC_FALLING_gc;
    PORTE.PIN2CTRL = PORT_ISC_FALLING_gc;
    PORTE.PIN3CTRL = PORT_ISC_FALLING_gc;
}

ISR(PORTE_PORT_vect, ISR_BLOCK) {
    // if upper button, paddle is going up
    if (PORTE.INTFLAGS & PIN0_bm) {
        PORTE.INTFLAGS |= PIN0_bm;
        paddle_up = true;
        paddle_down = false;
    }
    // same but for down button and paddle is going down (depends how your display is positioned tho)
    if (PORTE.INTFLAGS & PIN1_bm) {
        PORTE.INTFLAGS |= PIN1_bm;
        paddle_down = true;
        paddle_up = false;
    }

    if (PORTE.INTFLAGS & PIN2_bm) {
        PORTE.INTFLAGS |= PIN2_bm;
        start_game = true;
        stop_game = false;
    }

    if (PORTE.INTFLAGS & PIN3_bm) {
        PORTE.INTFLAGS |= PIN3_bm;
        start_game = false;
        stop_game = true;
    }
}

// timer B0 interrupt

ISR(TCB0_INT_vect, ISR_BLOCK) {
    TCB0.INTFLAGS = 1;
    seed++;
}

bool is_in_paddle_range(Paddle *p, uint8_t x, uint8_t y) {
    // check whether x, y  (f.e. coords of ball) is in range of paddle
    return y >= p->y_pos - 1 && y <= p->y_pos + 1 && p->x_pos == x;
}

void paddle_move(Paddle *p) {
    // update movement of paddle on display
    for (uint8_t y = 0; y < 8; y++) {
        set_led(p->x_pos, y, is_in_paddle_range(p, p->x_pos, y));
    }
}

void set_brightness() {
    // By rotating a potentiometer we will set intensity of MATRIX display
    ADC0.COMMAND |= ADC_STCONV_bm;
    while (ADC0.COMMAND) {
    }

    set_intensity(ADC0.RES >> 4); // Matrix knows only 0-15, so divide it by 16 => shift 4
}

void update_paddle(Paddle *p) {
    // position of paddle is defined by middle LED of 3-LED paddle - therefore - 1 to NOT cut upper part of paddle
    if (paddle_up && p->y_pos < MATRIX_BOUNDARY_HIGH - 1) {
        p->y_pos++;
    }
    paddle_up = false;

    // position of paddle is defined by middle LED of 3-LED paddle - therefore - 1 to NOT cut lower part of paddle
    if (paddle_down && p->y_pos > MATRIX_BOUNDARY_LOW + 1) {
        p->y_pos--;
    }

    paddle_down = false;


    paddle_move(p);
}

void setup() {
    cli(); //disable interrupts global

    // set clock prescaler to 0: CPU runs at 20MHz
    CPU_CCP = CCP_IOREG_gc;
    CLKCTRL.MCLKCTRLB = 0x00;

    // Init matrix display (1 = lowest brightness, 15 = highest )
    init(1);

    set_adc();
    set_timer();
    set_buttons();
    sei(); //enable interrupts global
}

void play() {
    Paddle p1 = {.x_pos = MATRIX_BOUNDARY_LOW, .y_pos = 3};
    // Ball b = {.x = 3, .y = 4, .x_direction = 1, .y_direction = 1}; // TO-DO randomize x-y_direction

    while (1) {
        set_brightness();
        update_paddle(&p1);
    }
}
