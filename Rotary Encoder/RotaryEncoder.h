/***************************************************************************//**
 * @brief Rotary Encoder Library (with switch)
 * 
 * @author Matthew Spinks
 * 
 * @date 10/2/16    Original Creation
 * @date 2/21/22    Added Doxygen
 * @date 7/2/22     Redesigned to add different encoders and debouncing
 * 
 * @file RotaryEncoder.h
 * 
 * @details
 *      A library that handles a basic quadrature rotary encoder. Quadrature 
 * rotary encoders have two traits that define them. The number of pulses per 
 * revolution (PPR) and the number of detents. Our goal is to generate a change 
 * in the output every time the knob moves one "click" or detent. Then we 
 * set a clockwise or counterclockwise event flag. To do all of this, you must
 * first know how many PPR and how many detents you have.
 * 
 *      By far, the most common type of rotary encoder is one that has half
 * the number of detents as PPR. We refer to this as a "1/2 cycle per detent" 
 * rotary encoder. This means the knobs stops on a detent halfway through the 
 * quadrature output cycle. There are also rotary encoders that have the same 
 * number of PPR as detents ("full cycle per detent") and even 1/4 cycle per 
 * detent rotary encoders. This last one has a detent on every state change in 
 * the cycle. If you don't know what your rotary encoder is, it's probably the 
 * 1/2 cycle per detent type.
 * 
 *      You also need the debounce time in milliseconds and the expected update 
 * rate in milliseconds (how often you plan to call the tick function). If you 
 * are debouncing using an RC filter, use 0 as the debounce time. For the tick 
 * rate, you should be updating the rotary encoder fairly quickly. If you 
 * update it too slow, it may feel sluggish. Most datasheets I've looked at 
 * recommend a 5 ms debounce time. So if you're debouncing using software, 
 * you'll probably want to call the update function at around that speed or 
 * faster. If you are using RC filters to debounce, you can probably get away 
 * with going slower. The maximum available debounce time for debouncing with 
 * software is 255 ms.
 * 
 * ****************************************************************************/

#ifndef ROTARYENCODER_H
#define	ROTARYENCODER_H

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

/* A forward declaration which will allow the compiler to "know" what a
RotaryEncoder type is before I use it in the callback function declaration */
typedef struct RotaryEncoderTag RotaryEncoder;

/* callback function pointer. The context is so that you can know which encoder 
initiated the callback. This is so that you can service multiple encoder 
callbacks with the same function if you desire. */
typedef void (*RECallbackFunc)(RotaryEncoder *context);

struct RotaryEncoderTag
{
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

    RECallbackFunc clockwiseEventCallback;
    RECallbackFunc counterClockwiseEventCallback;
};

// ***** Function Prototypes ***************************************************

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
 * The function prototype must have a pointer to a RotaryEncoder as its 
 * argument. This is so that multiple encoders can be serviced by the same 
 * function if desired. This function will not clear any event flags.
 * 
 * @param self  pointer to the Rotary Encoder that you are using
 * 
 * @param Function  format: void SomeFunction(RotaryEncoder *context)
 */
void RE_SetClockwiseEventCallback(RotaryEncoder *self, RECallbackFunc Function);

/***************************************************************************//**
 * @brief Set a function to be called when a counter clockwise event happens
 * 
* The function prototype must have a pointer to a RotaryEncoder as its 
 * argument. This is so that multiple encoders can be serviced by the same 
 * function if desired. This function will not clear any event flags.
 * 
 * @param self  pointer to the Rotary Encoder that you are using
 * 
 * @param Function  format: void SomeFunction(RotaryEncoder *context)
 */
void RE_SetCounterClockwiseEventCallback(RotaryEncoder *self, RECallbackFunc Function);

#endif  /* ROTARYENCODER_H */

