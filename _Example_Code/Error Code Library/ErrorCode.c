/***************************************************************************//**
 * @brief Error Code Library
 * 
 * @author Matthew Spinks
 * 
 * @date 11/20/22  Original creation
 * 
 * @file ErrorCode.c
 * 
 * @details
 *      Manages up to 32 error codes and flashes an LED for you. The LED can be 
 * updated by calling ErrorCode_GetBlinkOutput(). Relies on Pattern.h library
 * to define the on and off times for the LED itself. When the error code
 * library is initialized it is given a tick rate in milliseconds.
 * 
 ******************************************************************************/

#include "ErrorCode.h"
#include "Pattern.h"

// ***** Defines ***************************************************************

// flash once
#define ERROR_CODE_FLASH_SIZE       2
#define ERROR_CODE_FLASH_PATT       {{.output = 1, .timeInMs = 250},\
                                     {.output = 0, .timeInMs = 300}}

// pause at the end of the pattern
#define ERROR_CODE_PAUSE_1_SIZE     1
#define ERROR_CODE_PAUSE_2_SIZE     2
#define ERROR_CODE_PAUSE_PATT       {{.output = 0, .timeInMs = 450},\
                                     {.output = 0, .timeInMs = 700}}

// ***** Global Variables ******************************************************

static Pattern errorCodePattern;
static PatternState errorFlash[ERROR_CODE_FLASH_SIZE] = ERROR_CODE_FLASH_PATT;
static PatternState errorPause[ERROR_CODE_PAUSE_2_SIZE] = ERROR_CODE_PAUSE_PATT;
static uint32_t activeErrorMask;
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
                Pattern_Load(&errorCodePattern, errorFlash, ERROR_CODE_FLASH_SIZE);
                Pattern_StopAtomic(&errorCodePattern);
            }
            else
            {
                /* We finished the full sequence. Time to pause briefly. If the 
                error code that we want to flash is more than 1, insert a longer 
                pause at the end of the sequence */
                if(currentErrorCode > 1)
                {
                    Pattern_Load(&errorCodePattern, errorPause, ERROR_CODE_PAUSE_2_SIZE);
                }
                else
                {
                    Pattern_Load(&errorCodePattern, errorPause, ERROR_CODE_PAUSE_1_SIZE);
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
        /* Start another LED flash sequence. */
        currentErrorCode = GetNextErrorCode();
        numFlashes = currentErrorCode;
        Pattern_Load(&errorCodePattern, errorFlash, ERROR_CODE_FLASH_SIZE);
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

void ErrorCode_SetDisplayTopNumOfCodes(uint8_t flashNumErrorCodes)
{
    if(flashNumErrorCodes == 0)
        flashNumErrorCodes = 1;
    else if(flashNumErrorCodes > 32)
        flashNumErrorCodes = 32;

    numErrorCodesToDisplay = flashNumErrorCodes;
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Local Functions *****************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

static uint8_t GetNextErrorCode(void)
{
    /* Check how many error codes we are supposed to display. If we reach our 
    limit, start back over at index 0. We always take the n number active 
    errors codes of lowest value. So in a sense, error codes are prioritized 
    by number with lower numbers being higher priority. */
    static uint8_t errorCodesDisplayCount = 0;

    if(errorCodesDisplayCount == numErrorCodesToDisplay)
    {
        errorCodesDisplayCount = 0;
        currentErrorCode = 31;
    }

    uint8_t next = (currentErrorCode + 1) & 0x1F;

    while(next != currentErrorCode)
    {
        if(activeErrorMask & (1UL << next))
            break;
        next = (next + 1) & 0x1F;
    }

    errorCodesDisplayCount++;

    /* The index is 0 to 31, but error codes go from 1 to 32 */
    return next + 1;
}

/*
 End of File
 */