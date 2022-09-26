/***************************************************************************//**
 * @brief Character LCD Interface Header File
 * 
 * @author Matthew Spinks
 * 
 * @date 9/24/22  Original creation
 * 
 * @file ILCD.h
 * 
 * @details
 *      // TODO 80 characters max, up to 4 rows and 20 columns, font size 5x8
 * 
 ******************************************************************************/

#ifndef ILCD_H
#define ILCD_H

#include <stdint.h>
#include <stdbool.h>

// ***** Defines ***************************************************************

#define LCD_ROW1_ADDR   0x00
#define LCD_ROW2_ADDR   0x40
#define LCD_ROW3_ADDR   0x14
#define LCD_ROW4_ADDR   0x54

// ***** Global Variables ******************************************************

typedef enum LCDModeTag
{
    LCD_READ_WRITE,
    LCD_WRITE_ONLY,
} LCDMode;

typedef struct LCDInitTypeTag
{
    void *instance;
    uint8_t numRows;
    uint8_t numCols;
    LCDMode mode;
} LCDInitType;

typedef struct LCDInterfaceTag
{
    /* These are the functions that will be called. You will create your own
    interface object for your class that will have these function signatures.
    Set each of your functions equal to one of these pointers. The void pointer
    will be set to the sub class object. Typecasting will be needed. */
    void (*Foo_Func)(void *instance);
    uint16_t (*Foo_GetValue)(void *instance);
    void (*Foo_SetValue)(void *instance, uint16_t data);

    // Add more functions below
} LCDInterface;

typedef struct LCDTag
{
    LCDInterface *interface;
    void *instance;
    void (*DelayUs)(uint8_t delayInUs);
    uint8_t numRows;
    uint8_t numCols;
    uint8_t cursorRow;
    uint8_t cursorCol;
    struct {
        unsigned busyFlag   :1;
        unsigned displayOn  :1;
        unsigned cursorOn   :1;
        unsigned blinkOn    :1;
        unsigned            :4;
    };
} LCD;

/**
 * Foo_Interface The table of functions that need to be implemented
 */

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Non-Interface Functions *********************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void LCD_Create(LCD *self, void *instanceOfSubclass, LCDInterface *interface);

void LCD_CreateInitType(LCDInitType *params, void *instanceOfSubclass);

void LCD_SetInitTypeToDefaultParams(LCDInitType *params);

void LCD_SetInitTypeParams(LCDInitType *params, uint8_t numRows, uint8_t numCols);

void LCD_SetDelayUsFunc(LCD *self, void (*Function)(uint8_t delayInUs));

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void LCD_Init(LCD *self, LCDInitType *params, uint8_t tickMs);

void LCD_Tick(LCD *self);

bool LCD_IsBusy(LCD *self);

void LCD_WriteCommand(LCD *self, uint8_t command);

void LCD_WriteData(LCD *self, uint8_t data);

uint8_t LCD_ReadData(LCD *self);

void LCD_DisplayOn(LCD *self);

void LCD_DisplayOff(LCD *self);

void LCD_SetDisplayCursor(LCD *self, bool cursorOn);

void LCD_SetCursorBlink(LCD *self, bool blinkEnabled);

void LCD_MoveCursor(LCD *self, uint8_t row, uint8_t col);

void LCD_MoveCursorForward(LCD *self);

void LCD_MoveCursorBackward(LCD *self);

void LCD_PutChar(LCD *self, uint8_t character);

void LCD_PutDigit(LCD *self, uint8_t convertThisDigitToChar);

void LCD_PutString(LCD *self, uint8_t *ptrToString);

void LCD_WriteFullLine(LCD *self, uint8_t lineNum, uint8_t *array, uint8_t size);

void LCD_ScrollLine(LCD *self, uint8_t lineNum, uint8_t scrollBoundry);

void LCD_SetCGRAMAddress(LCD *self, uint8_t address);

#endif  /* ILCD_H */