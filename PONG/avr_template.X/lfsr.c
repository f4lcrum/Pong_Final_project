/*
 * File:   lfsr.c
 * Author: kristian
 *
 * Created on April 18, 2025, 2:26 AM
 */


#include <xc.h>

// Internal LFSR state
static uint8_t lfsr_state = 0xACE1; // default seed

// Seed function

void lfsr_seed(uint8_t seed) {
    if (seed != 0)
        lfsr_state = seed;
    else
        lfsr_state = 0xACE1; // prevent stuck zero
}

// Generate next pseudo-random number (16-bit)

uint8_t lfsr_rand() {
    // Tap: 16 14 13 11 (bits 0-based: 0xB400)
    uint8_t lsb = lfsr_state & 1; // Get LSB (bit 0)
    lfsr_state >>= 1; // Shift register
    if (lsb)
        lfsr_state ^= 0xB400; // XOR with tap mask
    return lfsr_state;
}

// Generate random number in range [min, max)

uint8_t lfsr_randrange(uint8_t min, uint8_t max) {
    return (lfsr_rand() % (max - min)) +min;
}
