// Random - Random number generator

#ifndef RANDOM_H
#define RANDOM_H

#include <stdint.h>

// Lightweight LCG pseudo-random generator.
// Seed with a timestamp captured at game start.
void    rngSeed(uint32_t seed);
uint8_t rngBelow(uint8_t limit);   // returns 0 .. limit-1

#endif
