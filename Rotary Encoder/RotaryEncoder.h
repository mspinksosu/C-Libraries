/***************************************************************************//**
 * @brief Rotary Encoder Library (with switch)
 * 
 * @author Matthew Spinks
 * 
 * @date 10/2/16    Original Creation
 * @date 2/21/22    Added Doxygen
 * 
 * @file RotaryEncoder.h
 * 
 * @details
 *      Functions for a generic rotary encoder with switch. This is a
 *      quadrature rotary encoder with the number of pulses per revolution
 *      equal to half the detents. This is the most common type of rotary
 *      encoder.
 * 
 *      One phase of the rotary encoder should have both rising and falling
 *      edge interrupts enabled. It will look at a rising or falling edge
 *      on channel A and compare it to channel B to determine the direction 
 *      that the encoder was rotated.
 * 
 * ****************************************************************************/

#ifndef ROTARYENCODER_H
#define	ROTARYENCODER_H

// ***** Includes **************************************************************

#include <stdbool.h>

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************


// ***** Function Prototypes ***************************************************

void RE_RotateInterrupt(bool AisHigh, bool BisHigh);

void RE_SwitchInterrupt(bool isPressed);

bool RE_GetClockwise(void);

bool RE_GetCounterClockwise(void);

bool RE_GetSwitchPress(void);

bool RE_GetSwitchRelease(void);

#endif	/* ROTARYENCODER_H */

