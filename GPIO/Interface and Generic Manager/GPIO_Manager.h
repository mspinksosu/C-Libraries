/***************************************************************************//**
 * @brief GPIO Library Manager Header (Non-Processor Specific)
 * 
 * @author Matthew Spinks
 * 
 * @date 7/24/22   Original creation
 * 
 * @file GPIO_Manager.h
 * 
 * @details
 *     This is a simple piece of code that handles initializing all of the pins 
 * on the MCU. It also handles access to the GPIO pins for other files.
 * 
 * The code in the GPIO library should really only deal with modifying the 
 * pins. It should not be concerned with how the user manages their pin 
 * organization. Furthermore, The GPIO implementation will always have some 
 * sort of processor specific dependency. So if the user were to include the
 * header for that file they wouldn't be able to port their code as easily to 
 * a different processor.
 * 
 * Calls to the GPIO functions are done with the base GPIO type. Because of 
 * this, functions that may need access to the pins only need to use the base 
 * class object. By putting the external declaration here, any file that needs 
 * to use the pins can include this header. Now we don't need to deal with any 
 * other files needing to include processor specific headers.
 * 
 * Each external base GPIO object will need a matching declaration in the 
 * GPIO_Manager.c file, or in your own processor specific GPIO_Manager.c file 
 * if you choose to make one.
 * 
 * The first step in the initialization process is to set the GPIO driver
 * interface or function table by calling GPIO_SetDriverInterface. This table 
 * is created inside a processor specific GPIO library, which is library that 
 * implements the functions listed in IGPIO.h. After that, each pins properties
 * can be set and the GPIO_Init function can be called once for every pin that 
 * is needed.
 * 
 * I've also included some generic pin aliases below. If you'd rather not make
 * a processor specific subclass, you can use these definitions if you like. 
 * I've combined the port and pin together into a single byte, to be used as
 * the pinNumber variable in the base class (defined in GPIO.h). The upper 
 * nibble is the "port" A through N and the lower nibble is the pin 0 through 
 * 15. Not every processor is going to use 16 pins per port though. So, it is 
 * up to you to handle this in your processor's implementation.
 * 
 ******************************************************************************/

#ifndef GPIO_MANAGER_H
#define GPIO_MANAGER_H

#include "IGPIO.h"

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

/* Any file that includes this header can have access to these objects to use 
with the GPIO library. Only the GPIO type needs to be external. Each external 
variable will need a matching declaration and initialization. This will be done 
in your processor specific GPIO_Manager implementation. */

// ----- Declare GPIO pins here. Declare as extern  ----------------------------

extern GPIO led1, led2;


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Function Prototypes *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/***************************************************************************//**
 * @brief Set the GPIO driver interface and initialize all of the pins
 * 
 * Each pin has to be declared as a global variable. However, only the base
 * class GPIO needs to be extern. Anything that needs to access these pins
 * will include this header file which has the extern declaration.
 */
void GPIO_Manager_InitAllPins(void);

// ***** Generic Pin Numbers ***************************************************

/* Generic pin aliases. The upper nibble is the "port" A through N and the 
lower nibble is the pin 0 through 15. The port is also provided separately so 
you use the upper nibble to find your port. Example: 

led1Pin.pinNumber = GPIO_PORTB_PIN4; // 0x14
GPIO_MCU1_SetPin(&led1Pin); // set port "1", pin 4

GPIO_MCU1_SetPin(GPIO_MCU1 *self)
{
    portNibble = ((self->pinNumber) >> 4) & 0x0F;
    pinNibble = (self->pinNumber) & 0x0F;

    switch(portNibble)
    {
        case GPIO_PORTA:
            MCU1_PortA_Reg |= (1 << pinNibble);
            break;
    }
}
*/
enum {
    GPIO_PORTA = 0,
    GPIO_PORTB,
    GPIO_PORTC,
    GPIO_PORTD,
    GPIO_PORTE,
    GPIO_PORTF,
    GPIO_PORTG,
    GPIO_PORTH,
    GPIO_PORTI,
    GPIO_PORTJ,
    GPIO_PORTK,
    GPIO_PORTL,
    GPIO_PORTM,
    GPIO_PORTN };

