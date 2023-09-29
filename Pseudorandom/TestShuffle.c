/* Program to test PRNG_LCG - MS */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "PRNG.h"

void printArray(uint16_t *array, uint16_t n)
{
    for(uint8_t i = 0, j = 0; i < n; i++)
    {
        printf("%2u, ",array[i]);
        if(++j == 4)
        {
            printf("\n");
            j = 0;
        }
    }
    printf("\n");
}

int main(void)
{
    uint16_t array1[] = {0, 1, 2, 3, 4, 5, 6, 7, 8,
                         9, 10, 11, 12, 13, 14, 15};
    uint16_t array2[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 
                         16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27};
    uint32_t seed;
    uint16_t n;

    printf("Enter a seed value: ");
    scanf("%d", &seed);
    printf("Entered: %u\n", seed);
    // PRNG_Seed(&prng, seed);
    // printf("LCG Seed: %llu\n", prng.state.u64);

    n = sizeof(array1) / sizeof(array1[0]);
    PRNG_Shuffle(&array1, n, sizeof(array1[0]), seed);
    printf("Array 1:\n");
    printArray(array1, n);

    /* Test if two arrays with the same seed are similiar or not */
    n = sizeof(array2) / sizeof(array2[0]);
    PRNG_Shuffle(&array2, n, sizeof(array2[0]), seed);
    printf("Array 2:\n");
    printArray(array2, n);

    return 0;
}