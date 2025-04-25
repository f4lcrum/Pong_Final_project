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
#include "lfsr.h"
#include <util/delay.h>


volatile bool paddle_down = false;
volatile bool paddle_up = false;
volatile bool restart_game = false;
volatile bool pause_game = true;
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

static void reset_paddle(Paddle *p) {
    p->x_pos = MATRIX_BOUNDARY_LOW;
    p->y_pos = 3;
}

static void reset_ball(Ball *b) {
    // TO-DO randomize x-y_direction
    b->x = 3;
    b->y = 4;
    b->x_direction = lfsr_randrange(0, 2) ? -1 : 1;
    b->y_direction = lfsr_randrange(0, 2) ? -1 : 1;
}

ISR(PORTE_PORT_vect, ISR_BLOCK) {
    // if upper button, paddle is going up
    if (PORTE.INTFLAGS & PIN0_bm) {
        PORTE.INTFLAGS |= PIN0_bm;
        if (!pause_game) {
            paddle_up = true;
            paddle_down = false;
        }
    }
    // same but for down button and paddle is going down (depends how your display is positioned tho)
    if (PORTE.INTFLAGS & PIN1_bm) {
        PORTE.INTFLAGS |= PIN1_bm;
        if (!pause_game) {
            paddle_down = true;
            paddle_up = false;
        }
    }

    if (PORTE.INTFLAGS & PIN2_bm) {
        PORTE.INTFLAGS |= PIN2_bm;
        restart_game = true;
        pause_game = false;
    }

    if (PORTE.INTFLAGS & PIN3_bm) {
        PORTE.INTFLAGS |= PIN3_bm;
        pause_game = !pause_game;
        restart_game = false;
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

void pause() {
    while (pause_game) {
        // game was paused - allow only setting of brightness
        set_brightness();
    }
}

void reset(Paddle *p, Ball *b) {
    reset_paddle(p);
    reset_ball(b);
    lfsr_seed(seed);
}

void restart(Paddle *p, Ball *b) {
    reset(p, b);
    restart_game = false;
    pause_game = true;
    clear_display();
}

void random_ball_bounce(Ball *b) {
    lfsr_seed(seed);
    uint8_t chance = lfsr_randrange(0, 2);

    if (chance % 2 == 0) {
        if (b->y_direction == 0) {
            b->y_direction = chance < 128 ? 1 : -1;
        }
        b->y_direction = b->y_direction * -1;
    } else {
        b->y_direction = 0;
    }

    b->x_direction = b->x_direction * -1;
}

void update_ball(Paddle *p, Ball *b) {
    // to prevent "mirage effect" ( or call it "Sandevistan" effect from cyberpunk: edgerunners )
    if (!(is_in_paddle_range(p, b->x, b->y) || is_in_paddle_range(p, b->x, b->y))) {
        set_led(b->x, b->y, false);
    }


    // check for ball interaction with paddle
    if (b->x == MATRIX_BOUNDARY_LOW) {
        if (is_in_paddle_range(p, b->x, b->y)) {
            random_ball_bounce(b);
        } else {
            restart_game = true;
        }
    }

    // bouncing off ceiling
    if (b->y == MATRIX_BOUNDARY_HIGH || b->y == MATRIX_BOUNDARY_LOW) {
        b->y_direction = b->y_direction * -1;
    }

    // bounce of wall opposite to paddle
    if (b->x == MATRIX_BOUNDARY_HIGH) {
        b->x_direction = b->x_direction * -1;
    }

    b->x = b->x + b->x_direction;
    b->y = b->y + b->y_direction;

    set_led(b->x, b->y, true);
    _delay_ms(2000);
}

void play() {
    setup();
    Paddle p1;
    Ball b;
    reset(&p1, &b);

    while (1) {
        set_brightness();
        update_paddle(&p1);
        update_ball(&p1, &b);
        if (pause_game) pause();
        if (restart_game) restart(&p1, &b);

    }
}
