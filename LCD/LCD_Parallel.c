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

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

/*  The sub class must implement the functions provided in the interface. In 
    this case we are declaring an interface struct and initializing its members 
    (which are function pointers) the our local functions. Typecasting is 
    necessary. When a new sub class object is created, we will set its interface
    member equal to this table. */
LCDInterface LCD_ParallelFunctionTable = {

};

// ***** Static Function Prototypes ********************************************

/* Put static function prototypes here */


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
    if(row > self->super->numRows || col > self->super->numCols)
        return;

    // if single row, check halfway boundry
    if(self->super->numRows == 1 && col >= self->super->numCols / 2)
    {
        // Add 0x40 to address
    }

    // write address
}

void LCD_Parallel_MoveCursorForward(LCD_Parallel *self)
{
    // check rows and columns

    // if single row, check halfway boundry

    // else if last column, move to next row

    // write address
}

void LCD_Parallel_MoveCursorBackward(LCD_Parallel *self)
{
    // check rows and columns

    // if single row, check halfway boundry

    // else if first column, move to previous row

    // write address
}

void LCD_Parallel_PutChar(LCD_Parallel *self, uint8_t character)
{

}

void LCD_Parallel_PutDigit(LCD_Parallel *self, uint8_t convertThisDigitToChar)
{

}

void LCD_Parallel_PutString(LCD_Parallel *self, uint8_t *ptrToString)
{

}

void LCD_Parallel_WriteFullLine(LCD_Parallel *self, uint8_t lineNum, uint8_t *array, uint8_t size)
{

}

void LCD_Parallel_ScrollLine(LCD_Parallel *self, uint8_t lineNum, uint8_t scrollBoundry)
{

}

void LCD_Parallel_SetCGRAMAddress(LCD_Parallel *self, uint8_t address)
{

}

/*
 End of File
 */