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
 *      Manages up to 64 error codes and flashes an LED for you. The LED can be 
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
#define MAX_BITS                        64
#define MAX_VALUE                       0xFFFFFFFFFFFFFFFF

// ***** Global Variables ******************************************************

static PatternState errorFlash[2] = {{.output = 1, .timeInMs = 250},
                                     {.output = 0, .timeInMs = 300}};

/* By changing the size that we load into the pattern, it can either pause for
an extra 450 ms or 1250 ms. */
static PatternState errorPause[2] = {{.output = 0, .timeInMs = 450},
                                     {.output = 0, .timeInMs = 800}};

static Pattern errorCodePattern;
static uint8_t sortedErrorCodes[MAX_BITS][2];
static uint64_t activeErrorMask, errorCodesToDisplayMask;
static uint8_t currentErrorCodeIndex, numFlashes, output;
static uint8_t numErrorCodesToDisplay = MAX_BITS, errorCodeDisplayCount;
static bool errorCodeFinished = true, errorCodeRunning, stopSignal;
enum stopOptions {STOP_IMMEDIATELY, STOP_ON_NEXT_OFF_STATE, STOP_WHEN_FINISHED};
static enum stopOptions stopBehavior = STOP_IMMEDIATELY;

// ***** Static Function Prototypes ********************************************

static void SortErrorCodeArrayByPriority(void);
static uint8_t GetNextErrorCodeIndex(void);

// *****************************************************************************

void ErrorCode_InitMs(uint16_t tickMs)
{
    activeErrorMask = 0;
    currentErrorCodeIndex = MAX_BITS - 1;
    errorCodesToDisplayMask = MAX_VALUE >> (MAX_BITS - numErrorCodesToDisplay);
    Pattern_InitMs(&errorCodePattern, tickMs);

    /* Error codes go from 1 to 64 */
    for(uint8_t i = 0; i < MAX_BITS; i++)
    {
        sortedErrorCodes[i][0] = i + 1;
        sortedErrorCodes[i][1] = DEFAULT_EC_PRIORITY_LEVEL;
    }
    SortErrorCodeArrayByPriority();
}

// *****************************************************************************

void ErrorCode_Set(uint8_t code)
{
    if(code > MAX_BITS || code == 0)
        return;

    code--;
    activeErrorMask |= (1UL << code);
}

// *****************************************************************************

void ErrorCode_Clear(uint8_t code)
{
    if(code > MAX_BITS || code == 0)
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

    if(code <= MAX_BITS && code > 0)
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
        start over. */
        if(errorCodeDisplayCount >= numErrorCodesToDisplay)
        {
            errorCodeDisplayCount = 0;
            currentErrorCodeIndex = MAX_BITS - 1;
        }

        uint8_t nextErrorCodeIndex = GetNextErrorCodeIndex();
        if(nextErrorCodeIndex != currentErrorCodeIndex)
            errorCodeDisplayCount++;
        currentErrorCodeIndex = nextErrorCodeIndex;
        numFlashes = sortedErrorCodes[currentErrorCodeIndex][0];
        Pattern_Load(&errorCodePattern, errorFlash, 2);
        Pattern_StopAtomic(&errorCodePattern);
        errorCodeFinished = false;
        errorCodeRunning = true;
    }
}

// *****************************************************************************

uint8_t ErrorCode_GetCurrentCode(void)
{
    return sortedErrorCodes[currentErrorCodeIndex][0];
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

uint64_t ErrorCode_GetActiveMask(void)
{
    return activeErrorMask;
}

// *****************************************************************************

uint32_t ErrorCode_GetActiveMaskRange(uint8_t errorCodeEnd, uint8_t errorCodeStart)
{
    if(errorCodeEnd > 64 || errorCodeStart > 64 || 
        errorCodeEnd == 0 || errorCodeStart == 0)
        return 0;

    if(errorCodeStart > errorCodeEnd)
    {
        uint8_t tmp = errorCodeEnd;
        errorCodeEnd = errorCodeStart;
        errorCodeStart = tmp;
    }

    errorCodeStart--;
    errorCodeEnd--;
    uint32_t result = 0;
    uint8_t bit = errorCodeStart, d = 0;

    while(bit <= errorCodeEnd && d < 31)
    {
        if(activeErrorMask & (1ULL << bit))
        {
            result |= (1UL << d);
        }
        d++;
        bit++;
    }
    return result;
}

// *****************************************************************************

void ErrorCode_SetDisplayTopNumOfCodes(uint8_t flashNumErrorCodes)
{
    if(flashNumErrorCodes == 0)
        flashNumErrorCodes = 1;
    else if(flashNumErrorCodes > MAX_BITS)
        flashNumErrorCodes = MAX_BITS;

    numErrorCodesToDisplay = flashNumErrorCodes;
    errorCodesToDisplayMask = MAX_VALUE >> (MAX_BITS - numErrorCodesToDisplay);
}

// *****************************************************************************

void ErrorCode_SetPriorityLevel(uint8_t code, uint8_t priority)
{
    if(code <= MAX_BITS && code > 0)
    {
        /* Locate the desired error code. Returns -1 if not found. */
        int8_t i = sizeof(sortedErrorCodes) / sizeof(sortedErrorCodes[0]) - 1;
        for(; i >= 0; i--)
        {
            if(sortedErrorCodes[i][0] == code)
                break;
        }

        if(i > 0)
        {
            sortedErrorCodes[i][1] = priority;
            SortErrorCodeArrayByPriority();
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Local Functions *****************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/***************************************************************************//**
 * @brief Sort the array of error codes
 * 
 * The array of error codes contains a list of error codes and their priority
 * number. sortedErrorCodes[i][0] is the error code number from 1 to n. 
 * sortedErrorCodes[i][1] is the priority value. A lower priority number means 
 * higher priority.
 */
static void SortErrorCodeArrayByPriority(void)
{
    int8_t i, j, code, priority;
    for(i = 1; i < sizeof(sortedErrorCodes) / sizeof(sortedErrorCodes[0]); i++)
    {
        code = sortedErrorCodes[i][0];
        priority = sortedErrorCodes[i][1];
        j = i - 1;

        while(j >= 0 && sortedErrorCodes[j][1] > priority)
        {
            sortedErrorCodes[j + 1][0] = sortedErrorCodes[j][0];
            sortedErrorCodes[j + 1][1] = sortedErrorCodes[j][1];
            j--;
        }
        sortedErrorCodes[j + 1][0] = code;
        sortedErrorCodes[j + 1][1] = priority;
    }
}

/***************************************************************************//**
 * @brief Get the Next Error Code Index
 * 
 * Loop through the error codes and find the next one that is set. If there 
 * are none it will return whatever the current error code index is.
 * 
 * @return uint8_t the next error code index (0 to 31)
 */
static uint8_t GetNextErrorCodeIndex(void)
{
    uint8_t bit = 0;
    uint8_t next = (currentErrorCodeIndex + 1) & (MAX_BITS-1);

    while(next != currentErrorCodeIndex)
    {
        /* Error codes go from 1 to 64, but the bits go from 0 to 63 */
        bit = sortedErrorCodes[next][0] - 1;
        if(activeErrorMask & (1ULL << bit))
            break;
        next = (next + 1) & (MAX_BITS-1);
    }

    return next;
}

/*
 End of File
 */