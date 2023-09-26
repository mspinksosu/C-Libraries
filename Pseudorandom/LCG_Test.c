/* Program to test PRNG_LCG - MS */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "PRNG.h"

int main(void)
{
    uint32_t seed, result, n;
    char outName[64];
    FILE *out;
    LCG lcg;

    printf("Enter a seed value: ");
    scanf("%d", &seed);
    printf("Entered: %u\n", seed);
    PRNG_LCGSeed(&lcg, seed);

    /* Print out numbers to a file */
    printf("Enter number of values to output: ");
    scanf("%d", &n);
    printf("Entered: %u\n", n);
    printf ("Save as type: .csv - File name: ");
    scanf ("%59s", outName);
    strcat(outName, ".csv");
    out = fopen(outName, "w+");
    printf ("Writing output to %s...\n", outName);
    for(uint32_t i = 0; i < n; i++)
    {
        result = PRNG_LCGNext(&lcg);
        fprintf(out, "%u,\n", result);
    }
    printf("Finished.\n");
    return 0;
}