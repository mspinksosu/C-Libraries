/***************************************************************************//**
 * @brief Parallel Character Display Library Implementation Header
 * 
 * @author Matthew Spinks
 * 
 * @date 9/24/22  Original creation
 * 
 * @file LCD_Parallel.h
 * 
 * @details
 *      TODO
 * 
 ******************************************************************************/

#ifndef LCD_PAR_H
#define LCD_PAR_H

#include "ILCD.h"


// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

/* These are in order based on DDRAM address. First is row 1, then row 3, then 
row 2 starting at address 0x40, then row 4. */
typedef enum displayStateTag
{
    LCD_PAR_STATE_ROW1_LEFT = 0,
    LCD_PAR_STATE_ROW1_RIGHT,
    LCD_PAR_STATE_ROW3_LEFT, // address 0x14
    LCD_PAR_STATE_ROW3_RIGHT,
    LCD_PAR_STATE_ROW2_LEFT, // address 0x40
    LCD_PAR_STATE_ROW2_RIGHT,
    LCD_PAR_STATE_ROW4_LEFT, // address 0x54
    LCD_PAR_STATE_ROW4_RIGHT,
} displayState;

/* This is a bit mask that will match the typedef above starting from the LSb. 
I'm dividing the display into sections. Most people will keep a static image
somewhere on the screen. If I see that there is no change for that section of 
the display, I will skip over it to reduce the amount of writes. */
enum displayRefreshMask
{
    LCD_PAR_REFRESH_ROW1_LEFT = 0,
    LCD_PAR_REFRESH_ROW1_RIGHT,
    LCD_PAR_REFRESH_ROW3_LEFT,
    LCD_PAR_REFRESH_ROW3_RIGHT,
    LCD_PAR_REFRESH_ROW2_LEFT, // 0x40
    LCD_PAR_REFRESH_ROW2_RIGHT,
    LCD_PAR_REFRESH_ROW4_LEFT,
    LCD_PAR_REFRESH_ROW4_RIGHT,
};

typedef struct LCDParTimerTag
{
    uint8_t period;
    uint8_t count;
    union {
        struct {
            unsigned start      :1;
            unsigned active     :1;
            unsigned expired    :1;
            unsigned            :5;
        };
        uint8_t all;
    } flags;
} LCDParTimer;

/* If you need to extend the base class, then declare your processor specific
class here. Your processor specific functions should all use this type in place 
of the base class type. */
typedef struct LCD_ParallelTag
{
    LCD *super; // include the base class first
    void (*SetSelectPins)(bool rsPinHigh, bool rwPinHigh);
    void (*SetEnablePin)(bool setPinHigh);
    void (*SetDataPins)(uint8_t data, bool nibble);
    LCDParTimer clearDisplayTimer;
    uint8_t (*ReadDataPins)(void);
    uint8_t lineBuffer1[40];
    uint8_t lineBuffer2[40];
    bool updateAddressFlag;
    bool refreshCursor;
    uint8_t cursorRow;
    uint8_t cursorCol;
    uint8_t currentIndex;
    uint8_t count;
    uint8_t currentRefreshMask;
    displayState currentState;
    struct {
        unsigned displayOn      :1;
        unsigned cursorOn       :1;
        unsigned blinkOn        :1;
        unsigned use4BitMode    :1;
        unsigned                :4;
    };
} LCD_Parallel;

/** 
 * Description of struct
 * 
 * member1      description of variable member1
 * 
 */

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Non-Interface Functions *********************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void LCD_Parallel_Create(LCD_Parallel *self, LCD *base);

void LCD_Parallel_Set4BitMode(LCD_Parallel *self, bool use4BitMode);

void LCD_Parallel_SetSelectPinsFunc(LCD_Parallel *self, void (*Function)(bool rsPinHigh, bool rwPinHigh));

void LCD_Parallel_SetEnablePinFunc(LCD_Parallel *self, void (*Function)(bool setPinHigh));

void LCD_Parallel_SetDataPinsFunc(LCD_Parallel *self, void (*Function)(uint8_t data, bool nibble));

void LCD_Parallel_ReadDataPinsFunc(LCD_Parallel *self, uint8_t (*Function)(void));

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void LCD_Parallel_Init(LCD_Parallel *self, LCDInitType *params, uint8_t tickMs);

void LCD_Parallel_Tick(LCD_Parallel *self);

bool LCD_Parallel_IsBusy(LCD_Parallel *self);

void LCD_Parallel_WriteCommand(LCD_Parallel *self, uint8_t command);

void LCD_Parallel_WriteData(LCD_Parallel *self, uint8_t data);

uint8_t LCD_Parallel_ReadData(LCD_Parallel *self);

void LCD_Parallel_ClearDisplay(LCD_Parallel *self);

void LCD_Parallel_DisplayOn(LCD_Parallel *self);

void LCD_Parallel_DisplayOff(LCD_Parallel *self);

void LCD_Parallel_SetDisplayCursor(LCD_Parallel *self, bool cursorOn);

void LCD_Parallel_SetCursorBlink(LCD_Parallel *self, bool blinkEnabled);

void LCD_Parallel_MoveCursor(LCD_Parallel *self, uint8_t row, uint8_t col);

void LCD_Parallel_MoveCursorForward(LCD_Parallel *self);

void LCD_Parallel_MoveCursorBackward(LCD_Parallel *self);

void LCD_Parallel_PutChar(LCD_Parallel *self, uint8_t character);

void LCD_Parallel_PutString(LCD_Parallel *self, uint8_t *ptrToString);

void LCD_Parallel_WriteFullLine(LCD_Parallel *self, uint8_t lineNum, uint8_t *array, uint8_t size);

//void LCD_Parallel_ScrollLine(LCD_Parallel *self, uint8_t lineNum, uint8_t *array, uint8_t size);

//void LCD_Parallel_SetCGRAMAddress(LCD_Parallel *self, uint8_t address);

#endif  /* LCD_PAR_H */