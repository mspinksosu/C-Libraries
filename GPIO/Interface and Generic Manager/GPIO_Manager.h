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
 *     I removed the initialization function that was in the GPIO library and 
 * put it here. Code in the GPIO library should only be concerned with pins. 
 * It shouldn't care how the user initializes all their pins. This gives more 
 * flexibility with how we decide to handle pin organization.
 * 
 * When using the GPIO interface functions, calls to the base functions are 
 * done with the GPIO type. Other functions that may need external access to 
 * the pins only need to use the base class object.
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
in your processor specific GPIO Manager implementation. */

// ----- Declare GPIO pins here. Declare as extern  ----------------------------

extern GPIO led1, led2;


// ***** Function Prototypes ***************************************************

void GPIO_Manager_InitAllPins(void);

// ***** Generic Pin Numbers ***************************************************

/* I've create some generic pin aliases for you to use if you don't want to
make a subclass to define a port. The upper nibble is the "port" A through N
(I don't use "O" because it looks like "0") and the lower nibble is the pin 
0 through 15. Not every processor is going to use 16 pins per port though. It 
is up to you to handle this in your processor's implementation correctly. */
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
    GPIO_PORTN
};

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
    GPIO_PORTN_PIN15,
};


#endif  /* GPIO_MANAGER_H */