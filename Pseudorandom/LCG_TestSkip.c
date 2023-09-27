/* Program to test PRNG_LCG - MS */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "PRNG.h"

int main(void)
{
    uint32_t seed, result;
    int64_t n;
    char outName[64];
    FILE *out;
    LCG lcg;

    printf("Enter a seed value: ");
    scanf("%d", &seed);
    printf("Entered: %u\n", seed);
    PRNG_LCGSeed(&lcg, seed);

    /* Compute n + k terms */
    printf("Enter number to skip: n = ");
    /* Must use %lld for scanf this time or passing to function won't work 
    right for some reason. Even with the number 1. */
    scanf("%lld", &n);
    result = PRNG_LCGSkip(&lcg, n);
    printf("nth term = %u\n", result);

    return 0;
}