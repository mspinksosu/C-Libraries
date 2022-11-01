/***************************************************************************//**
 * @brief Foo Interface
 * 
 * @author Matthew Spinks
 * 
 * @date 12/2/14  Original creation
 * @date 8/6/22   Modified
 * 
 * @file IFoo.c
 * 
 * @details
 *      TODO
 * 
 ******************************************************************************/

#include "ILCD.h"
#include <stddef.h> // needed for NULL
#include <stdarg.h>

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

static uint8_t str[10];

// ***** Static Function Prototypes ********************************************


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Non-Interface Functions *********************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void LCD_Create(LCD *self, void *instanceOfSubclass, LCDInterface *interface)
{
    self->interface = interface;
    self->instance = instanceOfSubclass;
}

// *****************************************************************************

void LCD_CreateInitType(LCDInitType *params, void *instanceOfSubclass)
{
    params->instance = instanceOfSubclass;
}

// *****************************************************************************

void LCD_SetInitTypeToDefaultParams(LCDInitType *params)
{
    params->numRows = 2;
    params->numCols = 16;
    params->mode = LCD_READ_WRITE;
    params->displayOn = 1;
    params->cursorOn = 0;
    params->blinkOn = 0;
}

// *****************************************************************************

void LCD_SetInitTypeParams(LCDInitType *params, LCDMode mode, uint8_t numRows, 
    uint8_t numCols, uint8_t numArgs, ... )
{
    if(numRows < 1)
        numRows = 1;
    if(numCols < 1)
        numCols = 12;
    params->numRows = numRows;
    params->numCols = numCols;
    params->mode = mode;
    /* @follow-up va_arg promotes to int for char and short. If we don't use 
    int, we sometimes get a warning depending on the compiler */
    if(numArgs == 0)
        return;
    va_list list;
    va_start(list, numArgs);
    params->displayOn = va_arg(list, bool); // get the next argument
    if(--numArgs == 0)
        return;
    params->cursorOn = va_arg(list, bool); // get the next argument
    if(--numArgs == 0)
        return;
    params->blinkOn = va_arg(list, bool); // get the next argument
    if(--numArgs == 0)
        return;
    params->rowOverflow = va_arg(list, LCDRowOverflow); // get next argument
    if(--numArgs == 0)
        return;
    params->screenOverflow = va_arg(list, LCDScreenOverflow);
    va_end(list);
}

// *****************************************************************************

void LCD_SetDelayUsFunc(LCD *self, void (*Function)(uint16_t delayInUs))
{
    self->DelayUs = Function;
}

// *****************************************************************************

void LCD_SetTransmitByteFunc(LCD *self, void (*Function)(uint8_t data))
{
    self->TransmitByte = Function;
}

// *****************************************************************************

void LCD_SetReceiveByteFunc(LCD *self, uint8_t (*Function)(void))
{
    self->ReceiveByte = Function;
}

// *****************************************************************************

void LCD_PutInt(LCD *self, int16_t num, uint8_t width)
{
    if(self->interface->LCD_PutString != NULL && self->instance != NULL)
    {
        int n = sprintf(str, "%*d", width, num);
        
        if(n > 0)
        {
            (self->interface->LCD_PutString)(self->instance, str);
        }
    }
}

// *****************************************************************************

