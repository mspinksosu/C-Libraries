/***************************************************************************//**
 * @brief Rotary Encoder Library (with switch)
 * 
 * @file RotaryEncoder.h
 * 
 * @author Matthew Spinks <https://github.com/mspinksosu>
 * 
 * @date 10/2/16    Original Creation
 * @date 2/21/22    Added Doxygen
 * @date 7/2/22     Redesigned to add different encoders and debouncing
 * 
 * @details
 *      A library that handles a basic quadrature rotary encoder. Quadrature 
 * rotary encoders have two traits that define them. The number of pulses per 
 * revolution (PPR) and the number of detents. Our goal is to generate a change 
 * in the output every time the knob moves one "click" or detent. Then we 
 * set a clockwise or counterclockwise event flag. To do all of this, you must
 * first know how many PPR and how many detents you have.
 * 
 * By far, the most common type of rotary encoder is one that has half the 
 * number of detents as PPR. We refer to this as a "1/2 cycle per detent" 
 * rotary encoder. This means the knob stops on a detent halfway through the 
 * quadrature output cycle. There are also rotary encoders that have the same 
 * number of PPR as detents ("full cycle per detent") and even 1/4 cycle per 
 * detent rotary encoders. This last one has a detent on every state change in 
 * the cycle. If you don't know what your rotary encoder is, it's probably the 
 * 1/2 cycle per detent type.
 * 
 * You also need the debounce time in milliseconds and the expected update rate 
 * in milliseconds (how often you plan to call the tick function). If you are 
 * debouncing using an RC filter, use 0 as the debounce time. For the tick 
 * rate, you should be updating the rotary encoder fairly quickly. If you 
 * update it too slow, it may feel sluggish. Most datasheets I've looked at 
 * recommend a 5 ms debounce time. So if you're debouncing using software, 
 * you'll probably want to call the update function at around that speed or 
 * faster. If you are using RC filters to debounce, you can probably get away 
 * with going slower. The maximum available debounce time for debouncing with 
 * software is 255 ms.
 * 
 * There are two callback functions for clockwise and counter clockwise events. 
 * The function you create must follow the prototype listed as RECallbackFunc. 
 * It must have a void pointer as an argument. When your function gets called, 
 * you will get a pointer to the RotaryEncoder object that initiated the call. 
 * This way, you can have multiple callbacks pointing to the same function 
 * if you desire. Then inside your callback function you would compare the 
 * pointer with your RotaryEncoder object to determine which one initiated the 
 * callback. You do not have to use the contextPointer if you do not want to. 
 * The reason it is a void pointer is so that you can have your callback 
 * function in some other file without needing to include the RotaryEncoder.h 
 * header file. After you've created your function, call 
 * SetClockwiseEventCallback or SetCounterClockwiseEventCallback and give 
 * it your function as an argument.
 * 
 * @section example_code Example Code
 *      RotaryEncoder myEncoder;
 *      RE_InitWithType(&myEncoder, RE_FULL_CYCLE_PER_DETENT, 5, 1);
 *      // call update phases once per tick
 *      RE_UpdatePhases(&myEncoder, GPIO_ReadPin(&pinA), GPIO_ReadPin(&pinB));
 *      if(RE_GetClockwise(&myEncoder))
 *      { .... do something .... }
 *      if(RE_GetCounterClockwise(&myEncoder))
 *      { .... do something .... }
 * 
 * @section license License
 * SPDX-FileCopyrightText: © 2016 Matthew Spinks
 * SPDX-License-Identifier: Zlib
 * 
 * This software is released under the Zlib license. You are free alter and
 * redistribute it, but you must not misrepresent the origin of the software.
 * This notice may not be removed. <http://www.zlib.net/zlib_license.html>
 * 
 * ****************************************************************************/

#ifndef ROTARYENCODER_H
#define ROTARYENCODER_H

// ***** Includes **************************************************************

#include <stdint.h>
#include <stdbool.h>

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

typedef enum RotaryEncoderTypeTag
{
    RE_HALF_CYCLE_PER_DETENT = 0,
    RE_FULL_CYCLE_PER_DETENT,
    RE_QUARTER_CYCLE_PER_DETENT,
} RotaryEncoderType;

/* callback function pointer. The context pointer will point to the Rotary
Encoder that initiated the callback. This is so that you can service multiple 
Rotary Encoder callbacks with the same function if you desire. */
typedef void (*RECallbackFunc)(void *rotaryContext);

