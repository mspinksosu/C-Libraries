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
    PRNG prng;
    PRNGBig lcg;

    // Create manually for now
    prng.instance = &lcg;
    lcg.super = &prng;
    prng.type = PRNG_TYPE_LCG_BIG;

    printf("Enter a seed value: ");
    scanf("%d", &seed);
    printf("Entered: %u\n", seed);
    PRNGBig_Seed(&lcg, seed);

    /* Print out comma separated numbers to a file */
    printf("Enter number of values to output: ");
    scanf("%d", &n);
    printf("Entered: %u\n", n);
    printf("Save as type: .csv - File name: ");
    scanf("%59s", outName);
    strcat(outName, ".csv");
    out = fopen(outName, "w");
    printf("Writing output to %s...\n", outName);
    for(uint32_t i = 0; i < n; i++)
    {
        result = PRNGBig_Next(&lcg);
        fprintf(out, "%u,\n", result);
    }
    printf("Finished.\n");
    fclose(out);
    return 0;
}