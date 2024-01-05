/* Program to test PRNG_Skip - MS */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "PRNG.h"

int main(void)
{
    uint32_t seed, result, choice;
    int64_t n;
    FILE *out;
    PRNG prng;
    char c;
    char option[4][12] = {"LCG Big",
                          "LCG Small",
                          "Park Miller",
                          "Schrage"};
    while(1)
    {
        printf("Select type:\n1. %s\n2. %s\n3. %s\n4. %s\n", 
            option[0], option[1], option[2], option[3]);
        printf("Enter q to quit.\n");
        scanf(" %c", &c);

        switch(c)
        {
            case '1':
                choice = 0;
                prng.type = PRNG_TYPE_LCG_BIG;
                break;
            case '2':
                choice = 1;
                prng.type = PRNG_TYPE_LCG_SMALL;
                break;
            case '3':
                choice = 2;
                prng.type = PRNG_TYPE_PARK_MILLER;
                break;
            case '4':
                choice = 3;
                prng.type = PRNG_TYPE_SCHRAGE;
                break;
            case 'Q':
            case 'q':
                fclose(out);
                exit(0);
            break;
        }
        printf("Selected: %s\n", option[choice]);
        printf("Enter a seed value: ");
        scanf("%d", &seed);
        printf("Entered: %u\n", seed);
        PRNG_Seed(&prng, seed);
        // printf("LCG Seed: %llu\n", prng.state.u64);

        /* Compute n + k terms */
        printf("Enter number to skip: n = ");
        /* Must use %lld for scanf this time or passing to function won't work 
        right for some reason. Even with the number 1. */
        scanf("%lld", &n);
        result = PRNG_Skip(&prng, n);
        printf("nth term = %u\n", result);
    }
}