void LCD_PutFloat(LCD *self, float num, uint8_t precision)
{
    if(self->interface->LCD_PutString != NULL && self->instance != NULL)
    {
        float round = 0.5f;
        int n = 0;

        if(precision > 6)
            precision = 6;
        
        for(uint8_t i = 0; i < precision; i++) 
            round = round / 10.0f;
        
        if(num < 0.0)
        {
            /* If < 0, add 3 places for minus sign, 0, and decimal point */
            num -= round;
            n = sprintf(str, "%0*.*f", precision + 3, precision, num);
        }
        else
        {
            num += round;
            n = sprintf(str, "%.*f", precision, num);
        }
        
        if(n > 0)
        {
            (self->interface->LCD_PutString)(self->instance, str);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void LCD_Init(LCD *self, LCDInitType *params, uint16_t tickUs)
{
    if(self->interface->LCD_Init != NULL && self->instance != NULL 
        && params->instance != NULL)
    {
        self->numRows = params->numRows;
        self->numCols = params->numCols;
        self->mode = params->mode;
        self->rowOverflow = params->rowOverflow;
        self->screenOverflow = params->screenOverflow;
        (self->interface->LCD_Init)(self->instance, params->instance, tickUs);
    }
}

// *****************************************************************************

void LCD_Tick(LCD *self)
{
    if(self->interface->LCD_Tick != NULL && self->instance != NULL)
    {
        (self->interface->LCD_Tick)(self->instance);
    }
}

// *****************************************************************************

bool LCD_IsBusy(LCD *self)
{
    if(self->interface->LCD_IsBusy != NULL && self->instance != NULL)
    {
        return (self->interface->LCD_IsBusy)(self->instance);
    }
    else
    {
        return false;
    }
}

// *****************************************************************************

void LCD_WriteCommand(LCD *self, uint8_t command)
{
    if(self->interface->LCD_WriteCommand != NULL && self->instance != NULL)
    {
        (self->interface->LCD_WriteCommand)(self->instance, command);
    }
}

// *****************************************************************************

void LCD_WriteData(LCD *self, uint8_t data)
{
    if(self->interface->LCD_WriteData != NULL && self->instance != NULL)
    {
        (self->interface->LCD_WriteData)(self->instance, data);
    }
}

// *****************************************************************************

uint8_t LCD_ReadData(LCD *self)
{
    if(self->interface->LCD_ReadData != NULL && self->instance != NULL)
    {
        return (self->interface->LCD_ReadData)(self->instance);
    }
    else
    {
        return 0;
    }
}

// *****************************************************************************

void LCD_ClearDisplay(LCD *self)
{
    if(self->interface->LCD_ClearDisplay != NULL && self->instance != NULL)
    {
        (self->interface->LCD_ClearDisplay)(self->instance);
    }
}

// *****************************************************************************

void LCD_DisplayOn(LCD *self)
{
    if(self->interface->LCD_DisplayOn != NULL && self->instance != NULL)
    {
        (self->interface->LCD_DisplayOn)(self->instance);
    }
}

// *****************************************************************************

void LCD_DisplayOff(LCD *self)
{
    if(self->interface->LCD_DisplayOff != NULL && self->instance != NULL)
    {
        (self->interface->LCD_DisplayOff)(self->instance);
    }
}

// *****************************************************************************

void LCD_SetDisplayCursor(LCD *self, bool cursorOn)
{
    if(self->interface->LCD_SetDisplayCursor != NULL && self->instance != NULL)
    {
        (self->interface->LCD_SetDisplayCursor)(self->instance, cursorOn);
    }
}

// *****************************************************************************

void LCD_SetCursorBlink(LCD *self, bool blinkEnabled)
{
    if(self->interface->LCD_SetCursorBlink != NULL && self->instance != NULL)
    {
        (self->interface->LCD_SetCursorBlink)(self->instance, blinkEnabled);
    }
}

// *****************************************************************************

void LCD_MoveCursor(LCD *self, uint8_t row, uint8_t col)
{
    if(self->interface->LCD_MoveCursor != NULL && self->instance != NULL)
    {
        (self->interface->LCD_MoveCursor)(self->instance, row, col);
    }
}

// *****************************************************************************

void LCD_MoveCursorForward(LCD *self)
{
    if(self->interface->LCD_MoveCursorForward != NULL && self->instance != NULL)
    {
        (self->interface->LCD_MoveCursorForward)(self->instance);
    }
}

// *****************************************************************************

void LCD_MoveCursorBackward(LCD *self)
{
    if(self->interface->LCD_MoveCursorBackward != NULL && self->instance != NULL)
    {
        (self->interface->LCD_MoveCursorBackward)(self->instance);
    }
}

// *****************************************************************************

void LCD_PutChar(LCD *self, uint8_t character)
{
    if(self->interface->LCD_PutChar != NULL && self->instance != NULL)
    {
        (self->interface->LCD_PutChar)(self->instance, character);
    }
}

// *****************************************************************************

void LCD_PutString(LCD *self, uint8_t *ptrToString)
{
    if(self->interface->LCD_PutString != NULL && self->instance != NULL)
    {
        (self->interface->LCD_PutString)(self->instance, ptrToString);
    }
}

// *****************************************************************************

void LCD_WriteFullLine(LCD *self, uint8_t lineNum, uint8_t *array, uint8_t size)
{
    if(self->interface->LCD_WriteFullLine != NULL && self->instance != NULL)
    {
        (self->interface->LCD_WriteFullLine)(self->instance, lineNum, array, size);
    }
}

// *****************************************************************************

void LCD_ScrollDown(LCD *self)
{
    if(self->interface->LCD_ScrollDown != NULL && self->instance != NULL)
    {
        (self->interface->LCD_ScrollDown)(self->instance);
    }
}

// *****************************************************************************

void LCD_ScrollUp(LCD *self)
{
    if(self->interface->LCD_ScrollUp != NULL && self->instance != NULL)
    {
        (self->interface->LCD_ScrollUp)(self->instance);
    }
}

/*
 End of File
 */