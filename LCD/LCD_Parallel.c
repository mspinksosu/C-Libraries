/***************************************************************************//**
 * @brief Parallel Character Display Library Implementation
 * 
 * @author Matthew Spinks
 * 
 * @date 9/24/22  Original creation
 * 
 * @file LCD_Parallel.c
 * 
 * @details
 *      TODO
 * 
 ******************************************************************************/

#include "LCD_Parallel.h"
#include <string.h>

// ***** Defines ***************************************************************

#define LCD_PAR_ROW1_ADDR   0x00
#define LCD_PAR_ROW2_ADDR   0x40
#define LCD_PAR_ROW3_ADDR   LCD_PAR_ROW1_ADDR + 20
#define LCD_PAR_ROW4_ADDR   LCD_PAR_ROW2_ADDR + 20

/* Flags used to update either the left or right side of the screen */
#define LCD_PAR_LEFT        0x01
#define LCD_PAR_RIGHT       0x02

/* It takes a little more than 1.5 ms to execute the clear display and 
return home commands */
#define LCD_PAR_CLEAR_DISPLAY_US    2000

/* I will try reading the busy flag this many times */
#define LCD_PAR_BUSY_TRY_COUNT      3

/* If I can't use the read function I will delay for this time. The maximum 
delay should be 100 us. I think 50 us to 70 us works best. */
#define LCD_PAR_DELAY_US            50

/* If I can't read the busy flag and the user also didn't create a delay_us
function like they should have, I'm going to take a rough guess and count to
a number. Good luck. */
#define LCD_PAR_ROUGH_DELAY_COUNT   1000

// ***** Global Variables ******************************************************

/*  Create the function table */
LCDInterface LCD_ParallelFunctionTable = {
    .LCD_Init = (void (*)(void *, void *, uint16_t))LCD_Parallel_Init,
    .LCD_Tick = (void (*)(void *))LCD_Parallel_Tick,
    .LCD_IsBusy = (void (*)(void *))LCD_Parallel_IsBusy,
    .LCD_WriteCommand = (void (*)(void *, uint8_t))LCD_Parallel_WriteCommand,
    .LCD_WriteData = (void (*)(void *, uint8_t))LCD_Parallel_WriteData,
    .LCD_ReadData = (uint8_t (*)(void *))LCD_Parallel_ReadData,
    .LCD_ClearDisplay = (void (*)(void *))LCD_Parallel_ClearDisplay,
    .LCD_DisplayOn = (void (*)(void *))LCD_Parallel_DisplayOn,
    .LCD_DisplayOff = (void (*)(void *))LCD_Parallel_DisplayOff,
    .LCD_SetDisplayCursor = (void (*)(void *, bool))LCD_Parallel_SetDisplayCursor,
    .LCD_SetCursorBlink = (void (*)(void *, bool))LCD_Parallel_SetCursorBlink,
    .LCD_MoveCursor = (void (*)(void *, uint8_t, uint8_t))LCD_Parallel_MoveCursor,
    .LCD_MoveCursorForward = (void (*)(void *))LCD_Parallel_MoveCursorForward,
    .LCD_MoveCursorBackward = (void (*)(void *))LCD_Parallel_MoveCursorBackward,
    .LCD_GetCursorPosition = (void (*)(void *, uint8_t *, uint8_t *))LCD_Parallel_GetCursorPosition,
    .LCD_PutChar = (void (*)(void *, uint8_t))LCD_Parallel_PutChar,
    .LCD_PutString = (void (*)(void *, uint8_t *))LCD_Parallel_PutString,
    .LCD_WriteFullLine = (void (*)(void *, uint8_t, uint8_t *, uint8_t))LCD_Parallel_WriteFullLine,
    .LCD_ScrollDown = (void (*)(void *))LCD_Parallel_ScrollDown,
    .LCD_ScrollUp = (void (*)(void *))LCD_Parallel_ScrollUp,
};

/* A lookup table for use with the LCDParDisplayRefreshMask variable. 
Row number equals bit position: 4R 4L 2R 2L 3R 3L 1R 1L */
static uint8_t rowToBitPos[4] = {0, 1, 5, 3, 7};

/* Another lookup table for converting the row number to the address */
static uint8_t rowToAddr[4] = {0, LCD_PAR_ROW1_ADDR, LCD_PAR_ROW2_ADDR,
                                  LCD_PAR_ROW3_ADDR, LCD_PAR_ROW4_ADDR};

