// Random - Random number generator

#include "Random.h"

// Numerical Recipes LCG constants. The additive term prevents a zero lock,
// so any seed (including 0) yields a usable sequence.
static uint32_t s_state = 1;

void rngSeed(uint32_t seed)
{
    s_state = seed;
}

uint8_t rngBelow(uint8_t limit)
{
    s_state = s_state * 1664525uL + 1013904223uL;
    // Use the high 16 bits (better quality than the low bits of an LCG)
    // and map to 0..limit-1 via multiply-shift to dodge modulo bias.
    uint16_t hi = (uint16_t)(s_state >> 16);
    return (uint8_t)(((uint32_t)hi * limit) >> 16);
}
