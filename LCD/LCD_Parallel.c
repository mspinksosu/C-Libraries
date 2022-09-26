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

#define LCD_PAR_LEFT    0x01
#define LCD_PAR_RIGHT   0x02

// ***** Global Variables ******************************************************

/*  The sub class must implement the functions provided in the interface. In 
    this case we are declaring an interface struct and initializing its members 
    (which are function pointers) the our local functions. Typecasting is 
    necessary. When a new sub class object is created, we will set its interface
    member equal to this table. */
LCDInterface LCD_ParallelFunctionTable = {

};

/* For use with the displayRefreshMask variable */
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

}

void LCD_Parallel_Tick(LCD_Parallel *self)
{
    /* TODO Replace LCD busy while loop with some sort of non-blocking function
    or a retry */
    displayState nextState = GetNextState(self);

    if(nextState == self->currentState)
        return;

    switch(self->currentState)
    {
        case LCD_PAR_REFRESH_ROW1_LEFT:
            if(self->super->cursorCol == 1 || self->updateAddressFlag)
            {
                /* Set the DDRAM address to the row + column.
                Row 1 address is 0x00. Column address is 0-19
                Cmd = 0x80 + row addr + col - 1 = 0x80 + 0x00 + col - 1*/
                while(LCD_Parallel_IsBusy(self)){}
                LCD_Parallel_WriteCommand(self, 0x80 + self->super->cursorCol - 1);
                self->updateAddressFlag = false;
            }
            while(LCD_Parallel_IsBusy(self)){}
            LCD_Parallel_WriteData(self, self->lineBuffer1[self->super->cursorCol - 1]);
            self->super->cursorCol++;
            if(self->super->cursorCol > (self->super->numCols+1) / 2)
            {
                if(nextState != self->currentState + 1)
                    self->updateAddressFlag = true;
                self->currentState = nextState;
            }
            break;
        case LCD_PAR_REFRESH_ROW3_LEFT:
        case LCD_PAR_REFRESH_ROW3_RIGHT:
            if(self->updateAddressFlag)
            {
                /* Set the DDRAM address to the row + column.
                Row 3 address is 0x00. Column address is cursor columm 19
                Cmd = 0x80 + row addr + col = 0x80 + 0x00 + col */
                while(LCD_Parallel_IsBusy(self)){}
                LCD_Parallel_WriteCommand(self, 0x80 + self->super->cursorCol);
                self->updateAddressFlag = false;
            }
            while(LCD_Parallel_IsBusy(self)){}
            LCD_Parallel_WriteData(self, self->lineBuffer1[self->super->cursorCol]);
            self->super->cursorCol++;
            if(self->super->cursorCol > (self->super->numCols+1) / 2)
            {
                if(nextState != self->currentState + 1)
                    self->updateAddressFlag = true;
                self->currentState = nextState;
            }
            break;
    }
}

bool LCD_Parallel_IsBusy(LCD_Parallel *self)
{

}

void LCD_Parallel_WriteCommand(LCD_Parallel *self, uint8_t command)
{

}

void LCD_Parallel_WriteData(LCD_Parallel *self, uint8_t data)
{

}

uint8_t LCD_Parallel_ReadData(LCD_Parallel *self)
{

}

void LCD_Parallel_DisplayOn(LCD_Parallel *self)
{

}

void LCD_Parallel_DisplayOff(LCD_Parallel *self)
{

}

void LCD_Parallel_SetDisplayCursor(LCD_Parallel *self, bool cursorOn)
{

}

void LCD_Parallel_SetCursorBlink(LCD_Parallel *self, bool blinkEnabled)
{

}

void LCD_Parallel_MoveCursor(LCD_Parallel *self, uint8_t row, uint8_t col)
{
    // check rows and columns
    if(row == 0 || col == 0 || 
        row > self->super->numRows || col > self->super->numCols)
            return;

    self->super->cursorRow = row;
    self->super->cursorCol = col;

    self->refreshCursor = true;
}

void LCD_Parallel_MoveCursorForward(LCD_Parallel *self)
{
    if(self->super->cursorCol == self->super->numCols)
    {
        self->super->cursorCol = 1;
        self->super->cursorRow++;
    }

    if(self->super->cursorRow > self->super->numRows)
        self->super->cursorRow = 1;

    self->refreshCursor = true;
}

