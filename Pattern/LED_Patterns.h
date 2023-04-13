/***************************************************************************//**
 * @brief LED Patterns
 * 
 * @author Matthew Spinks
 * 
 * @date 7/10/22   Original creation
 * 
 * @file LED_Patterns.h
 * 
 * @details
 *      A list of LED patterns that work with my Pattern library. These are
 *  preprocessor define. This lets me put my declarations in a single
 * convenient file where they won't take up as much space. It also lets me 
 * choose whether or not I want to make my pattern variables global or static. 
 * In order to create a pattern, initialize an array of pattern states like so:
 * 
 * PatternState doubleBlinkPattern[DOUBLE_BLINK_SIZE] = DOUBLE_BLINK_PATT;
 * 
 * Remember to leave the semicolon off of the end of these defines! If you 
 * don't, you'll most likely get a very cryptic compiler error.
 * 
 * You technically don't have to have an array for every single pattern you
 * want to make. The pattern maker will loop back to zero once it hits the size 
 * that you initialized it as. You could make a temporary pattern array and 
 * copy from one array to a larger one if you desire.
 * 
 ******************************************************************************/

#ifndef LED_PATTERNS_H
#define LED_PATTERNS_H

#include "Pattern.h"

/* Remember to leave the semicolon off of the end of these defines. Also, I
like to make the last state an off state of some sort even for patterns that
loop. That way when I stop one, it's always in a known state. For these 
patterns I was using a tick rate of 8 ms, but the times don't have to be exact.
(for example, a lot of times I'll just put 250 ms instead 256 ms) */

/* Example code: 
    PatternState doubleBlink[DOUBLE_BLINK_SIZE] = DOUBLE_BLINK_PATT; */

// a single blink, once per second
#define SINGLE_BLINK_SIZE   3
#define SINGLE_BLINK_PATT   {{.output = 0, .timeInMs = 904},\
                             {.output = 1, .timeInMs = 32},\
                             {.output = 0, .timeInMs = 64}}

// two rapid blinks, once per second
#define DOUBLE_BLINK_SIZE   5
#define DOUBLE_BLINK_PATT   {{.output = 0, .timeInMs = 808},\
                             {.output = 1, .timeInMs = 32},\
                             {.output = 0, .timeInMs = 64},\
                             {.output = 1, .timeInMs = 32},\
                             {.output = 0, .timeInMs = 64}}

// flash both LED's at the same time
#define TWO_LED_FLASH_SIZE  2
#define TWO_LED_FLASH_PATT  {{.output = 0x03, .timeInMs = 250},\
                             {.output = 0x00, .timeInMs = 250}}

// alternate flashing LED's
#define TWO_LED_ALT_SIZE    3
#define TWO_LED_ALT_PATT    {{.output = 0x01, .timeInMs = 250},\
                             {.output = 0x02, .timeInMs = 250},\
                             {.output = 0x00, .timeInMs = 8}} // a small, un-noticeable, end state

#define FAST_BLINK_SIZE     2
#define FAST_BLINK_PATT     {{.output = 1, .timeInMs = 32},\
                             {.output = 0, .timeInMs = 468}}

// blink once, in three seconds
#define ERROR_CODE_1_SIZE   2
#define ERROR_CODE_1_PATT   {{.output = 1, .timeInMs = 250},\
                             {.output = 0, .timeInMs = 2750}}

#define ERROR_CODE_2_SIZE   4
#define ERROR_CODE_2_PATT   {{.output = 1, .timeInMs = 250},\
                             {.output = 0, .timeInMs = 250},\
                             {.output = 1, .timeInMs = 250},\
                             {.output = 0, .timeInMs = 2250}}

#define ERROR_CODE_3_SIZE   6
#define ERROR_CODE_3_PATT   {{.output = 1, .timeInMs = 250},\
                             {.output = 0, .timeInMs = 250},\
                             {.output = 1, .timeInMs = 250},\
                             {.output = 0, .timeInMs = 250},\
                             {.output = 1, .timeInMs = 250},\
                             {.output = 0, .timeInMs = 1750}}

#define ERROR_CODE_4_SIZE   8
#define ERROR_CODE_4_PATT   {{.output = 1, .timeInMs = 250},\
                             {.output = 0, .timeInMs = 250},\
                             {.output = 1, .timeInMs = 250},\
                             {.output = 0, .timeInMs = 250},\
                             {.output = 1, .timeInMs = 250},\
                             {.output = 0, .timeInMs = 250},\
                             {.output = 1, .timeInMs = 250},\
                             {.output = 0, .timeInMs = 1250}}

#endif  /* LED_PATTERNS_H */