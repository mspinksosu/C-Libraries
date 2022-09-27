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
    static displayState nextState;

    /* TODO Replace LCD busy while loop with some sort of non-blocking function
    or a retry */
    if(self->currentRefreshMask == 0)
    {
        /* We've finished updating the screen. There is nothing else to do 
        except place the cursor before returning if needed. 
        The command is 0x80 + row addr + col */
        if(self->refreshCursor)
        {
            self->refreshCursor = false;
            while(LCD_Parallel_IsBusy(self)){}
            switch(self->super->cursorRow)
            {
                case 1:
                    LCD_Parallel_WriteCommand(self, 0x80 + self->super->cursorCol - 1);
                    break;
                case 2:
                    LCD_Parallel_WriteCommand(self, 0xC0 + self->super->cursorCol - 1);
                    break;
                case 3:
                    LCD_Parallel_WriteCommand(self, 0x80 + self->super->cursorCol + 19);
                    break;
                case 4:
                    LCD_Parallel_WriteCommand(self, 0xC0 + self->super->cursorCol + 19);
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

void LCD_Parallel_ClearDisplay(LCD_Parallel *self)
{
    for(uint8_t i = 0; i < sizeof(self->lineBuffer1); i++)
    {
        self->lineBuffer1[i] = ' ';
        self->lineBuffer2[i] = ' ';
    }
    self->currentRefreshMask = 0xFF;
    self->refreshCursor = true;
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
    self->super->cursorCol + 1;
    if(self->super->cursorCol > self->super->numCols)
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
    self->super->cursorCol - 1;
    if(self->super->cursorCol < 1)
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

void LCD_Parallel_PutDigit(LCD_Parallel *self, uint8_t digit)
{
    
}

void LCD_Parallel_PutString(LCD_Parallel *self, uint8_t *ptrToString)
{
    if(*ptrToString == '\0')
        return;
        
    uint8_t index = 0;
    uint8_t bitmask = 0;
    uint8_t *lineBuffer = self->lineBuffer1;
    uint8_t bitPos = rowToBitPos[self->super->cursorRow];

    if(self->super->cursorRow == 2 || self->super->cursorRow == 4)
        lineBuffer = self->lineBuffer2;

    /* The memory addresses of a 4 line display are stored in two contiguous 
    address ranges. Row 3 begins after row 1 and row 4 begins after row 2. */
    if(self->super->cursorRow == 3 || self->super->cursorRow == 4)
        index = self->super->cursorCol + 19;
    else
        index = self->super->cursorCol - 1;

    /* flag left side for update */
    if(self->super->cursorCol <= (self->super->numCols+1) / 2)
        self->currentRefreshMask |= (LCD_PAR_LEFT << bitPos);

    /* Copy the data over. Stop when we hit the end of the row. Cursor will 
    stop at the next position. */
    do {
        if(self->super->cursorCol == ((self->super->numCols+1) / 2) + 1)
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
        self->super->cursorCol++;
    } while(*ptrToString != '\0' || self->super->cursorCol < self->super->numCols);
    LCD_Parallel_MoveCursorForward(self); // TODO decide where to put the cursor
}

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

void LCD_Parallel_ScrollLine(LCD_Parallel *self, uint8_t lineNum, uint8_t *array, uint8_t size)
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