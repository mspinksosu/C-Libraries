/***************************************************************************//**
 * @brief Rotary Encoder Library Header
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
 *      By far, the most common type of rotary encoder is one that has half
 * the number of detents as PPR. We refer to this is a "1/2 cycle per detent" 
 * rotary encoder. There are also rotary encoders that have the same number of
 * PPR as detents. These are "1 cycle per detent" rotary encoders. 
 * 
 *      The simplest way to decode these types of rotary encoders is to look at 
 * the rising and falling edge of one phase and then check the other phase. 
 * There are also "1/4 cycle per detent" rotary encoders. For this type, both 
 * phases have to monitored for rising and falling edges.
 * 
 *      To create a rotary encoder you need the debounce time in milliseconds
 * and the expected update rate in milliseconds (how often you call the tick
 * function). If you are debouncing using an RC filter, use 0 as the debounce 
 * time. For the tick rate, you should be updating the rotary encoder fairly
 * quickly. If you update it too slow, it may feel sluggish. Most datasheets 
 * I've looked at recommend a 5 ms debounce time. The maximum available 
 * debounce time is 255 ms.
 * 
 *      The flags for the get clockwise and get counter clockwise functions are
 * cleared automatically whenever they are called.
 * 
 * ****************************************************************************/

#include "RotaryEncoder.h"

// ***** Defines ***************************************************************


// ***** Function Prototypes ***************************************************


// ***** Global Variables ******************************************************

/* The previous value of the phases is shifted left and combined with the
current state. Using this, we can determine which direction we are going. The
value of the output will look like this: 0000baba. The index of the table 
corresponds to these values. A postive number indicates a clockwise transition
and a negative number indicates a counter clockwise transition. A zero means
there was no transition, or that it is invalid. */
static int8_t rotaryLookupTable[] = { 0, 1, -1, 0, -1, 0, 0, 1,
                                      1, 0, 0, -1, 0, -1, 1, 0 };

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
void RE_Init(RotaryEncoder *self, uint16_t debounceMs, uint16_t tickMs)
{
    RE_InitWithType(self, RE_HALF_CYCLE_PER_DETENT, debounceMs, tickMs);
}

/***************************************************************************//**
 * @brief Initialize a Rotary Encoder object
 * 
 * @param self  pointer to the Rotary Encoder that you are using
 * 
 * @param debounceMs  the debounce time in milliseconds
 * 
 * @param tickMs  how often you plan to call the RE Tick function
 */
void RE_InitWithType(RotaryEncoder *self, RotaryEncoderType type, uint16_t debounceMs, uint16_t tickMs)
{
    uint16_t debouncePeriod;
    
    /* The rotary encoder type mask will tell us how often we need to change
    the output depending on the type of rotary encoder. There will be a state
    change every 1/4 cycle. But the detent will only be at either every 1/4,
    1/2, or full cycle */
    switch(type)
    {
        case RE_HALF_CYCLE_PER_DETENT:
            self->typeMask = 0x01; // multiples of two
            break;
        case RE_FULL_CYCLE_PER_DETENT:
            self->typeMask = 0x03; // multiples of four
            break;
        default:
            self->typeMask = 0; // RE_QUARTER_CYCLE_PER_DETENT
            break;
    }

    if(tickMs != 0)
    {
        debouncePeriod = debounceMs / tickMs;
    }

    /* If you are using an RC filter to debounce, the debounce period should be
    zero. However, this type of debouncing algorithm requires a non-zero number 
    for its minimum. This will simply toggle the output every time the input 
    changes. For the maximum, I limit it to 8-bits. If your rotary encoder needs
    more than 255 ms to debounce, you've got some serious issues. */
    if(debouncePeriod == 0)
        self->debouncePeriod = 1;
    else if(debouncePeriod > 255)
        debouncePeriod = 255;
    
    self->debouncePeriod = (uint8_t)debouncePeriod;
}

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
void RE_UpdatePhases(RotaryEncoder *self, bool AisHigh, bool BisHigh)
{
    /* First update the inputs */
    if(AisHigh)
    {
        if(self->phaseAIntegrator < self->debouncePeriod)
        {
            self->phaseAIntegrator++;
        }
        else
        {
            /* Prevent from going over maximum value */
            self->phaseAIntegrator = self->debouncePeriod;
        }
    }
    else if(self->phaseAIntegrator > 0)
    {
        /* Phase A low */
        self->phaseAIntegrator--;
    }

    if(BisHigh)
    {
        if(self->phaseBIntegrator < self->debouncePeriod)
        {
            self->phaseBIntegrator++;
        }
        else
        {
            /* Prevent from going over maximum value */
            self->phaseBIntegrator = self->debouncePeriod;
        }
    }
    else if(self->phaseBIntegrator > 0)
    {
        /* Phase B low */
        self->phaseBIntegrator--;
    }

    /* Update the integrator outputs. Shifting the previous state left will 
    allow us to see the output from the previous state and the current state 
    together. Phase A will be encoded as bit 0, and and phase B as bit 1. 
    So the byte will look like this: 0000baba */
    self->state = ((self->state << 2) | self->state) & 0x0F;

    /* Update the outputs */
    if(self->phaseAIntegrator == 0)
    {
        self->state &= ~0x01; // clear phase A (bit 0)
    }
    else if(self->phaseAIntegrator >= self->debouncePeriod)
    {
        self->state |= 0x01; // set phase A (bit 0)
    }
    
    if(self->phaseBIntegrator == 0)
    {
        self->state &= ~0x02; // clear phase B (bit 1)
    }
    else if(self->phaseBIntegrator >= self->debouncePeriod)
    {
        self->state |= 0x02; // set phase B (bit 1)
    }
    
    /* Decode the output using the state table. A state transition will occur
    every quarter cycle. A postive number indicates a clockwise transition
    and a negative number indicates a counter clockwise transition. A zero 
    means there was no transition, or that it is invalid. */
    int8_t newOutput = rotaryLookupTable[self->state];
    bool goClockwise;

    /* Detect direction reversal. A -1 to +1 or +1 to -1 changes the sign and
    resets the count */
    if(self->output != newOutput && self->output + newOutput == 0)
    {
        self->output = newOutput;
    }
    else
    {
        self->output += newOutput;
    }
    
    if(self->output > 0)
        goClockwise = true;
    
    /* The typemask will cause an event to occur every quarter, half, or full
    cycle depending on the type of rotary encoder. It is based on modulo
    division. */
    if((self->output & self->typeMask) == 0)
    {
        if(goClockwise)
            self->flags.directionEvent.clockwise = 1;
        else
            self->flags.directionEvent.counterClockwise = 1;
    }
}

/***************************************************************************//**
 * @brief Get Clockwise Event
 * 
 * @param self  pointer to the Rotary Encoder that you are using
 * 
 * @return true if the there was one full clockwise click of the rotary encoder.
 */
bool RE_GetClockwise(RotaryEncoder *self)
{
    bool retVal = false;

    if(self->flags.directionEvent.clockwise)
        retVal = true;
    
    self->flags.directionEvent.clockwise = 0;
    return retVal;
}

/***************************************************************************//**
 * @brief Get Counter Clockwise Event
 * 
 * @param self  pointer to the Rotary Encoder that you are using
 * 
 * @return true if the there was one full counter-clockwise click of the rotary 
 * encoder
 */
bool RE_GetCounterClockwise(RotaryEncoder *self)
{
    bool retVal = false;

    if(self->flags.directionEvent.counterClockwise)
        retVal = true;
    
    self->flags.directionEvent.counterClockwise = 0;
    return retVal;
}