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

typedef enum LCDRowOverflowTag
{
    LCD_WRAP_NONE = 0,
    LCD_CHARACTER_WRAP,
    LCD_WORD_WRAP,
} LCDRowOverflow;

typedef enum LCDScreenOverflowTag
{
    LCD_OVERFLOW_STOP = 0,
    LCD_OVERFLOW_WRAP_AROUND,
    LCD_OVERFLOW_SCROLL_DOWN,
} LCDScreenOverflow;

typedef struct LCDInitTypeTag
{
    void *instance;
    uint8_t numRows;
    uint8_t numCols;
    LCDMode mode;
    bool displayOn;
    bool cursorOn;
    bool blinkOn;
    LCDRowOverflow rowOverflow; // TODO
    LCDScreenOverflow screenOverflow;
} LCDInitType;

typedef struct LCDInterfaceTag
{
    /* These are the functions that will be called. You will create your own
    interface object for your class that will have these function signatures.
    Set each of your functions equal to one of these pointers. The void pointer
    will be set to the sub class object. Typecasting will be needed. */
    void (*LCD_Init)(void *instance, void *params, uint16_t tickUs);
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
    void (*LCD_GetCursorPosition)(void *instance, uint8_t *retRow, uint8_t *retCol);
    void (*LCD_PutChar)(void *instance, uint8_t character);
    void (*LCD_PutString)(void *instance, uint8_t *ptrToString);
    void (*LCD_WriteFullLine)(void *instance, uint8_t lineNum, uint8_t *array, uint8_t size);
    void (*LCD_ScrollDown)(void *instance);
    void (*LCD_ScrollUp)(void *instance);
    //void (*LCD_ScrollLine)(void *instance, uint8_t lineNum, uint8_t *array, uint8_t size);
    //void (*LCD_SetCGRAMAddress)(void *instance, uint8_t address);
} LCDInterface;

typedef struct LCDTag
{
    LCDInterface *interface;
    void *instance;
    void (*DelayUs)(uint8_t delayInUs);
    void (*TransmitByte)(uint8_t byte);
    uint8_t (*ReceiveByte)(void);
    uint8_t numRows;
    uint8_t numCols;
    LCDMode mode;
    LCDRowOverflow rowOverflow; // TODO
    LCDScreenOverflow screenOverflow;
} LCD;

/**
 * Foo_Interface The table of functions that need to be implemented
 */

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Non-Interface Functions *********************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/***************************************************************************//**
 * @brief Link the base class, sub class, and function table
 * 
 * My preferred method is to call this function from a sub class constructor. 
 * This makes the create function more type safe. To create a sub class 
 * constructor, make your own Create function that takes your sub class and 
 * base class as arguments. Set your variables then call this function from it.
 * 
 * @param self  pointer to the LCD that you are using
 * 
 * @param instanceOfSubclass  the child object that inherits from LCD
 * 
 * @param interface  pointer to the function table that your LCD uses
 */
void LCD_Create(LCD *self, void *instanceOfSubclass, LCDInterface *interface);

/***************************************************************************//**
 * @brief Combine the base class and sub class LCDInitType
 * 
 * My preferred method is to call this function from a sub class constructor. 
 * This makes the create function more type safe. To create a sub class 
 * constructor, make your own Create function that takes your sub class and 
 * base class as arguments. Set your variables then call this function from it.
 * 
 * @param params  pointer to the LCDInitType that you are using
 * 
 * @param instanceOfSubclass  the child object that inherits from LCDInitType
 */
void LCD_CreateInitType(LCDInitType *params, void *instanceOfSubclass);

/***************************************************************************//**
 * @brief Set the init type object to its default values
 * 
 * mode = LCD_READ_WRITE, numRows = 2, numCols = 16, 
 * display = ON, cursor = OFF, cursor blink = OFF,
 * LCDRowOverflow = LCD_WRAP_NONE, screenOverflow = LCD_OVERFLOW_STOP
 * 
 * @param params  pointer to the LCDInitType that you are using
 */
void LCD_SetInitTypeToDefaultParams(LCDInitType *params);

