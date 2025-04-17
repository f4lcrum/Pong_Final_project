/* 
 * File:   pong.h
 * Author: kristian
 *
 * Created on April 17, 2025, 5:43 PM
 */

#ifndef PONG_H
#define	PONG_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#ifdef	__cplusplus
}
#endif

#endif	/* PONG_H */


#define MATRIX_BOUNDARY_LOW 0
#define MATRIX_BOUNDARY_HIGH 7

typedef struct {
    int x;
    int y;
    int x_direction; 
    int y_direction;
} Ball;

typedef struct {
    // position is defined by position of middle led of 3-led paddle. 
    int x_pos; 
    int y_pos;
} Paddle;

bool is_in_paddle_range(Paddle *p, uint8_t x, uint8_t y);
void play();
void set_brightness();
void update_paddle(Paddle *p);
void setup();