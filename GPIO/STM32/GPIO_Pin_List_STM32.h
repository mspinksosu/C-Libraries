/**
 * External pin list (processor specific)
 *
 * Any file that includes this header can have access to these objects to 
 * use with the GPIO library. Only the GPIO type needs to be external. Each 
 * external variable will need a matching declaration and initialization. This 
 * will be done in your processor specific GPIO implementation. 
 * 
 * - Matt Spinks
 * 
 * */

#ifndef GPIO_PIN_LIST_H
#define GPIO_PIN_LIST_H

#include "GPIO_STM32G0.h"

// ----- Declare GPIO pins starting here. Declare as extern pointers -----------

extern GPIO pin1, pin2;

#endif	/* GPIO_PIN_LIST_H */
