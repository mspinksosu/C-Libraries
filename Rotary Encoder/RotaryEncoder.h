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
 *      A library that handles the basic quadrature rotary encoder. Quadrature 
 * rotary encoders have two traits that define them. The number of pulses per 
 * revolution (PPR) and the number of detents. Our goal is to generate a change 
 * in the output every time the knob moves one "click" or detent. To do that, 
 * you must know how many PPR and how many detents.
 * 
 *      To create a rotary encoder you need to know the type. This is the
 * number of pulses per revolution (PPR) and the number of detents. If you
 * don't know, just assume that it is "1/2 cycle per detent" which is the most 
 * common type.
 * 
 *      You also need the debounce time in milliseconds and the expected update 
 * rate in milliseconds (how often you call the tick function). If you are 
 * debouncing using an RC filter, use 0 as the debounce time. For the tick rate, 
 * you should be updating the rotary encoder fairly quickly. If you update it 
 * too slow, it may feel sluggish. Most datasheets I've looked at recommend a 
 * 5 ms debounce time. If you're debouncing using software, you'll probably
 * want to update it around that fast. If you are using RC filters, you can
 * probably get away with going a little slower. The maximum available debounce 
 * time is 255 ms.
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

typedef struct RotaryEncoderTag
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
        } directionEvent;
    } flags;
} RotaryEncoder;


// ***** Function Prototypes ***************************************************

void RE_Init(RotaryEncoder *self, uint16_t debounceMs, uint16_t tickMs);

void RE_InitWithType(RotaryEncoder *self, RotaryEncoderType type, uint16_t debounceMs, uint16_t tickMs);

void RE_UpdatePhases(RotaryEncoder *self, bool AisHigh, bool BisHigh);

bool RE_GetClockwise(RotaryEncoder *self);

bool RE_GetCounterClockwise(RotaryEncoder *self);



#endif	/* ROTARYENCODER_H */