void LCD_Parallel_MoveCursorBackward(LCD_Parallel *self)
{
    if(self->super->cursorCol == 1)
    {
        self->super->cursorCol = self->super->numCols;
        self->super->cursorRow--;
    }

    if(self->super->cursorRow < 1)
        self->super->cursorRow = self->super->numRows;
    
    self->refreshCursor = true;
}

void LCD_Parallel_PutChar(LCD_Parallel *self, uint8_t character)
{
    uint8_t index = 0;
    uint8_t bitmask = LCD_PAR_LEFT;
    uint8_t *lineBuffer;
    uint8_t bitPos = rowToBitPos[self->super->cursorRow];

    /* The memory addresses of a 4 line display are stored in two contiguous 
    address ranges. Row 3 begins after row 1 and row 4 begins after row 2. */
    if(self->super->cursorRow == 3 || self->super->cursorRow == 4)
        index = self->super->cursorCol + 19;
    else
        index = self->super->cursorCol - 1;

    /* If we are further than halfway across, update the right side. Else, just
    the left side. For the bitmask, left side = 0x01 and right side = 0x02 */
    if(self->super->cursorCol > (self->super->numCols+1) / 2)
        bitmask = LCD_PAR_RIGHT;

    if(self->super->cursorRow == 1 || self->super->cursorRow == 3)
    {
        /* Single line displays split the line in half. If we are on the 
        right side, go to the line 2 buffer. */
        if(self->super->numRows == 1 && bitmask == LCD_PAR_RIGHT)
            lineBuffer = self->lineBuffer2;
        else
            lineBuffer = self->lineBuffer1;
    }
    else
    {
        lineBuffer = self->lineBuffer2;
    }

    lineBuffer[index] = character;
    self->nextRefreshMask |= (bitmask << bitPos);
}

void LCD_Parallel_PutDigit(LCD_Parallel *self, uint8_t digit)
{
    
}

void LCD_Parallel_PutString(LCD_Parallel *self, uint8_t *ptrToString)
{
    uint8_t index = 0;
    uint8_t bitmask = 0;
    uint8_t *lineBuffer = self->lineBuffer1;
    uint8_t bitPos = rowToBitPos[self->super->cursorRow];
    uint8_t maxLength = self->super->numCols - self->super->cursorCol + 1;

    /* The memory addresses of a 4 line display are stored in two contiguous 
    address ranges. Row 3 begins after row 1 and row 4 begins after row 2. */
    if(self->super->cursorRow == 3 || self->super->cursorRow == 4)
        index = self->super->cursorCol + 19;
    else
        index = self->super->cursorCol - 1;

    // flag left side for update
    if(self->super->cursorCol <= (self->super->numCols+1) / 2)
        bitmask |= LCD_PAR_LEFT;

    if(self->super->cursorRow == 2 || self->super->cursorRow == 4)
        lineBuffer = self->lineBuffer2;

    /* Stop at the end of the current row */
    uint8_t j = 0;
    while(*ptrToString != '\0' || j < maxLength)
    {
        lineBuffer[index] = *ptrToString;
        j++;
        ptrToString++;
    }

    if(self->super->cursorCol > (self->super->numCols+1) / 2)
        bitmask |= LCD_PAR_RIGHT;
    self->nextRefreshMask |= (bitmask << bitPos);
}

void LCD_Parallel_WriteFullLine(LCD_Parallel *self, uint8_t lineNum, uint8_t *array, uint8_t size)
{
    if(size > 20)
        size = 20;

    if(lineNum == 1 || lineNum == 3)
        memcpy(self->lineBuffer1, array, size);
    else if(lineNum == 2 || lineNum == 4)
        memcpy(self->lineBuffer2, array, size);
}

void LCD_Parallel_ScrollLine(LCD_Parallel *self, uint8_t lineNum, uint8_t scrollBoundry)
{

}

void LCD_Parallel_SetCGRAMAddress(LCD_Parallel *self, uint8_t address)
{

}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Local Functions *****************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

static displayState GetNextState(LCD_Parallel *self)
{
    displayState nextState = self->currentState + 1;

    while(nextState != self->currentState)
    {
        if(self->currentRefreshMask & (1 << nextState))
            break;
        nextState++;

        if(nextState == 8)
        {
            /* We finished going through all the states. Load new values and start 
            over. */
            self->currentRefreshMask = self->nextRefreshMask;
            self->currentState = 0;
        }
    }
}

/*
 End of File
 */