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
 *      // TODO details. A basic implementation for a HD44780 or similar LCD
 * character display
 * 
 ******************************************************************************/

#ifndef LCD_PAR_H
#define LCD_PAR_H

#include "ILCD.h"


// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

/* These are in order based on DDRAM address. First is row 1, then row 3, then 
row 2 starting at address 0x40, then row 4. */
typedef enum LCDParDisplayStateTag
{
    LCD_PAR_STATE_ROW1_LEFT = 0,
    LCD_PAR_STATE_ROW1_RIGHT,
    LCD_PAR_STATE_ROW3_LEFT,
    LCD_PAR_STATE_ROW3_RIGHT,
    LCD_PAR_STATE_ROW2_LEFT,
    LCD_PAR_STATE_ROW2_RIGHT,
    LCD_PAR_STATE_ROW4_LEFT,
    LCD_PAR_STATE_ROW4_RIGHT,
} LCDParDisplayState;

/* This is a bit mask that will match the typedef above starting from the LSb. 
I'm dividing the display into sections. Most people will keep a static image
somewhere on the screen. If I see that there is no change for that section of 
the display, I will skip over it to reduce the amount of writes. */
enum LCDParDisplayRefreshMask
{
    LCD_PAR_REFRESH_ROW1_LEFT = 0,
    LCD_PAR_REFRESH_ROW1_RIGHT,
    LCD_PAR_REFRESH_ROW3_LEFT,
    LCD_PAR_REFRESH_ROW3_RIGHT,
    LCD_PAR_REFRESH_ROW2_LEFT,
    LCD_PAR_REFRESH_ROW2_RIGHT,
    LCD_PAR_REFRESH_ROW4_LEFT,
    LCD_PAR_REFRESH_ROW4_RIGHT,
};

typedef enum LCDParInitStateTag
{
    LCD_PAR_INIT_HOME = 0,
    LCD_PAR_INIT_ENTRY,
    LCD_PAR_INIT_DISPLAY,
    LCD_PAR_INIT_FUNCTION,
} LCDParInitState;

