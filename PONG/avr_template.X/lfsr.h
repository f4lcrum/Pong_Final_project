/* 
 * File:   lfsr.h
 * Author: kristian
 *
 * Created on April 18, 2025, 2:27 AM
 */

#include <stdint.h>

#ifndef LFSR_H
#define	LFSR_H

#ifdef	__cplusplus
extern "C"
{
#endif

void lfsr_seed(uint8_t seed);
uint8_t lfsr_rand();
uint8_t lfsr_randrange(uint8_t min, uint8_t max);

#ifdef	__cplusplus
}
#endif

#endif	/* LFSR_H */

