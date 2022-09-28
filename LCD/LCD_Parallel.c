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

#define LCD_PAR_LEFT        0x01
#define LCD_PAR_RIGHT       0x02
#define LCD_PAR_DELAY_US    70

// ***** Global Variables ******************************************************

/*  The sub class must implement the functions provided in the interface. In 
    this case we are declaring an interface struct and initializing its members 
    (which are function pointers) the our local functions. Typecasting is 
    necessary. When a new sub class object is created, we will set its interface
    member equal to this table. */
LCDInterface LCD_ParallelFunctionTable = {

};

/* Lookup table for use with the displayRefreshMask variable. Row number equals
bit position: 4R 4L 2R 2L 3R 3L 1R 1L */
static uint8_t rowToBitPos[4] = {0, 1, 5, 3, 7};

// ***** Static Function Prototypes ********************************************

/* Put static function prototypes here */
static displayState GetNextState(LCD_Parallel *self);

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

void LCD_Parallel_Set4BitMode(LCD_Parallel *self, bool use4BitMode)
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

void LCD_Parallel_SetDataPinsFunc(LCD_Parallel *self, void (*Function)(uint8_t data, bool nibble))
{
    self->SetDataPins = Function;
}

void LCD_Parallel_ReadDataPinsFunc(LCD_Parallel *self, uint8_t (*Function)(void))
{
    self->ReadDataPins = Function;
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void LCD_Parallel_Init(LCD_Parallel *self, LCDInitType *params, uint8_t tickMs)
{
    // Set timer period

    // set up variables

    // call base class constructor
}

// *****************************************************************************

void LCD_Parallel_Tick(LCD_Parallel *self)
{
    static displayState nextState;

    if(self->clearDisplayTimer.flags.start && self->clearDisplayTimer.period != 0)
    {
        self->clearDisplayTimer.flags.start = 0;
        self->clearDisplayTimer.count = self->clearDisplayTimer.period;
        self->clearDisplayTimer.flags.active = 1;
    }
    
    if(self->clearDisplayTimer.flags.active)
    {
        /* Non-blocking wait function. It takes at least 1 ms for the clear
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
    /* TODO Replace LCD busy while loop with some sort of non-blocking function
    or a retry */
    if(self->currentRefreshMask == 0)
    {
        /* We've finished updating the screen. There is nothing else to do 
        except place the cursor if needed before returning. 
        The command is 0x80 + row addr + col */
        if(self->refreshCursor)
        {
            self->refreshCursor = false;
            while(LCD_Parallel_IsBusy(self)){}
            switch(self->cursorRow)
            {
                case 1:
                    LCD_Parallel_WriteCommand(self, 0x80 + self->cursorCol - 1);
                    break;
                case 2:
                    LCD_Parallel_WriteCommand(self, 0xC0 + self->cursorCol - 1);
                    break;
                case 3:
                    LCD_Parallel_WriteCommand(self, 0x80 + self->cursorCol + 19);
                    break;
                case 4:
                    LCD_Parallel_WriteCommand(self, 0xC0 + self->cursorCol + 19);
                    break;
            }
        }
        return;
    }

    /* If there is a refresh flag set, and it's not the one belonging to the 
    state we are currently in, go find it. */
    if(!(self->currentRefreshMask & (1 << self->currentState)))
    {
        self->currentState = GetNextState(self);
    }
    /* Dvide the screen into quadrants. Most people are going to have some
    sort of static image on the screen somewhere. If I don't see any change
    in the buffer, I will skip it in order to reduce the number of writes. The
    states go in order of address. First row 1, then 3, then 2, then 4. If 
    there was an LCD_Read or we skip a state, rewrite the cursor address. This
    also reduces the time spent here by only doing an extra write command when
    necessary. */
    switch(self->currentState)
    {
        case LCD_PAR_REFRESH_ROW1_LEFT:
        case LCD_PAR_REFRESH_ROW1_RIGHT:
        case LCD_PAR_REFRESH_ROW3_LEFT:
        case LCD_PAR_REFRESH_ROW3_RIGHT:
            if(self->currentIndex == 0 || self->updateAddressFlag)
            {
                /* Set the DDRAM address to the row + column.
                Row 1 address is 0x00. Column address is 0-39
                Cmd = 0x80 + row addr + col = 0x80 + 0x00 + col */
                while(LCD_Parallel_IsBusy(self)){}
                LCD_Parallel_WriteCommand(self, 0x80 + self->currentIndex);
                self->updateAddressFlag = false;
            }
            while(LCD_Parallel_IsBusy(self)){}
            LCD_Parallel_WriteData(self, self->lineBuffer1[self->currentIndex]);
            self->currentIndex++;
            self->count++;
            break;
        case LCD_PAR_REFRESH_ROW2_LEFT:
        case LCD_PAR_REFRESH_ROW2_RIGHT:
        case LCD_PAR_REFRESH_ROW4_LEFT:
        case LCD_PAR_REFRESH_ROW4_RIGHT:
            if(self->currentIndex == 0 || self->updateAddressFlag)
            {
                /* Set the DDRAM address to the row + column.
                Row 2 address is 0x40. Column address is 0 - 39
                Cmd = 0x80 + row addr + col = 0x80 + 0x40 + col */
                while(LCD_Parallel_IsBusy(self)){}
                LCD_Parallel_WriteCommand(self, 0xC0 + self->currentIndex);
                self->updateAddressFlag = false;
            }
            while(LCD_Parallel_IsBusy(self)){}
            LCD_Parallel_WriteData(self, self->lineBuffer2[self->currentIndex]);
            self->currentIndex++;
            self->count++;
            break;
    }

    /* If we're done writing, find the next state. If the next state is not
    continuous, then set a flag to rewrite the cursor. */
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
            self->updateAddressFlag = true;
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
    if(self->super->mode == LCD_READ_WRITE && self->SetEnablePin && 
        self->SetSelectPins && self->ReadDataPins)
    {
        (self->SetEnablePin)(false); // RS and RW must be set while E is low
        (self->SetSelectPins)(false, true); // RS = 0: instruction, RW = 1: read
        (self->SetEnablePin)(true);
        data = self->ReadDataPins();
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
        /* I'm going to attempt to make a delay of some kind. This is going to 
        vary a lot based on your processor */
        for(uint16_t i = 0; i < 1000; i++) continue;
    }
    return isBusy;
}

// *****************************************************************************

void LCD_Parallel_WriteCommand(LCD_Parallel *self, uint8_t command)
{
    if(self->SetEnablePin && self->SetSelectPins && self->SetDataPins)
    {
        (self->SetEnablePin)(false); // RS and RW must be set while E is low
        (self->SetSelectPins)(false, false); // RS = 0: instruction, RW = 0: write
        (self->SetEnablePin)(true);
        
        if(self->super->DelayUs)
            (self->super->DelayUs)(1);

        // TODO add 4-bit mode
        (self->SetDataPins)(command, false);
        
        if(self->super->DelayUs)
            (self->super->DelayUs)(1);
        
        (self->SetEnablePin)(false);
    }
}

// *****************************************************************************

void LCD_Parallel_WriteData(LCD_Parallel *self, uint8_t data)
{
    if(self->SetEnablePin && self->SetSelectPins && self->SetDataPins)
    {
        (self->SetEnablePin)(false); // RS and RW must be set while E is low
        (self->SetSelectPins)(true, false); // RS = 1: data, RW = 0: write
        (self->SetEnablePin)(true);
        
        if(self->super->DelayUs)
            (self->super->DelayUs)(1);

        // TODO add 4-bit mode
        (self->SetDataPins)(data, false);
        
        if(self->super->DelayUs)
            (self->super->DelayUs)(1);
        
        (self->SetEnablePin)(false);
    }
}

// *****************************************************************************

uint8_t LCD_Parallel_ReadData(LCD_Parallel *self)
{
    uint8_t data = 0;

    if(self->super->mode == LCD_READ_WRITE && self->SetEnablePin && 
        self->SetSelectPins && self->ReadDataPins)
    {
        (self->SetEnablePin)(false); // RS and RW must be set while E is low
        (self->SetSelectPins)(true, true); // RS = 1: data, RW = 1: read

        /* The minimum acquisition time is 520 ns after rising edge of E pin. 
        I imagine this code will take longer, but just in case, I will use a 
        small delay */
        (self->SetEnablePin)(true);
        
        if(self->super->DelayUs)
            (self->super->DelayUs)(1);

        data = self->ReadDataPins();
        (self->SetEnablePin)(false);
        self->updateAddressFlag = true;
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
    self->refreshCursor = true;
    self->currentRefreshMask = 0xFF; // TODO figure out if this is necessary
    LCD_Parallel_WriteCommand(self, 0x01);
    self->clearDisplayTimer.flags.start = 1;
}

// *****************************************************************************

void LCD_Parallel_DisplayOn(LCD_Parallel *self)
{
    uint8_t command = 0x08;
    if(self->blinkOn) command |= 0x01;
    if(self->cursorOn) command |= 0x02;
    command |= 0x0C; // display on
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

    self->refreshCursor = true;
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

    self->refreshCursor = true;
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
    
    self->refreshCursor = true;
}

// *****************************************************************************

void LCD_Parallel_PutChar(LCD_Parallel *self, uint8_t character)
{
    uint8_t index = 0;
    uint8_t bitmask = LCD_PAR_LEFT;
    uint8_t *lineBuffer;
    uint8_t bitPos = rowToBitPos[self->cursorRow];

    /* The memory addresses of a 4 line display are stored in two contiguous 
    address ranges. Row 3 begins after row 1 and row 4 begins after row 2. */
    if(self->cursorRow == 3 || self->cursorRow == 4)
        index = self->cursorCol + 19;
    else
        index = self->cursorCol - 1;

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
        
    uint8_t index = 0;
    uint8_t bitmask = 0;
    uint8_t *lineBuffer = self->lineBuffer1;
    uint8_t bitPos = rowToBitPos[self->cursorRow];

    if(self->cursorRow == 2 || self->cursorRow == 4)
        lineBuffer = self->lineBuffer2;

    /* The memory addresses of a 4 line display are stored in two contiguous 
    address ranges. Row 3 begins after row 1 and row 4 begins after row 2. */
    if(self->cursorRow == 3 || self->cursorRow == 4)
        index = self->cursorCol + 19;
    else
        index = self->cursorCol - 1;

    /* flag left side for update */
    if(self->cursorCol <= (self->super->numCols+1) / 2)
        self->currentRefreshMask |= (LCD_PAR_LEFT << bitPos);

    /* Copy the data over. Stop when we hit the end of the row. Cursor will 
    stop at the next position. */
    do {
        if(self->cursorCol == ((self->super->numCols+1) / 2) + 1)
        {
            /* Single line displays split the line in half. If we cross the 
            halfway boundry, go to the line 2 buffer, left side. */
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

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Local Functions *****************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

static displayState GetNextState(LCD_Parallel *self)
{
    /* The states go from 0 to 7. This is just a quick method of doing modulo
    division */
    displayState nextState = (self->currentState + 1) & 0x07;

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