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
    void (*LCD_Init)(void *instance, void *params, uint8_t tickUs);
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
    void (*TransmitByte)(uint8_t byte);
    uint8_t (*ReceiveByte)(void);
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

void LCD_SetTransmitByteFunc(LCD *self, void (*Function)(uint8_t data));

void LCD_SetReceiveByteFunc(LCD *self, uint8_t (*Function)(void));

void LCD_PutInt(LCD *self, int16_t num, uint8_t width);

void LCD_PutFloat(LCD *self, float num, uint8_t precision);

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/***************************************************************************//**
 * @brief Initialize the LCD and the tick rate in us
 * 
 * The tick rate should preferably be greater than 100 us. The write functions
 * will each have a small delay of about 1 us. But the busy function could 
 * delay up to 100 us. The maximum amount of delay should 100 us. Once the 
 * screen is written to, there is no need to continuously update it. The 
 * controller in the LCD will hold the characters being displayed. I think a 
 * 1 or 2 millisecond (1000 - 2000 us) tick rate works best.
 * 
 * @param self  pointer to the LCD that you are using
 * 
 * @param params  pointer to the LCDInitType that you are using
 * 
 * @param tickUs  the tick rate in microseconds
 */
void LCD_Init(LCD *self, LCDInitType *params, uint8_t tickUs);

/***************************************************************************//**
 * @brief Update the characters on the display
 * 
 * The maximum amount of time spent in this function should preferably be 
 * 200 us or less. Typically, you will call the IsBusy function to generate a 
 * read. Then afterwards a write. Then exit, and come back and write the next 
 * character to be displayed on the next function call. The LCD read and write
 * commands will have a slight delay of 1 to 2 us. This is to allow time for 
 * the E,RS,RW pins. In reality, this time will be longer though, because it 
 * takes a little extra time to set the pins themselves.
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