// ***** Static Function Prototypes ********************************************

static LCDParDisplayState GetNextState(LCD_Parallel *self);

/* A function to convert the cursor row and column to the correct address.
The memory addresses of a 4 line display are stored in two contiguous address 
ranges. Row 3 begins after row 1 and row 4 begins after row 2. */
static inline uint8_t CursorToAddress(uint8_t cursorRow, uint8_t cursorCol)
{
    return rowToAddr[cursorRow] + cursorCol - 1;
}

/* A function to convert the cursor row and column to the buffer index. I have
two buffers that mirror the addresses of the LCD. Rows 2 and 4 are the same as
rows 1 and 3 but with an added offset. */
static inline uint8_t CursorToIndex(uint8_t cursorRow, uint8_t cursorCol)
{
    return (rowToAddr[cursorRow] + cursorCol - 1) & ~LCD_PAR_ROW2_ADDR;
}

/* This just guarantees that we don't get stuck in a loop for "too" long. After
so many tries, we're just going to have to assume that the LCD is finished
doing its command. Most commands will execute in under 50 microseconds */
static inline void CheckIfBusyAndRetry(LCD_Parallel *self)
{
    for(uint8_t i = 0; i < LCD_PAR_BUSY_TRY_COUNT; i++)
    {
        if(!LCD_Parallel_IsBusy(self))
            break;
    }
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Non-Interface Functions *********************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void LCD_Parallel_Create(LCD_Parallel *self, LCD *base)
{
    self->super = base;

    /* Call base class constructor */
    LCD_Create(base, self, &LCD_ParallelFunctionTable);
}

void LCD_Parallel_CreateInitType(LCDInitType_Parallel *self, LCDInitType *base)
{
    self->super = base;

    /* Call base class constructor */
    LCD_CreateInitType(base, self);
}

void LCD_Parallel_Set4BitMode(LCDInitType_Parallel *self, bool use4BitMode)
{
    self->use4BitMode = use4BitMode;
}

void LCD_Parallel_SetSelectPinsFunc(LCD_Parallel *self, void (*Function)(bool rsPinHigh, bool rwPinHigh))
{
    self->SetSelectPins = Function;
}

void LCD_Parallel_SetEnablePinFunc(LCD_Parallel *self, void (*Function)(bool setPinHigh))
{
    self->SetEnablePin = Function;
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void LCD_Parallel_Init(LCD_Parallel *self, LCDInitType_Parallel *params, uint16_t tickUs)
{
    if(tickUs != 0)
        self->clearDisplayTimer.period = LCD_PAR_CLEAR_DISPLAY_US / tickUs;

    if(self->clearDisplayTimer.period == 0)
        self->clearDisplayTimer.period = 1;

    // set up variables
    self->displayOn = params->super->displayOn;
    self->cursorOn = params->super->cursorOn;
    self->blinkOn = params->super->blinkOn;
    self->use4BitMode = params->use4BitMode;
    self->clearDisplayTimer.flags.all = 0;
    self->currentIndex = 0;
    self->count = 0;
    self->cursorRow = 1;
    self->cursorCol = 1;
    self->currentRefreshMask = 0;
    self->currentState = LCD_PAR_STATE_ROW1_LEFT;

    // Send initial LCD commands
    self->initState = LCD_PAR_INIT_HOME;
    self->initialize = 1;
}

// *****************************************************************************

void LCD_Parallel_Tick(LCD_Parallel *self)
{
    static LCDParDisplayState nextState;

// ----- Wait Timer ------------------------------------------------------------

    if(self->clearDisplayTimer.flags.start && self->clearDisplayTimer.period != 0)
    {
        self->clearDisplayTimer.flags.start = 0;
        self->clearDisplayTimer.count = self->clearDisplayTimer.period;
        self->clearDisplayTimer.flags.active = 1;
    }
    
    if(self->clearDisplayTimer.flags.active)
    {
        /* Non-blocking wait function. It takes at least 1.5 ms for the clear
        display command to execute. Do nothing until the timer is done. */
        self->clearDisplayTimer.count--;

        if(self->clearDisplayTimer.count == 0)
        {
            self->clearDisplayTimer.flags.active = 0;
            self->clearDisplayTimer.flags.expired = 1;
        }
        else
        {
            return;
        }
    }

// ----- Initialize ------------------------------------------------------------

    if(self->initialize)
    {
        switch(self->initState)
        {
            case LCD_PAR_INIT_HOME:
                CheckIfBusyAndRetry(self);
                LCD_Parallel_WriteCommand(self, 0x30); // return home
                self->clearDisplayTimer.flags.start = 1;
                self->initState = LCD_PAR_INIT_ENTRY;
                break;
            case LCD_PAR_INIT_ENTRY:
                if(self->clearDisplayTimer.flags.expired)
                {
                    self->clearDisplayTimer.flags.expired = 0;
                    // increment address (move cursor right), shift = 0
                    LCD_Parallel_WriteCommand(self, 0x06);
                    self->initState = LCD_PAR_INIT_DISPLAY;
                }
                break;
            case LCD_PAR_INIT_DISPLAY:
                CheckIfBusyAndRetry(self);
                uint8_t command = 0x08;
                if(self->blinkOn) command |= 0x01;
                if(self->cursorOn) command |= 0x02;
                if(self->displayOn) command |= 0x04;
                LCD_Parallel_WriteCommand(self, command);
                self->initState = LCD_PAR_INIT_FUNCTION;
                break;
            case LCD_PAR_INIT_FUNCTION:
                CheckIfBusyAndRetry(self);
                // 8-bit, 2 lines, single height
                uint8_t command = 0x38;
                if(self->use4BitMode) command &= ~0x10;
                LCD_Parallel_WriteCommand(self, command);
                self->initState = LCD_PAR_INIT_HOME;
                self->initialize = 0; // finished
                break;
            default:
                self->initState = LCD_PAR_INIT_HOME;
                self->initialize = 0;
                break;
        }
    }

// ----- Update Display --------------------------------------------------------

    if(self->currentRefreshMask == 0)
    {
        /* We've finished updating the screen. There is nothing else to do 
        except place the cursor if needed before returning.
        The command is 0x80 + row addr + col - 1 */
        if(self->refreshCursor)
        {
            self->refreshCursor = 0;
            CheckIfBusyAndRetry(self);
            LCD_Parallel_WriteCommand(self, 0x80 + CursorToAddress(self->cursorRow, self->cursorCol));
        }
        return;
    }
    else if(!(self->currentRefreshMask & (1 << self->currentState)))
    {
        /* There is a refresh flag set, and it's not the one belonging to the 
        state we are currently in. Go find it. */
        self->currentState = GetNextState(self);
    }
    /* I've divided the rows into left and right sides. Most people are going 
    to have some sort of static image on the screen somewhere. If I don't see 
    any change in the buffer, I will skip it in order to reduce the number of 
    writes. The states go in order of address. First row 1, then 3, then 2, 
    then 4. If there was an LCD_Read or we skip a state, then we re-write the 
    cursor. This also reduces the time spent here by only doing an extra write 
    command when necessary. */
    if(self->currentIndex == 0 || self->updateAddressFlag)
    {
        /* Set the DDRAM address to the row + column.
        Row 1 address is 0x00. Row 2 address ix 0x40. Column address is 0-39
        The command is 0x80 + row addr + col - 1 */
        CheckIfBusyAndRetry(self);
        LCD_Parallel_WriteCommand(self, 0x80 + CursorToAddress(self->cursorRow, self->cursorCol));
        self->updateAddressFlag = 0;
    }
    
    CheckIfBusyAndRetry(self);
    if(self->cursorRow == 1 || self->cursorRow == 3)
    {
        LCD_Parallel_WriteData(self, self->lineBuffer1[self->currentIndex]);
    }
    else
    {
        LCD_Parallel_WriteData(self, self->lineBuffer2[self->currentIndex]);
    }
    self->currentIndex++;
    self->count++;

    /* If we are finished with this section, find the next state. If the next 
    state is not continuous, then set a flag to rewrite the cursor. */
    if(self->count > (self->super->numCols-1) / 2)
    {
        self->count = 0;
        self->currentRefreshMask &= ~(1 << self->currentState);
        nextState = GetNextState(self);
        if(nextState == LCD_PAR_REFRESH_ROW1_LEFT || 
            nextState == LCD_PAR_REFRESH_ROW2_LEFT)
        {
            self->currentIndex = 0;
        }
        if(nextState != self->currentState + 1)
            self->updateAddressFlag = 1;
        self->currentState = nextState;
    }
}

// *****************************************************************************

bool LCD_Parallel_IsBusy(LCD_Parallel *self)
{
    if(self->clearDisplayTimer.flags.active)
        return true;

    uint8_t data = 0;
    bool isBusy = false;
    
    /* The best way to check if the LCD is busy is to read the address counter
    and look at bit 7. If for some reason I can't do that, I'll use a delay. */
    if(self->super->mode == LCD_READ_WRITE && self->SetEnablePin 
        && self->SetSelectPins && self->super->ReceiveByte)
    {
        (self->SetEnablePin)(false); // RS and RW must be set while E is low
        (self->SetSelectPins)(false, true); // RS = 0: instruction, RW = 1: read
        (self->SetEnablePin)(true);
        data = self->super->ReceiveByte();
        (self->SetEnablePin)(false);

        if(data & 0x80)
            isBusy = true;
    }
    else if(self->super->DelayUs)
    {
        (self->super->DelayUs)(LCD_PAR_DELAY_US);
    }
    else
    {
        /* Last resort. I'm going to attempt to make a delay of some kind. 
        This is going to vary a lot based on your processor */
        for(uint16_t i = 0; i < LCD_PAR_ROUGH_DELAY_COUNT; i++) continue;
    }
    return isBusy;
}

// *****************************************************************************

void LCD_Parallel_WriteCommand(LCD_Parallel *self, uint8_t command)
{
    if(self->SetEnablePin && self->SetSelectPins && self->super->TransmitByte)
    {
        (self->SetEnablePin)(false); // RS and RW must be set while E is low
        (self->SetSelectPins)(false, false); // RS = 0: instruct, RW = 0: write
        (self->SetEnablePin)(true);
        
        if(self->super->DelayUs)
            (self->super->DelayUs)(1);

        /* In 4-bit mode, the upper nibble is sent first, followed immediately
        by the lower nibble. */
        if(self->use4BitMode)
        {
            (self->super->TransmitByte)(command & 0xF0);
            command <<= 4;

            if(self->super->DelayUs)
                (self->super->DelayUs)(1);

            (self->SetEnablePin)(false);

            if(self->super->DelayUs)
                (self->super->DelayUs)(1);

            (self->SetEnablePin)(true);
        }

        (self->super->TransmitByte)(command);
        
        if(self->super->DelayUs)
            (self->super->DelayUs)(1);
        
        (self->SetEnablePin)(false);
    }
}

// *****************************************************************************

void LCD_Parallel_WriteData(LCD_Parallel *self, uint8_t data)
{
    if(self->SetEnablePin && self->SetSelectPins && self->super->TransmitByte)
    {
        (self->SetEnablePin)(false); // RS and RW must be set while E is low
        (self->SetSelectPins)(true, false); // RS = 1: data, RW = 0: write
        (self->SetEnablePin)(true);
        
        if(self->super->DelayUs)
            (self->super->DelayUs)(1);

        /* In 4-bit mode, the upper nibble is sent first, followed immediately
        by the lower nibble. */
        if(self->use4BitMode)
        {
            (self->super->TransmitByte)(data & 0xF0);
            data <<= 4;

            if(self->super->DelayUs)
                (self->super->DelayUs)(1);

            (self->SetEnablePin)(false);

            if(self->super->DelayUs)
                (self->super->DelayUs)(1);

            (self->SetEnablePin)(true);
        }

        (self->super->TransmitByte)(data);
        
        if(self->super->DelayUs)
            (self->super->DelayUs)(1);
        
        (self->SetEnablePin)(false);
    }
}

// *****************************************************************************

uint8_t LCD_Parallel_ReadData(LCD_Parallel *self)
{
    uint8_t data = 0;

    if(self->super->mode == LCD_READ_WRITE && self->SetEnablePin 
        && self->SetSelectPins && self->super->ReceiveByte)
    {
        (self->SetEnablePin)(false); // RS and RW must be set while E is low
        (self->SetSelectPins)(true, true); // RS = 1: data, RW = 1: read

        /* The minimum acquisition time is 520 ns after rising edge of E pin. 
        I imagine this code will take longer, but just in case, I will use a 
        small delay */
        (self->SetEnablePin)(true);
        
        if(self->super->DelayUs)
            (self->super->DelayUs)(1);

        data = self->super->ReceiveByte();
        (self->SetEnablePin)(false);
        
        /* In 4-bit mode, the upper nibble is read first, followed immediately
        by the lower nibble. */
        if(self->use4BitMode)
        {
            if(self->super->DelayUs)
                (self->super->DelayUs)(1);

            (self->SetEnablePin)(true);

            uint8_t lowerNibble = self->super->ReceiveByte();

            (self->SetEnablePin)(false);

            data |= (lowerNibble >> 4);
        }
        self->updateAddressFlag = 1;
    }
    return data;
}

// *****************************************************************************

void LCD_Parallel_ClearDisplay(LCD_Parallel *self)
{
    for(uint8_t i = 0; i < sizeof(self->lineBuffer1); i++)
    {
        self->lineBuffer1[i] = 0;
        self->lineBuffer2[i] = 0;
    }
    self->cursorRow = 0;
    self->cursorCol = 0;
    self->refreshCursor = 1;
    self->currentRefreshMask = 0xFF; // TODO figure out if this is necessary
    LCD_Parallel_WriteCommand(self, 0x01);
    self->clearDisplayTimer.flags.expired = 0;
    self->clearDisplayTimer.flags.start = 1;
}

// *****************************************************************************

void LCD_Parallel_DisplayOn(LCD_Parallel *self)
{
    uint8_t command = 0x08;
    if(self->blinkOn) command |= 0x01;
    if(self->cursorOn) command |= 0x02;
    command |= 0x04; // display on
    self->displayOn = 1;
    LCD_Parallel_WriteCommand(self, command);
}

// *****************************************************************************

void LCD_Parallel_DisplayOff(LCD_Parallel *self)
{
    uint8_t command = 0x08;
    if(self->blinkOn) command |= 0x01;
    if(self->cursorOn) command |= 0x02;
    self->displayOn = 0;
    LCD_Parallel_WriteCommand(self, command);
}

// *****************************************************************************

void LCD_Parallel_SetDisplayCursor(LCD_Parallel *self, bool cursorOn)
{
    uint8_t command = 0x08;
    if(self->blinkOn) command |= 0x01;
    if(cursorOn) 
    {
        command |= 0x02;
        self->cursorOn = 1;
    }
    else
    {
        self->cursorOn = 0;
    }
    if(self->displayOn) command |= 0x04;
    LCD_Parallel_WriteCommand(self, command);
}

// *****************************************************************************

void LCD_Parallel_SetCursorBlink(LCD_Parallel *self, bool blinkEnabled)
{
    uint8_t command = 0x08;
    if(blinkEnabled) 
    {
        command |= 0x01;
        self->blinkOn = 1;
    }
    else
    {
        self->blinkOn = 0;
    }
    if(self->cursorOn) command |= 0x02;
    if(self->displayOn) command |= 0x04;
    LCD_Parallel_WriteCommand(self, command);
}

// *****************************************************************************

void LCD_Parallel_MoveCursor(LCD_Parallel *self, uint8_t row, uint8_t col)
{
    if(row == 0) 
        row = 1;
    else if(row > self->super->numRows)
        row = self->super->numRows;

    if(col == 0)
        col = 1; 
    else if(col > self->super->numCols)
        col = self->super->numCols;

    self->cursorRow = row;
    self->cursorCol = col;
    self->refreshCursor = 1;
}

// *****************************************************************************

void LCD_Parallel_MoveCursorForward(LCD_Parallel *self)
{
    self->cursorCol + 1;
    if(self->cursorCol > self->super->numCols)
    {
        self->cursorCol = 1;
        self->cursorRow++;
    }

    if(self->cursorRow > self->super->numRows)
        self->cursorRow = 1;

    self->refreshCursor = 1;
}

// *****************************************************************************

void LCD_Parallel_MoveCursorBackward(LCD_Parallel *self)
{
    self->cursorCol - 1;
    if(self->cursorCol < 1)
    {
        self->cursorCol = self->super->numCols;
        self->cursorRow--;
    }

    if(self->cursorRow < 1)
        self->cursorRow = self->super->numRows;
    
    self->refreshCursor = 1;
}

// *****************************************************************************

void LCD_Parallel_GetCursorPosition(LCD_Parallel *self, uint8_t *retRow, uint8_t *retCol)
{
    *retRow = self->cursorRow;
    *retCol = self->cursorCol;
}

// *****************************************************************************

void LCD_Parallel_PutChar(LCD_Parallel *self, uint8_t character)
{
    uint8_t index = CursorToIndex(self->cursorRow, self->cursorCol);
    uint8_t bitmask = LCD_PAR_LEFT;
    uint8_t *lineBuffer;
    uint8_t bitPos = rowToBitPos[self->cursorRow];

    /* If we are further than halfway across, update the right side. Else, just
    the left side. For the bitmask, left side = 0x01 and right side = 0x02 */
    if(self->cursorCol > (self->super->numCols+1) / 2)
        bitmask = LCD_PAR_RIGHT;

    if(self->cursorRow == 1 || self->cursorRow == 3)
    {
        /* Single line displays split the line in half. If we are on the 
        right side, go to the line 2 buffer, left side. */
        if(self->super->numRows == 1 && bitmask == LCD_PAR_RIGHT)
        {
            lineBuffer = self->lineBuffer2;
            index = 0;
            bitPos = rowToBitPos[2];
            bitmask = LCD_PAR_LEFT;
        }
        else
            lineBuffer = self->lineBuffer1;
    }
    else
    {
        lineBuffer = self->lineBuffer2;
    }
    lineBuffer[index] = character;
    self->currentRefreshMask |= (bitmask << bitPos);
    LCD_Parallel_MoveCursorForward(self); // TODO decide where to put the cursor
}

// *****************************************************************************

void LCD_Parallel_PutString(LCD_Parallel *self, uint8_t *ptrToString)
{
    if(*ptrToString == '\0')
        return;
        
    uint8_t index = CursorToIndex(self->cursorRow, self->cursorCol);
    uint8_t bitmask = 0;
    uint8_t *lineBuffer = self->lineBuffer1;
    uint8_t bitPos = rowToBitPos[self->cursorRow];

    if(self->cursorRow == 2 || self->cursorRow == 4)
        lineBuffer = self->lineBuffer2;

    /* flag left side for update */
    if(self->cursorCol <= (self->super->numCols+1) / 2)
        self->currentRefreshMask |= (LCD_PAR_LEFT << bitPos);

    /* Copy the data over. Stop when we hit the end of the row. Cursor will 
    stop at the next position. */
    do {
        if(self->cursorCol == ((self->super->numCols+1) / 2) + 1)
        {
            /* Single line displays split the line in half. If we cross the 
            halfway boundary, go to the line 2 buffer, left side. */
            if(self->super->numRows == 1)
            {
                lineBuffer = self->lineBuffer2;
                index = 0;
                bitPos = rowToBitPos[2];
                self->currentRefreshMask |= (LCD_PAR_LEFT << bitPos);
            }
            else
            {
                self->currentRefreshMask |= (LCD_PAR_RIGHT << bitPos);
            }
        }
        lineBuffer[index] = *ptrToString;
        index++;
        self->cursorCol++;
    } while(*ptrToString != '\0' || self->cursorCol < self->super->numCols);
    LCD_Parallel_MoveCursorForward(self); // TODO decide where to put the cursor
}

// *****************************************************************************

void LCD_Parallel_WriteFullLine(LCD_Parallel *self, uint8_t lineNum, uint8_t *array, uint8_t size)
{
    if(lineNum == 0 || lineNum > self->super->numRows)
        return;

    if(size > 20)
        size = 20;

    switch(lineNum)
    {
        case 1:
            uint8_t leftEndCol = (self->super->numCols+1) / 2;
            if(self->super->numRows == 1 && size > leftEndCol)
            {
                /* Split the single row display in half */
                memcpy(self->lineBuffer1, array, leftEndCol);
                memcpy(self->lineBuffer2, &array[leftEndCol - 1], size - leftEndCol);
            }
            else
            {
                memcpy(self->lineBuffer1, array, size);
            }
            break;
        case 3:
            memcpy(self->lineBuffer1, array, size);
            break;
        case 2:
        case 4:
            memcpy(self->lineBuffer2, array, size);
            break;
    }
}

void LCD_Parallel_ScrollDown(LCD_Parallel *self)
{
    // TODO scroll
}

// *****************************************************************************

void LCD_Parallel_ScrollUp(LCD_Parallel *self)
{
    // TODO scroll
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Local Functions *****************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

static LCDParDisplayState GetNextState(LCD_Parallel *self)
{
    /* The states go from 0 to 7. This is just a quick method of doing modulo
    division */
    LCDParDisplayState nextState = (self->currentState + 1) & 0x07;

    while(nextState != self->currentState)
    {
        if(self->currentRefreshMask & (1 << nextState))
            break;
        nextState = (nextState + 1) & 0x07;
    }
    return nextState;
}

/*
 End of File
 */