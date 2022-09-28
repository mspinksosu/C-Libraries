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
    struct {
        unsigned displayOn  :1;
        unsigned cursorOn   :1;
        unsigned blinkOn    :1;
        unsigned            :5;
    };
} LCDInitType;

typedef struct LCDInterfaceTag
{
    /* These are the functions that will be called. You will create your own
    interface object for your class that will have these function signatures.
    Set each of your functions equal to one of these pointers. The void pointer
    will be set to the sub class object. Typecasting will be needed. */
    void (*LCD_Init)(void *instance, void *params, uint8_t tickMs);
    void (*LCD_Tick)(void *instance);
    bool (*LCD_IsBusy)(void *instance);
    void (*LCD_WriteCommand)(void *instance, uint8_t command);
    void (*LCD_WriteData)(void *instance, uint8_t data);
    uint8_t (*LCD_ReadData)(void *instance);
    void (*LCD_ClearDisplay)(void *instance);
    void (*LCD_DisplayOn)(void *instance);
    void (*LCD_DisplayOff)(void *instance);
    void (*LCD_SetDisplayCursor)(void *instance, bool cursorOn);
    void (*LCD_SetCursorBlink)(void *instance, bool blinkEnabled);
    void (*LCD_MoveCursor)(void *instance, uint8_t row, uint8_t col);
    void (*LCD_MoveCursorForward)(void *instance);
    void (*LCD_MoveCursorBackward)(void *instance);
    void (*LCD_PutChar)(void *instance, uint8_t character);
    void (*LCD_PutString)(void *instance, uint8_t *ptrToString);
    void (*LCD_WriteFullLine)(void *instance, uint8_t lineNum, uint8_t *array, uint8_t size);
    //void (*LCD_ScrollLine)(void *instance, uint8_t lineNum, uint8_t *array, uint8_t size);
    //void (*LCD_SetCGRAMAddress)(void *instance, uint8_t address);
} LCDInterface;

typedef struct LCDTag
{
    LCDInterface *interface;
    void *instance;
    void (*DelayUs)(uint8_t delayInUs);
    uint8_t numRows;
    uint8_t numCols;
    LCDMode mode;
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

void LCD_SetInitTypeParams(LCDInitType *params, LCDMode mode, uint8_t numRows, 
    uint8_t numCols, bool displayOn, bool cursorOn, bool blinkOn);

void LCD_SetDelayUsFunc(LCD *self, void (*Function)(uint16_t delayInUs));

void LCD_PutInt(LCD *self, int16_t num, uint8_t width);

void LCD_PutFloat(LCD *self, float num, uint8_t precision);

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

void LCD_ClearDisplay(LCD *self);

void LCD_DisplayOn(LCD *self);

void LCD_DisplayOff(LCD *self);

void LCD_SetDisplayCursor(LCD *self, bool cursorOn);

void LCD_SetCursorBlink(LCD *self, bool blinkEnabled);

void LCD_MoveCursor(LCD *self, uint8_t row, uint8_t col);

void LCD_MoveCursorForward(LCD *self);

void LCD_MoveCursorBackward(LCD *self);

void LCD_PutChar(LCD *self, uint8_t character);

void LCD_PutString(LCD *self, uint8_t *ptrToString);

void LCD_WriteFullLine(LCD *self, uint8_t lineNum, uint8_t *array, uint8_t size);

// TODO
void LCD_ScrollLine(LCD *self, uint8_t lineNum, uint8_t *array, uint8_t size);

void LCD_SetCGRAMAddress(LCD *self, uint8_t address);

#endif  /* ILCD_H */