/* Program to test PRNG_LCG - MS */

#include <stdio.h>
#include <stdlib.h>
#include "PRNG.h"

int main(void)
{
    uint32_t seed, result;
    ParkMiller pm;

    printf("Enter a seed value.\n");
    scanf("%d", &seed);
    PRNG_ParkMillerSeed(&pm, seed);
    printf("seed value: %u\n", seed);

    /* Print out the first 16 numbers in the sequence */
    for(uint32_t i = 0; i < 16; i++)
    {
        result = PRNG_ParkMillerNext(&pm);
        printf("%u\n", result);
    }

    return 0;
}