/* 
 * File:   max7219.h
 * Author: Kristian Oravec
 *
 * Created on April 11, 2025, 7:48 PM
 */

#ifndef MAX7219_H
#define	MAX7219_H

#ifdef	__cplusplus
extern "C" {
#endif

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
    
    
static void SPI0_init(void);
static void clientSelect(void);
static void clientDeselect(void);
static uint8_t SPI0_exchangeData(uint8_t data);

void sendToDisplay(uint8_t reg, uint8_t data);
void init(uint8_t intensity);



#ifdef	__cplusplus
}
#endif

#endif	/* MAX7219_H */

