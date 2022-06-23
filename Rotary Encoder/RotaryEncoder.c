/***************************************************************************//**
 * @brief Rotary Encoder Library Header
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
 *      The flags for the getters are cleared automatically when called.
 * 
 *      TODO Expand to accept the other kind of rotary encoder later.
 * 
 * ****************************************************************************/

#include "RotaryEncoder.h"

// ***** Defines ***************************************************************


// ***** Function Prototypes ***************************************************


// ***** Global Variables ******************************************************

bool isClockwise, isCounterClockwise, isSwitchPress, isSwitchRelease;

/***************************************************************************//**
 * @brief Rotate Interrupt
 * 
 * To be called upon to process an event from the rotary encoder
 * 
 * @param AisHigh boolean for phase A of the rotary encoder
 *  
 * @param BisHigh boolean for phase B of the rotary encoder
 */
void RE_RotateInterrupt(bool AisHigh, bool BisHigh)
{
    // Phase A is enabled for rising and falling interrupts.
    // Look for a rising edge on channel A and check channel B.
    // Then look for a falling edge on channel A and check channel B.
    
    if(AisHigh) // rising edge on phase A
    {
        if(BisHigh)
            isCounterClockwise = true;
        else
            isClockwise = true;
    }
    else // falling edge on phase A
    {
        if(BisHigh)
            isClockwise = true;
        else
            isCounterClockwise = true;
    }
}

/***************************************************************************//**
 * @brief Switch Interrupt
 * 
 * To be called upon to process an event from the rotary encoder
 * 
 * @param isPressed boolean. true = switch pressed, false = release
 */
void RE_SwitchInterrupt(bool isPressed)
{
    // There are two flags, one for press and one for release. They will be
    // processed separately.
    isSwitchPress = isPressed;
    isSwitchRelease = !isPressed;
}

/***************************************************************************//**
 * @brief Get Clockwise Event
 * 
 * @param none
 * 
 * @return true if the there was one full clockwise click of the rotary encoder.
 */
bool RE_GetClockwise()
{
    bool temp = isClockwise;
    isClockwise = false;
    return temp;
}

/***************************************************************************//**
 * @brief Get Counter Clockwise Event
 * 
 * @param none
 * 
 * @return true if the there was one full counter-clockwise click of the rotary 
 * encoder
 */
bool RE_GetCounterClockwise()
{
    bool temp = isCounterClockwise;
    isCounterClockwise = false;
    return temp;
}

/***************************************************************************//**
 * @brief Get Switch Press Event
 * 
 * @param none
 * 
 * @return true = the switch was pressed
 */
bool RE_GetSwitchPress()
{
    bool temp = isSwitchPress;
    isSwitchPress = false;
    return temp;
}

/***************************************************************************//**
 * @brief Get Switch Release Event
 * 
 * @param none
 * 
 * @return true = the switch was released
 */
bool RE_GetSwitchRelease()
{
    bool temp = isSwitchRelease;
    isSwitchRelease = false;
    return temp;
}