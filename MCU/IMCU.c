/***************************************************************************//**
 * @brief MCU Interface
 * 
 * @author Matthew Spinks
 * 
 * @date 10/14/22  Original creation
 * 
 * @file IMCU.c
 * 
 * @details
 *      An interface that will handle tasks such as sleep, shutdown, delay.
 * It will also have a very simple scheduler.
 * // TODO more details
 * 
 ******************************************************************************/

#include "IMCU.h"

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

static MCUTask *head = NULL;
static MCUTask *task;
static bool schedulerFlag = false;
static unsigned int taskCounter = 0;


// ***** Static Function Prototypes ********************************************


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Non-Interface Functions *********************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void MCU_AddTask(MCUTask *self, unsigned int period, void (*Function)(void))
{
    /* Make new task point to the head of the list */
    self->next = head;
    /* Move head to point to the new task */
    head = self;

    self->period = period;
    self->count = period;
}

// *****************************************************************************

void MCU_TaskLoop(void)
{
    if(!schedulerFlag)
        return;

    taskCounter++;
    task = head;

    while(task->next != NULL)
    {
        if(taskCounter == task->count && task->Function != NULL)
        {
            task->Function();
            task->count += task->period;
        }
        task = task->next;
    }
    
    schedulerFlag = false;
}

// *****************************************************************************

void MCU_TaskTick(void)
{
    schedulerFlag = true;
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/* Implement the functions listed in IMCU.h under "Interface Functions" in your 
own .c file. I usually name my .c file with an underscore prefix, such as 
MCU_STM32G0.c or MCU_PIC32MX.c. Don't forget to include IMCU.h. */

/*
 End of File
 */