typedef struct RotaryEncoderTag
{
    RECallbackFunc clockwiseEventCallback;
    RECallbackFunc counterClockwiseEventCallback;
    uint8_t typeMask;
    uint8_t debouncePeriod;
    uint8_t phaseAIntegrator;
    uint8_t phaseBIntegrator;
    uint8_t state;
    int8_t output;
    union {
        struct {
            unsigned clockwise        :1;
            unsigned counterClockwise :1;
            unsigned                  :0; // fill to nearest byte
        };
        uint8_t all;
    } flags;
} RotaryEncoder;

/** 
 * Description of struct members: You shouldn't need to mess with any of these
 * variables directly.
 * 
 * clockwiseEventCallback  A function that is called when a clockwise event
 *                         occurs.
 * 
 * counterClockwiseEventCallback  A function that is called when a counter
 *                                clockwise event occurs.
 * 
 * typeMask  A variable that alters the output to be every 1/4, 1/2, or full
 *           cycle.
 * 
 * debouncePeriod  How long to debounce the rotary encoder switches (in ticks)
 * 
 * phaseIntegrator  A counter that will smooth the input in order to debounce
 *                  it. When it hits either 0 or the debounce period, the 
 *                  output changes.
 * 
 * state  Stores the current state of the phase inputs, as well as the previous
 *        state.
 * 
 * output  An int that indicates which direction the rotary encoder is turning.
 *         If the rotary encoder is going clockwise, it will go from 1 to 127
 *         then rollover. If counter clockwise, it will go from -1 to -128
 * 
 * clockwise  A flag that indicates a clockwise event occurred
 * 
 * counterClockwise  A flag that indicates a counter clockwise event occurred
 * 
 */

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Function Prototypes *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/***************************************************************************//**
 * @brief Initialize a Rotary Encoder object (half cycle type)
 * 
 * Initializes a rotary encoder of the most common type
 * 
 * @param self  pointer to the Rotary Encoder that you are using
 * 
 * @param debounceMs  the debounce time in milliseconds
 * 
 * @param tickMs  how often you plan to call the RE Tick function
 */
void RE_Init(RotaryEncoder *self, uint16_t debounceMs, uint16_t tickMs);

/***************************************************************************//**
 * @brief Initialize a Rotary Encoder object
 * 
 * @param self  pointer to the Rotary Encoder that you are using
 * 
 * @param debounceMs  the debounce time in milliseconds
 * 
 * @param tickMs  how often you plan to call the RE Tick function
 */
void RE_InitWithType(RotaryEncoder *self, RotaryEncoderType type, uint16_t debounceMs, uint16_t tickMs);

/***************************************************************************//**
 * @brief Update the value of phases of the Rotary Encoder
 * 
 * This function must be called up periodically at the rate that you specified
 * when you initialized the Rotary Encoder object. If your rotary encoder
 * happens to be going the opposite direction as the events generated, just 
 * invert the inputs to this function when you call it.
 * 
 * @param AisHigh boolean for phase A of the rotary encoder
 *  
 * @param BisHigh boolean for phase B of the rotary encoder
 */
void RE_UpdatePhases(RotaryEncoder *self, bool AisHigh, bool BisHigh);

/***************************************************************************//**
 * @brief Get Clockwise Event
 * 
 * @param self  pointer to the Rotary Encoder that you are using
 * 
 * @return true if the there was one full clockwise click of the rotary encoder.
 */
bool RE_GetClockwise(RotaryEncoder *self);

/***************************************************************************//**
 * @brief Get Counter Clockwise Event
 * 
 * @param self  pointer to the Rotary Encoder that you are using
 * 
 * @return true if the there was one full counter-clockwise click of the rotary 
 * encoder
 */
bool RE_GetCounterClockwise(RotaryEncoder *self);

/***************************************************************************//**
 * @brief Set a function to be called when a clockwise event happens
 * 
 * The function prototype must have a void pointer as its argument. The pointer
 * will be set to the RotaryEncoder that called the function. This is so that 
 * multiple encoders can be serviced by the same function if desired. This 
 * function will not clear any event flags.
 * 
 * @param self  pointer to the Rotary Encoder that you are using
 * 
 * @param Function  format: void SomeFunction(void *rotaryContext)
 */
void RE_SetClockwiseEventCallback(RotaryEncoder *self, RECallbackFunc Function);

/***************************************************************************//**
 * @brief Set a function to be called when a counter clockwise event happens
 * 
 * The function prototype must have a void pointer as its argument. The pointer
 * will be set to the RotaryEncoder that called the function. This is so that 
 * multiple encoders can be serviced by the same function if desired. This 
 * function will not clear any event flags.
 * 
 * @param self  pointer to the Rotary Encoder that you are using
 * 
 * @param Function  format: void SomeFunction(void *rotaryContext)
 */
void RE_SetCounterClockwiseEventCallback(RotaryEncoder *self, RECallbackFunc Function);

#endif  /* ROTARYENCODER_H */

