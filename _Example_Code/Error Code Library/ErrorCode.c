/***************************************************************************//**
 * @brief Error Code Library
 *  
 * @file ErrorCode.c
 * 
 * @author Matthew Spinks
 * 
 * @date 11/20/22  Original creation
 *
 * @details
 *      Manages up to 32 error codes and flashes an LED for you. The LED can be 
 * updated by calling ErrorCode_GetBlinkOutput(). Relies on Pattern.h library
 * to define the on and off times for the LED itself. When the error code
 * library is initialized it is given a tick rate in milliseconds.
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

#include "ErrorCode.h"
#include "Pattern.h"

// ***** Defines ***************************************************************

#define ERROR_CODE_SHORT_PAUSE_SIZE     1
#define ERROR_CODE_LONG_PAUSE_SIZE      2

// ***** Global Variables ******************************************************

static PatternState errorFlash[2] = {{.output = 1, .timeInMs = 250},
                                     {.output = 0, .timeInMs = 300}};

/* By changing the size that we load into the pattern, it can either pause for
an extra 450 ms or 1250 ms. */
static PatternState errorPause[2] = {{.output = 0, .timeInMs = 450},
                                     {.output = 0, .timeInMs = 800}};

static Pattern errorCodePattern;
static uint32_t activeErrorMask, errorCodesToDisplayMask;
static uint8_t currentErrorCode, numFlashes, output;
static uint8_t numErrorCodesToDisplay = 32;
static bool errorCodeFinished = true, errorCodeRunning, stopSignal;
enum stopOptions {STOP_IMMEDIATELY, STOP_ON_NEXT_OFF_STATE, STOP_WHEN_FINISHED};
static enum stopOptions stopBehavior = STOP_IMMEDIATELY;

// ***** Static Function Prototypes ********************************************

static uint8_t GetNextErrorCode(void);

// *****************************************************************************

void ErrorCode_InitMs(uint16_t tickMs)
{
    activeErrorMask = 0;
    currentErrorCode = 0;
    errorCodesToDisplayMask = 0xFFFFFFFF >> (32 - numErrorCodesToDisplay);
    Pattern_InitMs(&errorCodePattern, tickMs);
}

// *****************************************************************************

void ErrorCode_Set(uint8_t code)
{
    if(code > 32 || code == 0)
        return;

    code--;
    activeErrorMask |= (1UL << code);
}

// *****************************************************************************

void ErrorCode_Clear(uint8_t code)
{
    if(code > 32 || code == 0)
        return;

    code--;
    activeErrorMask &= ~(1UL << code);
}

// *****************************************************************************

void ErrorCode_ClearAll(void)
{
    activeErrorMask = 0;
}

// *****************************************************************************

bool ErrorCode_IsSet(uint8_t code)
{
    bool retVal = false;

    if(code <= 32 && code > 0)
    {
        code--;
        if(activeErrorMask & (1UL << code))
            retVal = true;
    }
    
    return retVal;
}

// *****************************************************************************

void ErrorCode_Tick(void)
{
    static uint8_t errorCodesDisplayCount = 0;

    Pattern_Tick(&errorCodePattern);
    output = Pattern_GetOutput(&errorCodePattern);

    if(Pattern_IsFinished(&errorCodePattern))
    {
        Pattern_ClearFlag(&errorCodePattern);
        if(numFlashes > 0)
        {
            numFlashes--;
            if(numFlashes > 0)
            {
                /* We finished one flash. Load another */
                Pattern_Load(&errorCodePattern, errorFlash, 2);
                Pattern_StopAtomic(&errorCodePattern);
            }
            else
            {
                /* We finished the full sequence. If the the only error code 
                we need to flash is error code 1, insert a short pause at the 
                end of the sequence */
                if((activeErrorMask & errorCodesToDisplayMask) == 1)
                {
                    Pattern_Load(&errorCodePattern, errorPause, ERROR_CODE_SHORT_PAUSE_SIZE);
                }
                else
                {
                    Pattern_Load(&errorCodePattern, errorPause, ERROR_CODE_LONG_PAUSE_SIZE);
                }
                Pattern_StopAtomic(&errorCodePattern);
            }
        }
        else
        {
            errorCodeFinished = true;
            errorCodeRunning = false;
        }
    }

    if(activeErrorMask != 0 && errorCodeFinished && !stopSignal)
    {
        /* Start another LED flash sequence. First, check how many error codes 
        we are supposed to display. If we reach our limit, skip to the end and
        start over. We always take n number active errors codes of the lowest 
        value. So in a sense, error codes are prioritized by number with lower 
        numbers being higher priority. */
        if(errorCodesDisplayCount >= numErrorCodesToDisplay)
        {
            errorCodesDisplayCount = 0;
            currentErrorCode = 32;
        }

        currentErrorCode = GetNextErrorCode();
        errorCodesDisplayCount++;

        numFlashes = currentErrorCode;
        Pattern_Load(&errorCodePattern, errorFlash, 2);
        Pattern_StopAtomic(&errorCodePattern);
        errorCodeFinished = false;
        errorCodeRunning = true;
    }
}

// *****************************************************************************

uint8_t ErrorCode_GetCurrentCode(void)
{
    return currentErrorCode;
}

// *****************************************************************************

bool ErrorCode_GetBlinkOutput(void)
{
    return output;
}

// *****************************************************************************

void ErrorCode_Stop(void)
{
    switch(stopBehavior)
    {
        case STOP_IMMEDIATELY:
            Pattern_Stop(&errorCodePattern);
            errorCodeRunning = false;
            output = 0;
            break;
        case STOP_ON_NEXT_OFF_STATE:
            Pattern_StopAtomic(&errorCodePattern);
            break;
        case STOP_WHEN_FINISHED:
            stopSignal = true;
            break;
    }
}

// *****************************************************************************

void ErrorCode_Start(void)
{
    /* Restart the flash sequence from the beginning */
    numFlashes = 0;
    errorCodeFinished = true;
    errorCodeRunning = true;
    stopSignal = false;
}

// *****************************************************************************

bool ErrorCode_IsRunning(void)
{
    return errorCodeRunning;
}

// *****************************************************************************

uint32_t ErrorCode_GetActiveMask(void)
{
    return activeErrorMask;
}

// *****************************************************************************

void ErrorCode_SetDisplayTopNumOfCodes(uint8_t flashNumErrorCodes)
{
    if(flashNumErrorCodes == 0)
        flashNumErrorCodes = 1;
    else if(flashNumErrorCodes > 32)
        flashNumErrorCodes = 32;

    numErrorCodesToDisplay = flashNumErrorCodes;
    errorCodesToDisplayMask = 0xFFFFFFFF >> (32 - numErrorCodesToDisplay);
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Local Functions *****************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/***************************************************************************//**
 * @brief Get the Next Error Code
 * 
 * Loop through the error codes and find the next one that is set. If there 
 * are none it will return whatever currentErrorCode is.
 * 
 * @return uint8_t the next error code (0 to 31)
 */
static uint8_t GetNextErrorCode(void)
{
    /* Error codes always go from 1 to 32, but the index goes from 0 to 31 */
    uint8_t next = currentErrorCode & 0x1F;

    while(next != (currentErrorCode - 1))
    {
        if(activeErrorMask & (1UL << next))
            break;
        next = (next + 1) & 0x1F;
    }

    return next + 1;
}

/*
 End of File
 */