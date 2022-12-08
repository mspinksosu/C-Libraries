/***************************************************************************//**
 * @brief Error Code Library Header File
 * 
 * @author Matthew Spinks
 * 
 * @date 11/20/22  Original creation
 * 
 * @file ErrorCode.h
 * 
 * @details
 *      Manages up to 32 error codes and flashes an LED for you. The LED can be 
 * updated by calling ErrorCode_GetBlinkOutput(). When you initialize this
 * library, you must give it a tick rate in milliseconds. This is the rate at 
 * which you plan to call the Tick() function. Every tick, the library will
 * go through the list of error codes and set the LED output accordingly.
 * 
 * The error codes begin from 1 and go to 32. An error code of zero is
 * equivalent to having no errors. By default, the library will show each error 
 * code that is set, one by one. Once it finishes blinking an error code, it 
 * will insert a brief pause, then begin the next one. Calling the 
 * GetCurrentErrorCode function will tell you which one is being flashed at
 * that moment.
 * 
 * Calling GetActiveMask will give a 32-bit word of all the errors that are
 * currently set. The error code state machine is normally always running, even 
 * if there are no error codes. Do not use the IsRunning function to see if 
 * there are any error codes. Rather, get the active mask and check if it is 
 * zero instead. The reason I have the Start, Stop, and IsRunning functions is 
 * so that you can pause the state machine briefly if you need to take control 
 * of the LED pin. When the error code state machine is restarted, it will 
 * start over on the current error code.
 * 
 ******************************************************************************/

#ifndef ERROR_CODE_H
#define ERROR_CODE_H

#include <stdint.h>
#include <stdbool.h>

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Function Prototypes *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/***************************************************************************//**
 * @brief Initialize the library
 * 
 * Sets the expected tick rate and the LED pattern. The tick rate you specify
 * should preferrably be between 1 and 25 ms for the most accurate results. It
 * can be higher though if needed. The LED times will be truncated a little.
 * 
 * @param tickMs  how often you plan to call the ErrorCode Tick function
 */
void ErrorCode_InitMs(uint16_t tickMs);

/***************************************************************************//**
 * @brief Set an error code
 * 
 * @param code  error code 1 to 32
 */
void ErrorCode_Set(uint8_t code);

/***************************************************************************//**
 * @brief Clear an error code
 * 
 * @param code  error code 1 to 32
 */
void ErrorCode_Clear(uint8_t code);

/***************************************************************************//**
 * @brief Clear all error codes
 * 
 */
void ErrorCode_ClearAll(void);

/***************************************************************************//**
 * @brief Check if an error code has been set
 * 
 * @param code  error code 1 to 32
 * 
 * @return true  error code set
 */
bool ErrorCode_IsSet(uint8_t code);

/***************************************************************************//**
 * @brief Updates the state machine and LED output
 * 
 * The LED will flash to show the number of the error code it is currently on. 
 * When it is finished, it will insert a small pause to make it easier for the
 * customer to see where one error code stops and the next begins. Then it will
 * look for the next active error code and begin flashing the LED for that one.
 */
void ErrorCode_Tick(void);

/***************************************************************************//**
 * @brief Get the error code currently being flashed
 * 
 * @return uint8_t  the current error code (1 to 32)
 */
uint8_t ErrorCode_GetCurrentCode(void);

/***************************************************************************//**
 * @brief Set the output of an LED
 * 
 * @return true  set the output of your LED on. Else, set it off
 */
bool ErrorCode_GetBlinkOutput(void);

/***************************************************************************//**
 * @brief Stop the output
 * 
 * Does not change the current error code or clear any error codes. Behavior 
 * can be changed to stop immediately, stop on next off state, or stop when
 * finished.
 */
void ErrorCode_Stop(void);

/***************************************************************************//**
 * @brief Start the output
 * 
 * Restarts at the beginning of the current error code
 */
void ErrorCode_Start(void);

/***************************************************************************//**
 * @brief Check if the state machine is running
 * 
 * @return true  if running
 */
bool ErrorCode_IsRunning(void);

/***************************************************************************//**
 * @brief Get error code mask
 * 
 * Returns all the error codes in a 32-bit word. Remember that the bits are
 * numbered 0 through 31, but the error codes are 1 through 32
 * 
 * @return uint32_t  a bit mask. 1 = error code set, 0 = error code cleared
 */
uint32_t ErrorCode_GetActiveMask(void);

/***************************************************************************//**
 * @brief Change behavior for flashing the top n number of codes
 * 
 * The default behavior is to go through every error code and flash each one
 * with a pause in between them. By setting this value less than 32, you can
 * control how many error codes are flashed. The priority of error codes is 
 * from smallest to largest. 
 * 
 * For example, if you only want to blink one error code to the customer set 
 * this value to 1. If error codes 3 and 5 are both active, then only error 
 * code 3 gets flashed.
 * 
 * A value of 0 is not allowed. If you set it to zero, it will be set to 1.
 * A value or 32 or greater will enable all.
 * 
 * @param displayNumErrorCodes  flash up to 1 to 32 codes. (32 is default) 
 */
void ErrorCode_SetDisplayTopNumOfCodes(uint8_t displayNumErrorCodes);

#endif /* ERROR_CODE_H */