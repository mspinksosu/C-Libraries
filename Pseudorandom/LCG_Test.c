/* Program to test PRNG_LCG - MS */

#include <stdio.h>
#include <stdlib.h>
#include "PRNG.h"

int main(void)
{
    uint32_t seed, result;
    LCG lcg;

    printf("Enter a seed value.\n");
    scanf("%d", &seed);
    PRNG_LCGSeed(&lcg, seed);
    printf("seed value: %u\n", seed);

    /* Print out the first 16 numbers in the sequence */
    for(uint32_t i = 0; i < 16; i++)
    {
        result = PRNG_LCGNext(&lcg);
        printf("%u\n", result);
    }

    return 0;
}