typedef struct LCDParTimerTag
{
    uint16_t period;
    uint16_t count;
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

/* Create the sub class */
typedef struct LCD_ParallelTag
{
    LCD *super; // include the base class first
    void (*SetSelectPins)(bool rsPinHigh, bool rwPinHigh);
    void (*SetEnablePin)(bool setPinHigh);
    LCDParTimer clearDisplayTimer;
    uint8_t lineBuffer1[40];
    uint8_t lineBuffer2[40];
    uint8_t cursorRow;
    uint8_t cursorCol;
    uint8_t currentIndex;
    uint8_t count;
    uint8_t currentRefreshMask;
    LCDParDisplayState currentState;
    LCDParInitState initState;
    struct {
        unsigned displayOn          :1;
        unsigned cursorOn           :1;
        unsigned blinkOn            :1;
        unsigned use4BitMode        :1;
        unsigned updateAddressFlag  :1;
        unsigned refreshCursor      :1;
        unsigned initialize         :1;
        unsigned                    :1;
    };
} LCD_Parallel;

typedef struct LCDInitType_ParallelTag
{
    LCDInitType *super;
    bool use4BitMode;
} LCDInitType_Parallel;

/** 
 * Description of LCD_Parallel struct:
 * 
 * You shouldn't need to mess with any of these variables. That is why I have
 * made functions for you to use.
 * 
 * super  A pointer to the base class. Set by using the Create function
 * 
 * SetSelectPins  A pointer to a function that sets the RW, RS pins. This 
 *                function is required.
 * 
 * SetEnablePin  A pointer to a function that sets the E pin. This function is
 *               required.
 * 
 * clearDisplayTimer  A timer for waiting for the clear display or return home
 *                    commands to finish executing.
 * 
 * lineBuffer  Holds up to 80 characters to be displayed. Mimics the internal 
 *             memory of the LCD.
 * 
 * cursorRow  The row that the cursor is currently on
 * 
 * cursorCol  The column that the cursor is currently on
 * 
 * currentIndex  Keeps track of where we are in the lineBuffer array
 * 
 * count  Keeps track of how many characters we have written, so that we know
 *        when to go to the next section of the screen.
 * 
 * currentRefreshMask  Keeps track of which parts of the screen need updating
 * 
 * currentState  Used in conjunction with the refresh mask
 * 
 * initState  A smaller state machine that is used to send the inital commands
 *            to the LCD on startup
 * 
 * Description of LCDInitType_Parallel struct:
 * 
 * use4BitMode  Uses only the upper pins of the LCD (DB4 - DB7). Likewise, only
 *              uses the upper bits of the data byte. Each command is to be 
 *              sent twice.
 * 
 */

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Non-Interface Functions *********************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/***************************************************************************//**
 * @brief Connects the sub class to the base class
 * 
 * Calls the base class LCD_Create function. Each sub class object must have 
 * a base class.
 * 
 * @param self  pointer to the sub class LCD_Parallel that you are using
 * 
 * @param base  pointer to the base class LCD that you are using
 */
void LCD_Parallel_Create(LCD_Parallel *self, LCD *base);

/***************************************************************************//**
 * @brief Connects the sub class to the base class
 * 
 * Calls the base class LCD_CreateInitType function. Each sub class object 
 * must have a base class.
 * 
 * @param self  pointer to the sub class InitType_Parallel you are using
 * 
 * @param base  pointer to the base class LCDInitType that you are using
 */
void LCD_Parallel_CreateInitType(LCDInitType_Parallel *self, LCDInitType *base);

/***************************************************************************//**
 * @brief Set the initial mode to 4-bit
 * 
 * The default mode is 8-bit. In 4-bit mode the LCD only uses the upper pins 
 * (DB4 - DB7). The data should be made left justified to match. Every data 
 * transfer will require two calls to TransmitByte or ReceiveByte. First, 
 * the upper nibble is transmitted or received. Then the lower nibble.
 * 
 * @param self  pointer to the LCDInitType_Parallel that you are using
 * 
 * @param use4BitMode  if true, use 4-bit mode
 */
void LCD_Parallel_Set4BitMode(LCDInitType_Parallel *self, bool use4BitMode);

/***************************************************************************//**
 * @brief Set a function to allow the LCD control of the RS and RW pins
 * 
 * Your function should follow the format listed below. If the argument passed 
 * in is true, set the corresponding pin high. Else, low.
 * 
 * @param self  pointer to the LCD_Parallel that you are using
 * 
 * @param Function format: void someFunction(bool rsPinHigh, bool rwPinHigh)
 */
void LCD_Parallel_SetSelectPinsFunc(LCD_Parallel *self, void (*Function)(bool rsPinHigh, bool rwPinHigh));

/***************************************************************************//**
 * @brief Set a function to allow the LCD control of the E pin
 * 
 * Your function should follow the format listed below. If the argument passed 
 * in is true, set the corresponding pin high. Else, low.
 * 
 * @param self  pointer to the LCD_Parallel that you are using
 * 
 * @param Function format: void someFunction(bool setEnablePinHigh)
 */
void LCD_Parallel_SetEnablePinFunc(LCD_Parallel *self, void (*Function)(bool setPinHigh));

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void LCD_Parallel_Init(LCD_Parallel *self, LCDInitType_Parallel *params, uint16_t tickUs);

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

void LCD_Parallel_GetCursorPosition(LCD_Parallel *self, uint8_t *retRow, uint8_t *retCol);

void LCD_Parallel_PutChar(LCD_Parallel *self, uint8_t character);

void LCD_Parallel_PutString(LCD_Parallel *self, uint8_t *ptrToString);

void LCD_Parallel_WriteFullLine(LCD_Parallel *self, uint8_t lineNum, uint8_t *array, uint8_t size);

void LCD_Parallel_ScrollDown(LCD_Parallel *self);

void LCD_Parallel_ScrollUp(LCD_Parallel *self);

//void LCD_Parallel_ScrollLine(LCD_Parallel *self, uint8_t lineNum, uint8_t *array, uint8_t size);

//void LCD_Parallel_SetCGRAMAddress(LCD_Parallel *self, uint8_t address);

#endif  /* LCD_PAR_H */