/***************************************************************************//**
 * @brief Number To ASCII
 * 
 * @author Matthew Spinks
 * 
 * @date 4/9/23    Original creation
 * 
 * @file NumberToAscii.c
 * 
 * @details
 *      Sometimes using sprintf can cost a lot of memory. If you don't need
 * to create large strings of numbers or floating point numbers, this function 
 * could be useful for you. I've even included the option to have left or right
 * justification.
 * 
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/***************************************************************************//**
 * @brief A simple function to convert an int to a string
 * 
 * Creates a null null terminated string of the number you give it. Accepts a
 * pointer to a string and the width of the string. Make sure the width you 
 * give has enough room to hold your number plus a null terminator. The number 
 * will be padded with blank spaces.
 * 
 * @param num  number to be converted
 * @param str  pointer to a string where the output will be placed
 * @param strWidth  the width of the string (minimum is 2)
 * @param leftJust  if true, left justify the resulting string
 */
void NumberToAscii(int16_t num, uint8_t *str, uint8_t strWidth, bool leftJust)
{
    /* The width should include a space for a null terminator. If the width is 
    wrong, place a null terminator before exiting */
    if(strWidth < 2)
        strWidth = 1;
    str[--strWidth] = '\0';
    if(strWidth == 0)
        return;

    uint16_t temp = abs(num);
    uint8_t numDigits = 0, numStart = 0, numEnd = strWidth - 1;

    do {
        temp /= 10;
        numDigits++;
    } while(temp > 0 && numDigits <= strWidth);

    /* Pad the number string with spaces */
    if(leftJust)
    {
        numEnd = numDigits - 1;
        for(uint8_t i = strWidth - 1; i > numEnd; i--)
            str[i] = ' ';
    }
    else
    {
        numStart = strWidth - numDigits;
        for(uint8_t i = 0; i < numStart; i++)
            str[i] = ' ';
    }

    /* Print a negative sign if needed */
    if(num < 0)
    {
        num = -num;
        if(leftJust == false && numStart > 0)
        {
            str[numStart-1] = '-';
        }
        else
        {
            /* Push the number string to the right one space */
            str[numStart] = '-';
            numStart++;
            if(numEnd < strWidth - 1)
                numEnd++;
        }
    }

    /* Perform the conversion */
    uint8_t i = numEnd + 1;
    do {
        str[i-1] = num % 10 + '0';
        num /= 10;
        i--;
    } while(i > numStart && num > 0);
}

/*
 End of File
 */