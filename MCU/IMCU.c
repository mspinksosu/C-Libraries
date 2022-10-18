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
#include <stddef.h>

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

static MCUTask *head = NULL;
static bool schedulerFlag = false;
static unsigned int taskCounter = 0;
static MCUTask EmptyTask = {.priority = 255, // impossible low priority level
                            .pending = false,
                            .Function = NULL};

// ***** Static Function Prototypes ********************************************

static MCUTask* GetNextTask(void);

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Non-Interface Functions *********************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void MCU_AddTask(MCUTask *self, unsigned int period, uint8_t priority, void (*Function)(void))
{
    /* Make the new task point to the current head of the list */
    self->next = head;
    /* Move the head to point to the new task */
    head = self;

    if(period == 0)
        period = 1;

    if(priority > 127)
        priority = 127;
    
    self->period = period;
    self->count = period;
    self->priority = priority;
}

// *****************************************************************************

void MCU_TaskLoop(void)
{
    static MCUTask *task;

    /* GetNextTask will return an empty task if there are no tasks pending */
    task = GetNextTask();

    if(task != &EmptyTask && task->Function != NULL)
    {
        task->Function();
        task->pending = false;
        task->count = task->period;
    }
}

// *****************************************************************************

void MCU_TaskTick(void)
{
    static MCUTask *task;
    task = head;

    while(task != NULL)
    {
        if(task->count > 0)
        {
            task->count--;
            if(task->count == 0)
            {
                task->pending = true;
            }
        }
        task = task->next;
    }
}

// *****************************************************************************

static MCUTask* GetNextTask(void)
{
    static MCUTask *task;
    static MCUTask *retTask;
    task = head;
    retTask = &EmptyTask;

    /* Go through the list and look for pending tasks. If there is more than
    one task pending, highest (lowest number) priority takes it. If two tasks
    have equal priority, first come, first serve. */
    while(task != NULL)
    {
        if(task->pending == true)
        {
            if(task->priority < retTask->priority)
            {
                retTask = task;
            }
        }
        task = task->next;
    }
    return retTask;
}

// *****************************************************************************

void MCU_Delay(uint32_t count)
{
    while(count--);
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