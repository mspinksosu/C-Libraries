/* Program to demonstrate LCG bias removal method - MS */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "PRNG.h"

int main(void)
{
    uint32_t seed, result, n, choice, lower, upper;
    uint32_t randMax = 0xFFFFFFFF;
    char outName[64];
    FILE *out;
    PRNG prng;
    char c, removeBias;
    char option[2][22] = {"LCG No Bias Removal",
                          "LCG With Bias Removal"};
    while(1)
    {
        printf("Select type:\n1. %s\n2. %s\n", 
            option[0], option[1]);
        printf("Enter q to quit.\n");
        scanf(" %c", &c);

        switch(c)
        {
            case '1':
                choice = 0;
                prng.type = PRNG_TYPE_LCG_BIG;
                removeBias = 0;
                break;
            case '2':
                choice = 1;
                prng.type = PRNG_TYPE_LCG_BIG;
                removeBias = 1;
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
        printf("Enter rand max in hex without \"0x\": ");
        scanf("%x", &randMax);
        printf("Entered: 0x%x\n", randMax);
        // printf("LCG Seed: %llu\n", prng.state.u64);

        /* Print out comma separated numbers to a file */
        printf("Enter number of values to output: ");
        scanf("%d", &n);
        printf("Entered: %u\n", n);
        printf("Enter lower bound: ");
        scanf("%d", &lower);
        printf("Enter upper bound: ");
        scanf("%d", &upper);
        printf("Save as type: .csv - File name: ");
        scanf("%59s", outName);
        strcat(outName, ".csv");
        out = fopen(outName, "w");
        printf("Writing output to %s...\n", outName);


        /* output = output % (upper - lower + 1) + min */
        uint32_t range = upper - lower;
        if(range < 0xFFFFFFFF)
            range++;
        
        /* If no bias removal is selected, skip the end of the do-while */
        uint32_t threshold = 0;
        if(removeBias)
            threshold = randMax - randMax % range;

        for(uint32_t i = 0; i < n; i++)
        {
            /* The loop and threshold are the method for removing modulo bias.
            The step in the middle that performs modulus with randMax + 1 is so 
            that this test can use a variable maximum limit to demonstrate how 
            the bit width of the random generator's output can affect the bias. */
            do {
                result = PRNG_Next(&prng);
                result = result % (randMax + 1);
            } while(removeBias && result >= threshold);

            result = (result % range) + lower;
            fprintf(out, "%u,\n", result);
        }
        printf("Finished.\n");
        fclose(out);
    }
}