enum {
    GPIO_PORTA_PIN0 = 0,
    GPIO_PORTA_PIN1,
    GPIO_PORTA_PIN2,
    GPIO_PORTA_PIN3,
    GPIO_PORTA_PIN4,
    GPIO_PORTA_PIN5,
    GPIO_PORTA_PIN6,
    GPIO_PORTA_PIN7,
    GPIO_PORTA_PIN8,
    GPIO_PORTA_PIN9,
    GPIO_PORTA_PIN10,
    GPIO_PORTA_PIN11,
    GPIO_PORTA_PIN12,
    GPIO_PORTA_PIN13,
    GPIO_PORTA_PIN14,
    GPIO_PORTA_PIN15,
    GPIO_PORTB_PIN0,
    GPIO_PORTB_PIN1,
    GPIO_PORTB_PIN2,
    GPIO_PORTB_PIN3,
    GPIO_PORTB_PIN4,
    GPIO_PORTB_PIN5,
    GPIO_PORTB_PIN6,
    GPIO_PORTB_PIN7,
    GPIO_PORTB_PIN8,
    GPIO_PORTB_PIN9,
    GPIO_PORTB_PIN10,
    GPIO_PORTB_PIN11,
    GPIO_PORTB_PIN12,
    GPIO_PORTB_PIN13,
    GPIO_PORTB_PIN14,
    GPIO_PORTB_PIN15,
    GPIO_PORTC_PIN0,
    GPIO_PORTC_PIN1,
    GPIO_PORTC_PIN2,
    GPIO_PORTC_PIN3,
    GPIO_PORTC_PIN4,
    GPIO_PORTC_PIN5,
    GPIO_PORTC_PIN6,
    GPIO_PORTC_PIN7,
    GPIO_PORTC_PIN8,
    GPIO_PORTC_PIN9,
    GPIO_PORTC_PIN10,
    GPIO_PORTC_PIN11,
    GPIO_PORTC_PIN12,
    GPIO_PORTC_PIN13,
    GPIO_PORTC_PIN14,
    GPIO_PORTC_PIN15,
    GPIO_PORTD_PIN0,
    GPIO_PORTD_PIN1,
    GPIO_PORTD_PIN2,
    GPIO_PORTD_PIN3,
    GPIO_PORTD_PIN4,
    GPIO_PORTD_PIN5,
    GPIO_PORTD_PIN6,
    GPIO_PORTD_PIN7,
    GPIO_PORTD_PIN8,
    GPIO_PORTD_PIN9,
    GPIO_PORTD_PIN10,
    GPIO_PORTD_PIN11,
    GPIO_PORTD_PIN12,
    GPIO_PORTD_PIN13,
    GPIO_PORTD_PIN14,
    GPIO_PORTD_PIN15,
    GPIO_PORTE_PIN0,
    GPIO_PORTE_PIN1,
    GPIO_PORTE_PIN2,
    GPIO_PORTE_PIN3,
    GPIO_PORTE_PIN4,
    GPIO_PORTE_PIN5,
    GPIO_PORTE_PIN6,
    GPIO_PORTE_PIN7,
    GPIO_PORTE_PIN8,
    GPIO_PORTE_PIN9,
    GPIO_PORTE_PIN10,
    GPIO_PORTE_PIN11,
    GPIO_PORTE_PIN12,
    GPIO_PORTE_PIN13,
    GPIO_PORTE_PIN14,
    GPIO_PORTE_PIN15,
    GPIO_PORTF_PIN0,
    GPIO_PORTF_PIN1,
    GPIO_PORTF_PIN2,
    GPIO_PORTF_PIN3,
    GPIO_PORTF_PIN4,
    GPIO_PORTF_PIN5,
    GPIO_PORTF_PIN6,
    GPIO_PORTF_PIN7,
    GPIO_PORTF_PIN8,
    GPIO_PORTF_PIN9,
    GPIO_PORTF_PIN10,
    GPIO_PORTF_PIN11,
    GPIO_PORTF_PIN12,
    GPIO_PORTF_PIN13,
    GPIO_PORTF_PIN14,
    GPIO_PORTF_PIN15,
    GPIO_PORTG_PIN0,
    GPIO_PORTG_PIN1,
    GPIO_PORTG_PIN2,
    GPIO_PORTG_PIN3,
    GPIO_PORTG_PIN4,
    GPIO_PORTG_PIN5,
    GPIO_PORTG_PIN6,
    GPIO_PORTG_PIN7,
    GPIO_PORTG_PIN8,
    GPIO_PORTG_PIN9,
    GPIO_PORTG_PIN10,
    GPIO_PORTG_PIN11,
    GPIO_PORTG_PIN12,
    GPIO_PORTG_PIN13,
    GPIO_PORTG_PIN14,
    GPIO_PORTG_PIN15,
    GPIO_PORTH_PIN0,
    GPIO_PORTH_PIN1,
    GPIO_PORTH_PIN2,
    GPIO_PORTH_PIN3,
    GPIO_PORTH_PIN4,
    GPIO_PORTH_PIN5,
    GPIO_PORTH_PIN6,
    GPIO_PORTH_PIN7,
    GPIO_PORTH_PIN8,
    GPIO_PORTH_PIN9,
    GPIO_PORTH_PIN10,
    GPIO_PORTH_PIN11,
    GPIO_PORTH_PIN12,
    GPIO_PORTH_PIN13,
    GPIO_PORTH_PIN14,
    GPIO_PORTH_PIN15,
    GPIO_PORTI_PIN0,
    GPIO_PORTI_PIN1,
    GPIO_PORTI_PIN2,
    GPIO_PORTI_PIN3,
    GPIO_PORTI_PIN4,
    GPIO_PORTI_PIN5,
    GPIO_PORTI_PIN6,
    GPIO_PORTI_PIN7,
    GPIO_PORTI_PIN8,
    GPIO_PORTI_PIN9,
    GPIO_PORTI_PIN10,
    GPIO_PORTI_PIN11,
    GPIO_PORTI_PIN12,
    GPIO_PORTI_PIN13,
    GPIO_PORTI_PIN14,
    GPIO_PORTI_PIN15,
    GPIO_PORTJ_PIN0,
    GPIO_PORTJ_PIN1,
    GPIO_PORTJ_PIN2,
    GPIO_PORTJ_PIN3,
    GPIO_PORTJ_PIN4,
    GPIO_PORTJ_PIN5,
    GPIO_PORTJ_PIN6,
    GPIO_PORTJ_PIN7,
    GPIO_PORTJ_PIN8,
    GPIO_PORTJ_PIN9,
    GPIO_PORTJ_PIN10,
    GPIO_PORTJ_PIN11,
    GPIO_PORTJ_PIN12,
    GPIO_PORTJ_PIN13,
    GPIO_PORTJ_PIN14,
    GPIO_PORTJ_PIN15,
    GPIO_PORTK_PIN0,
    GPIO_PORTK_PIN1,
    GPIO_PORTK_PIN2,
    GPIO_PORTK_PIN3,
    GPIO_PORTK_PIN4,
    GPIO_PORTK_PIN5,
    GPIO_PORTK_PIN6,
    GPIO_PORTK_PIN7,
    GPIO_PORTK_PIN8,
    GPIO_PORTK_PIN9,
    GPIO_PORTK_PIN10,
    GPIO_PORTK_PIN11,
    GPIO_PORTK_PIN12,
    GPIO_PORTK_PIN13,
    GPIO_PORTK_PIN14,
    GPIO_PORTK_PIN15,
    GPIO_PORTL_PIN0,
    GPIO_PORTL_PIN1,
    GPIO_PORTL_PIN2,
    GPIO_PORTL_PIN3,
    GPIO_PORTL_PIN4,
    GPIO_PORTL_PIN5,
    GPIO_PORTL_PIN6,
    GPIO_PORTL_PIN7,
    GPIO_PORTL_PIN8,
    GPIO_PORTL_PIN9,
    GPIO_PORTL_PIN10,
    GPIO_PORTL_PIN11,
    GPIO_PORTL_PIN12,
    GPIO_PORTL_PIN13,
    GPIO_PORTL_PIN14,
    GPIO_PORTL_PIN15,
    GPIO_PORTM_PIN0,
    GPIO_PORTM_PIN1,
    GPIO_PORTM_PIN2,
    GPIO_PORTM_PIN3,
    GPIO_PORTM_PIN4,
    GPIO_PORTM_PIN5,
    GPIO_PORTM_PIN6,
    GPIO_PORTM_PIN7,
    GPIO_PORTM_PIN8,
    GPIO_PORTM_PIN9,
    GPIO_PORTM_PIN10,
    GPIO_PORTM_PIN11,
    GPIO_PORTM_PIN12,
    GPIO_PORTM_PIN13,
    GPIO_PORTM_PIN14,
    GPIO_PORTM_PIN15,
    GPIO_PORTN_PIN0,
    GPIO_PORTN_PIN1,
    GPIO_PORTN_PIN2,
    GPIO_PORTN_PIN3,
    GPIO_PORTN_PIN4,
    GPIO_PORTN_PIN5,
    GPIO_PORTN_PIN6,
    GPIO_PORTN_PIN7,
    GPIO_PORTN_PIN8,
    GPIO_PORTN_PIN9,
    GPIO_PORTN_PIN10,
    GPIO_PORTN_PIN11,
    GPIO_PORTN_PIN12,
    GPIO_PORTN_PIN13,
    GPIO_PORTN_PIN14,
    GPIO_PORTN_PIN15 };

#endif  /* GPIO_MANAGER_H */