/***************************************************************************//**
 * @brief Set the initial values for the LCDInitType
 * 
 * There are some extra parameters as part of an argument list to reduce the 
 * size of the function. Make sure to list your arguments in the order given.
 * 
 * Alternatively, you can set the values of the type members directly. But 
 * sometimes not every member is meant to be set by the user initially, so 
 * please read the decscription of the struct members if you do it that way.
 * 
 * @param params  pointer to the LCDInitType that you are using
 * @param mode  LCD_READ_WRITE or LCD_WRITE_ONLY
 * @param numRows  the number of rows
 * @param numCols  the number of columns
 * @param numArgs  remaining number of arguments (0 to 5)
 * @param displayOn  bool: if true, turn display on initially (default true)
 * @param cursorOn  bool: if true, turn the cursor on (default false)
 * @param blinkOn  bool: if true, turn the cursor blink on (default false)
 * @param rowOverflow  type LCDRowOverflow. Default = LCD_WRAP_NONE
 * @param screenOverflow  type LCDScreenOverflow. Default = LCD_OVERFLOW_STOP
 */
void LCD_SetInitTypeParams(LCDInitType *params, LCDMode mode, uint8_t numRows, 
    uint8_t numCols, uint8_t numArgs, ... );

/***************************************************************************//**
 * @brief Set a function to allow the LCD to call for a delay (in us)
 * 
 * The LCD library should only delay a few microseconds at most. Normally, when
 * setting pins such as E, RS, RW, or the data pins. It is possible, that it
 * could delay up to 100 us when trying to read from the LCD. You should never
 * be delaying more than this. Your function should follow the format listed
 * below. It should accept the number of microseconds to delay as a uint16_t.
 * 
 * @param self  pointer to the LCD that you are using
 * 
 * @param Function  format: void someFunction(uint16_t delayInUs)
 */
void LCD_SetDelayUsFunc(LCD *self, void (*Function)(uint16_t delayInUs));

/***************************************************************************//**
 * @brief Set a function to transmit a byte to the LCD
 * 
 * Your function should follow the format listed below. It will give a byte to 
 * LCD when it is called.
 * 
 * @param self  pointer to the LCD that you are using
 * 
 * @param Function  format: void someFunction(uint8_t dataToSend)
 */
void LCD_SetTransmitByteFunc(LCD *self, void (*Function)(uint8_t data));

/***************************************************************************//**
 * @brief Set a function to receive a byte from the LCD
 * 
 * Your function should follow the format listed below. It will return a byte to
 * the LCD when it is called. 
 * 
 * @param self  pointer to the LCD that you are using
 * 
 * @param Function  format: uint8_t SomeFunction(void)
 */
void LCD_SetReceiveByteFunc(LCD *self, uint8_t (*Function)(void));

/***************************************************************************//**
 * @brief Print an int at the current cursor position
 * 
 * A simple function that will print numbers for you. This function will call 
 * your implementation of PutString and give it a null terminated string.
 * 
 * @param self  pointer to the LCD that you are using
 * 
 * @param num  the number you wish to print
 * 
 * @param width  the minimum number of digits to print
 */
void LCD_PutInt(LCD *self, int16_t num, uint8_t width);

/***************************************************************************//**
 * @brief Print a float at the current cursor position
 * 
 * A simple function that will print a float for you with limited precision. 
 * This function will call your implementation of PutString and give it a null 
 * terminated string. It has a maximum precision of 6 decimal places. It will
 * round your number to the number of decimals you specify. If you need more 
 * options when printing a float, consider using sprintf instead.
 * 
 * @param self  pointer to the LCD that you are using
 * 
 * @param num  the number you wish to print
 * 
 * @param precision  the number of digits after the decimal (0 to 6)
 */
