/***************************************************************************//**
 * @brief Comparator Library Header File
 * 
 * @file Comp.h
 * 
 * @author Matthew Spinks <https://github.com/mspinksosu>
 * 
 * @date 5/7/22    Original creation
 * 
 * @details
 *      A comparator with multiple output levels. The output levels are based
 * on the number of deadzones. For example, a basic analog comparator will have
 * two output states and one deadzone area on the input. An analog button with 
 * three states (off, low, high) will have two deadzone areas. The number of 
 * outputs will be one greater than the number of deadzones. The minimum 
 * number of deadzones is one.
 * 
 * In order to create a comparator with multiple output levels, you will need 
 * to give it an array of deadzones. The deadzones are defined as an area with 
 * an upper and lower bound. They cannot overlap, otherwise your output may not 
 * change when in a particular state.
 * 
 * The input will be an analog value from 0-65535. The output will be a number 
 * that represents the output level. The output will be a number that goes from
 *  low to high starting at 0. If you comparator is just a basic, two output 
 * comparator with a single deadzone, the output is either 0 or 1. If it has 
 * two deadzones, the output is 0, 1, 2 and so on.
 * 
 * A flag is set whenever the output level changes and can be checked by
 * calling the get output changed event function. This flag is not cleared 
 * automatically.
 * 
 * @section example_code Example Code
 * 
 *      Comp underVoltateComp;
 *      CompDeadzone underVoltageThresholds[2] = {
 *      {.lower = LOW_VOLTAGE_SHUTOFF_LOW_THRESHOLD, // lowest analog value
 *       .upper = LOW_VOLTAGE_SHUTOFF_HIGH_THRESHOLD},
 *      {.lower = LOW_VOLTAGE_WARNING_LOW_THRESHOLD,
 *       .upper = LOW_VOLTAGE_WARNING_HIGH_THRESHOLD}}; // highest analog value
 * 
 *      Comp_Init(&underVoltageComp, underVoltageThresholds, 
 *        (sizeof(underVoltageThresholds) / sizeof(underVoltageThresholds[0])));
 * 
 *      Comp_UpdateValue(&underVoltageComp, battVoltage);
 * 
 *      if(Comp_GetOutputChangeEvent(&underVoltageComp))
 *      {
 *          Comp_ClearOutputChangedFlag(&underVoltageComp);
 *          uint8_t warningLevel = Comp_GetOutputLevel(&underVoltageComp);
 * 
 *          // warningLevel == 2: Okay
 *          // warningLevel == 1: Low voltage warning
 *          // warningLevel == 0: Shutdown now!
 *      }
 * 
 * @section license License
 * SPDX-FileCopyrightText: © 2022 Matthew Spinks
 * SPDX-License-Identifier: Zlib
 * 
 * This software is released under the Zlib license. You are free alter and
 * redistribute it, but you must not misrepresent the origin of the software.
 * This notice may not be removed. <http://www.zlib.net/zlib_license.html>
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
 * Description of struct members. You shouldn't really mess with any of these
 * variables directly. That is why I made functions for you to use.
 * 
 * upper  The upper boundary of the deadzone. It is a closed region which 
 *        includes this value.
 * 
 * lower  The lower boundary of the deadzone. Should be greater than zero. It 
 *        is a closed region which includes this value.
 * 
 * deadzone  Pointer to the deadzone or array of deadzones
 * 
 * numDeadZones  The number of deadzones given
 * 
 * outputLevel  The output level. 0 to numDeadZones
 * 
 * outputChangedEvent  Is set whenever the output level changes. This flag is 
 *                     not cleared automatically.
 */

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Function Prototypes *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/***************************************************************************//**
 * @brief Initializes a Comparator object
 * 
 * Goes through the array of deadzones that you created and attempts to fix any
 * issues with it. Your deadzones should be in order from lowest to highest 
 * and preferrably not overlap with one another.
 * 
 * The number of output levels is one greater than the deadzones. i.e. a basic
 * comparator has two outputs, low/high, and one deadzone in the middle.
 * 
 * @param self  pointer to the Comparator that you are using
 * 
 * @param deadzones  pointer to a deadzone object or an array of deadzones
 * 
 * @param numDeadzones  the number of deadzone objects in the array
 */
void Comp_Init(Comp *self, CompDeadzone *deadzones, uint8_t numDeadzones);

/***************************************************************************//**
 * @brief Update the Comparator object with its current value
 * 
 * This function should be called periodically
 * 
 * @param self  pointer to the Comparator that you are using
 * 
 * @param analogInput  0 to 65535
 */
void Comp_UpdateValue(Comp *self, uint16_t analogInput);

/***************************************************************************//**
 * @brief Check if there has been an output changed event
 * 
 * @param self  pointer to the Comparator that you are using
 * 
 * @return  true if the output has changed
 */
bool Comp_GetOutputChangedEvent(Comp *self);

/***************************************************************************//**
 * @brief Clear the output changed event flag
 * 
 * @param self  pointer to the Comparator that you are using
 */
void Comp_ClearOutputChangedFlag(Comp *self);

/***************************************************************************//**
 * @brief Return the current output level of the Comparator object
 * 
 * The number of different levels is one greater than the amount of deadzones
 * that you define for your Comparator object
 * 
 * @param self  pointer to the Comparator that you are using
 * 
 * @return uint8_t  output level
 */
uint8_t Comp_GetOutputLevel(Comp *self);

#endif /* COMP_H */