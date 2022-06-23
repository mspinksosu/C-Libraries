/***************************************************************************//**
 * @brief Comparator Library Header File
 * 
 * @author Matthew Spinks
 * 
 * @date 5/5/22     Original creation
 * 
 * @file Comp.h
 * 
 * @details
 *      A comparator with multiple output levels. I decided to define the
 * outputs based on the number of deadzones. This way the comparator can have
 * hysteresis. The number of outputs should be 1 greater than the number of 
 * deadzones. For example an analog button with three states (off, low, high)
 * will have two deadzone areas. The minimum number of deadzones is 1. This 
 * would be a standard comparator with two outputs states: on and off.
 * 
 * In order to create a comparator with multiple output levels, you will need
 * to give it an array of deadzones. The deadzones are defined as an area with 
 * an upper and lower bound. They cannot overlap, otherwise your output may not 
 * change when in a particular state.
 * 
 * The input will be an analog value from 0-65535. The output will be a number 
 * that represents the output level. The output will go from low to high 
 * starting at 0. This makes a convenient way to interface with if-statements
 * if you choose. If you comparator is just a basic, two output comparator with
 * a single deadzone define, the output is either 0 or 1. If it has two
 * deadzones, the output is 0, 1, 2 and so on.
 * 
 ******************************************************************************/

#ifndef COMP_H
#define COMP_H

#include <stdint.h>
#include <stdbool.h>

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

typedef struct CompDeadzoneTag
{
    uint16_t upper;
    uint16_t lower;
} CompDeadzone;

typedef struct CompTag
{
    CompDeadzone *deadzones;
    uint8_t numDeadzones;
    uint8_t outputLevel;
    bool outputChangedEvent;
} Comp;

/** 
 * Description of struct
 * 
 * member1      description of variable member1
 * 
 */

// ***** Function Prototypes ***************************************************

void Comp_Create(Comp *self, CompDeadzone *deadzones, uint8_t numDeadzones);

// -----------------------------------------------------------------------------

void Comp_Process(Comp *self, uint16_t analogInput);

bool Comp_GetOutputChangedEvent(Comp *self);

void Comp_ClearOutputChangedFlag(Comp *self);

uint8_t Comp_GetOutputLevel(Comp *self);


#endif	/* COMP_H */