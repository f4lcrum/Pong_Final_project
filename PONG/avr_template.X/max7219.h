/* 
 * File:   max7219.h
 * Author: Kristian Oravec
 *
 * Created on April 11, 2025, 7:48 PM
 */

#ifndef MAX7219_H
#define	MAX7219_H

#include <stdbool.h>

#ifdef	__cplusplus
extern "C" {
#endif

// Commands from datasheet
#define NOOP 0x0
#define DIGIT0 0x1
#define DIGIT1 0x2
#define DIGIT2 0x3
#define DIGIT3 0x4
#define DIGIT4 0x5
#define DIGIT5 0x6
#define DIGIT6 0x7
#define DIGIT7 0x8
#define DECODEMODE 0x9
#define INTENSITY  0xA
#define SCANLIMIT  0xB
#define SHUTDOWN   0xC
#define DISPLAYTEST 0xF

void send_to_display(uint8_t reg, uint8_t data);
void init(uint8_t intensity);
void set_intensity(uint8_t intensity);
void clear_display();
void set_led(uint8_t x, uint8_t y, bool on);
void pause();


#ifdef	__cplusplus
}
#endif

#endif	/* MAX7219_H */