void LCD_PutFloat(LCD *self, float num, uint8_t precision);

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/***************************************************************************//**
 * @brief Initialize the LCD and the tick rate in microseconds
 * 
 * The tick rate should preferably be greater than 100 us. The write functions
 * will each have a small delay of about 1 us, but the busy function could 
 * delay up to 100 us. The maximum amount of delay should 100 us. Once the 
 * screen is written to, there is no need to continuously update it. The 
 * controller in the LCD will hold the characters being displayed. I think a 
 * 1 or 2 millisecond (1000 - 2000 us) tick rate works best. The number of rows
 * and columns begins with 1 and goes to numRows and numCols. By default, after
 * a write to the LCD, the address should increment.
 * 
 * @param self  pointer to the LCD that you are using
 * 
 * @param params  pointer to the LCDInitType that you are using
 * 
 * @param tickUs  the tick rate in microseconds
 */
void LCD_Init(LCD *self, LCDInitType *params, uint16_t tickUs);

/***************************************************************************//**
 * @brief Update the characters on the display
 * 
 * The maximum amount of time spent in this function should preferably be 
 * 200 us or less. Typically, you will call the IsBusy function to generate a 
 * read. Then afterwards a write. Then exit, and come back and write the next 
 * character to be displayed on the next function call. The LCD read and write
 * commands will have a slight delay of 1 to 2 us. This is to allow time for 
 * setup of pins (such as E,RS,RW). In reality, this time will be longer 
 * though, because it takes a little extra time to set the pins themselves.
 * 
 * @param self  pointer to the LCD that you are using
 */
void LCD_Tick(LCD *self);

/***************************************************************************//**
 * @brief Check if the LCD is currently busy
 * 
 * Perform an LCD read to check the busy flag of the LCD. If you cannot perform
 * an LCD read, then call the DelayUs function pointer and use a short delay.
 * (preferrably less than 100 us). If the DelayUs function isn't set, then just
 * take a shot in the dark and generate some sort of delay as a last resort.
 * 
 * @param self  pointer to the LCD that you are using
 * 
 * @return true if the LCD is busy
 */
bool LCD_IsBusy(LCD *self);

/***************************************************************************//**
 * @brief Send a command to the LCD
 * 
 * Do any necessary setup for a write command operation. Set the E, RS, RW pins 
 * if applicable, then call the TransmitByte function.
 * 
 * @param self  pointer to the LCD that you are using
 * 
 * @param command  the command or instruction for the LCD to execute
 */
void LCD_WriteCommand(LCD *self, uint8_t command);

/***************************************************************************//**
 * @brief Send a command to the LCD
 * 
 * Do any necessary setup for a write data operation. Set the E, RS, RW pins if
 * applicable, then call the TransmitByte function.
 * 
 * @param self  pointer to the LCD that you are using
 * 
 * @param data  the data to be written to the LCD
 */
void LCD_WriteData(LCD *self, uint8_t data);

/***************************************************************************//**
 * @brief Send a command to the LCD
 * 
 * Do any necessary setup for a read data operation. Set the E, RS, RW pins if
 * applicable. Call ReceiveByte, then return that data to the user.
 * 
 * @param self  pointer to the LCD that you are using
 * 
 * @param data  the data to be written to the LCD
 */
uint8_t LCD_ReadData(LCD *self);

/***************************************************************************//**
 * @brief Clear the display
 * 
 * Usually this means sending a clear display instruction. If needed, set a 
 * busy flag for the tick function, and a counter. Do not use a delay.
 * 
 * @param self  pointer to the LCD that you are using
 */
void LCD_ClearDisplay(LCD *self);

/***************************************************************************//**
 * @brief Turn the entire display on
 * 
 * @param self  pointer to the LCD that you are using
 */
void LCD_DisplayOn(LCD *self);

/***************************************************************************//**
 * @brief Turn the entire display on
 * 
 * @param self  pointer to the LCD that you are using
 */
void LCD_DisplayOff(LCD *self);

/***************************************************************************//**
 * @brief Turn the cursor on or off
 * 
 * @param self  pointer to the LCD that you are using
 * 
 * @param cursorOn  turn the cursor on if true
 */
void LCD_SetDisplayCursor(LCD *self, bool cursorOn);

/***************************************************************************//**
 * @brief Enable or disable the cursor blink
 * 
 * @param self  pointer to the LCD that you are using
 * 
 * @param cursorOn  enable cursor blink if true
 */
void LCD_SetCursorBlink(LCD *self, bool blinkEnabled);

