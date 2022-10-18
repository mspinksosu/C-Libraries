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

static MCUTask *taskList = NULL;
static MCUTask *pendingTasks = NULL;
static MCUTask *currentTask = NULL;
static bool schedulerFlag = false;
static unsigned int taskCounter = 0;
static MCUTask EmptyTask = {.priority = 255, // impossible low priority level
                            .pending = false,
                            .Function = NULL};

// ***** Static Function Prototypes ********************************************

static void GetTasks(void);
static void AddToPending(MCUTask *taskToAdd);

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Non-Interface Functions *********************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void MCU_AddTask(MCUTask *self, unsigned int period, uint8_t priority, void (*Function)(void))
{
    /* Make the new task point to the current head of the list */
    self->next = taskList;
    /* Move the head to point to the new task */
    taskList = self;

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
    //task = GetNextTask();

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
    task = taskList;

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

static void GetTasks(void)
{
    static MCUTask *task;
    static MCUTask *retTask;
    task = taskList;

    /* Look for pending tasks and add them to the list. If two tasks have equal 
    priority, then first come, first serve. */
    while(task != NULL)
    {
        if(task->pending == true)
        {
            AddToPending(task);
        }
        task = task->next;
    }
    return retTask;
}

// *****************************************************************************

static void AddToPending(MCUTask *taskToAdd)
{
    static MCUTask *task;
    task = pendingTasks;

    if(pendingTasks == NULL)
    {
        /* Make the new task point to the current head of the list */
        taskToAdd->nextPending = pendingTasks;
        /* Move the head to point to the new task */
        pendingTasks = taskToAdd;
    }
    else
    {
        while(task != NULL)
    }
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