/***************************************************************************//**
 * @brief Move the cursor
 * 
 * The number of rows and columns begin at 1, not 0. This is because the 
 * datasheets will number the rows and columns this way. Setting the cursor
 * position usually means performing a write command to set the DDRAM address. 
 * As a general rule, the cursor should never leave the screen. Limit the value
 * of the row or column to be between 1 and the number of rows or columns.
 * 
 * @param self  pointer to the LCD that you are using
 * 
 * @param row  row number from 1 to numRows
 * 
 * @param col  column number from 1 to numCols
 */
void LCD_MoveCursor(LCD *self, uint8_t row, uint8_t col);

/***************************************************************************//**
 * @brief Move the cursor forward once
 * 
 * Movement goes from left to right, top to bottom. Limit the value of the row 
 * or column to be between 1 and the number of rows or columns. When at the end
 * of a row, go the beginning of the next row. At the end of the screen, if 
 * LCD_OVERFLOW_WRAP_AROUND is set, go back to position (1,1). If 
 * LCD_OVERFLOW_SCROLL_DOWN is set, move whatever is on the display up one row
 * and begin a new row with the cursor at (1,numCols).
 * 
 * @param self  pointer to the LCD that you are using
 */
void LCD_MoveCursorForward(LCD *self);

/***************************************************************************//**
 * @brief Move the cursor backwards once
 * 
 * Movement goes from right to left, bottom to top. Limit the value of the row 
 * or column to be between 1 and the number of rows or columns. When at the
 * beginning of a row, go the end of the next row. At the beginning of the 
 * screen, if LCD_OVERFLOW_WRAP_AROUND is set, go back to position 
 * (numRows,numCols). 
 * 
 * @param self  pointer to the LCD that you are using
 */
void LCD_MoveCursorBackward(LCD *self);

/***************************************************************************//**
 * @brief Return the current position of the cursor
 * 
 * @param self  pointer to the LCD that you are using
 * 
 * @param retRow  pointer to return the row number (1 to numRows)
 * 
 * @param retCol  pointer to return the col number (1 to numCols)
 */
void LCD_GetCursorPosition(LCD *self, uint8_t *retRow, uint8_t *retCol);

/***************************************************************************//**
 * @brief Place a character on the screen at the current cursor position
 * 
 * The cursor should automatically move to the next position after placing the
 * character.
 * 
 * // TODO options for character wrap
 * 
 * @param self  pointer to the LCD that you are using
 * 
 * @param character  the character you want to display
 */
void LCD_PutChar(LCD *self, uint8_t character);

/***************************************************************************//**
 * @brief  Write a string of characters at the current cursor position
 * 
 * // TODO options for stopping at end of screen
 * 
 * @param self  pointer to the LCD that you are using
 * 
 * @param ptrToString  pointer to a null terminated string
 */
void LCD_PutString(LCD *self, uint8_t *ptrToString);

/***************************************************************************//**
 * @brief  Write a string of characters beginning at column 1
 * 
 * Null terminators do not matter in this function. Keep writing characters 
 * until either size has been reached or we hit the end of the row then stop. 
 * If the line number given does not exist, write nothing.
 * 
 * @param self  pointer to the LCD that you are using
 * 
 * @param lineNum  the line number from 1 to numRows
 * 
 * @param array  pointer to the array of characters
 * 
 * @param size  the size of the array
 */
void LCD_WriteFullLine(LCD *self, uint8_t lineNum, uint8_t *array, uint8_t size);

/***************************************************************************//**
 * @brief Scroll the screen down once
 * 
 * Move all the characters displayed up one row and have a blank line at the 
 * bottom row.
 * 
 * @param self 
 */
void LCD_ScrollDown(LCD *self);

/***************************************************************************//**
 * @brief Scroll the screen up once
 * 
 * Move all the characters displayed down one row and have a blank line at the 
 * top row.
 * 
 * @param self 
 */
void LCD_ScrollUp(LCD *self);

// TODO
void LCD_ScrollLine(LCD *self, bool forward, uint8_t lineNum, uint8_t *array, uint8_t size);

void LCD_SetCGRAMAddress(LCD *self, uint8_t address);

#endif  /* ILCD_H */