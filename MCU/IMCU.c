/***************************************************************************//**
 * @brief MCU Interface
 * 
 * @file IMCU.c
 * 
 * @author Matthew Spinks <https://github.com/mspinksosu>
 * 
 * @date 10/14/22  Original creation
 * 
 * @details
 *      An interface that will handle tasks such as sleep, shutdown, delay.
 * It will also have a very simple scheduler.
 * // TODO more details
 * 
 * @section license License
 * SPDX-FileCopyrightText: © 2022 Matthew Spinks
 * SPDX-License-Identifier: Zlib
 * 
 * This software is released under the Zlib license. You are free alter and
 * redistribute it, but you must not misrepresent the origin of the software.
 * This notice may not be removed. <http://www.zlib.net/zlib_license.html>
 * 
 ******************************************************************************/

#include "IMCU.h"
#include <stddef.h>

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

static MCUTask *taskList = NULL;    // the head of the task list
static MCUTask *currentTask = NULL; // the head of the pending task list

// ***** Static Function Prototypes ********************************************

static void FindTasks(void);
static void AddToPending(MCUTask *newTask);

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Non-Interface Functions *********************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void MCU_AddTask(MCUTask *self, uint16_t period, uint8_t priority, void (*Function)(void))
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
    FindTasks();

    if(currentTask != NULL)
    {
        if(currentTask->Function != NULL)
            currentTask->Function();
        
        currentTask->pending = false;
        currentTask->count = currentTask->period;
        currentTask = currentTask->nextPending;
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
                task->addToPending = true;
            }
        }
        task = task->next;
    }
}

// *****************************************************************************

void MCU_Delay(uint32_t count)
{
    while(count--);
}

// *****************************************************************************

bool MCU_IsLittleEndian(void)
{
    uint16_t x = 1;
    
    return *((uint8_t *)&x);
}

// *****************************************************************************

void *MCU_ReverseMemcpy(void *dst, const void *src, uint16_t n)
{
    uint8_t *pdst = (uint8_t*)dst;
    const uint8_t *psrc = (const uint8_t*)src;
    pdst += n - 1;

    while(n--)
    {
        *pdst-- = *psrc++;
    }
    return dst;
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Local Functions *****************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/***************************************************************************//**
 * @brief Find pending tasks
 * 
 * Go through the lists of tasks and look for pending tasks and add them to the
 * pending task list. If two tasks have equal priority then it will be first
 * come, first serve. A task cannot be added to the pending list if it is
 * already in it.
 */
static void FindTasks(void)
{
    static MCUTask *task;
    task = taskList;

    while(task != NULL)
    {
        if(task->addToPending == true && task->pending == false)
        {
            AddToPending(task);
            task->addToPending = false;
        }
        task = task->next;
    }
}

/***************************************************************************//**
 * @brief Add a task to the pending list
 * 
 * Go through the list of pending tasks and find where to insert the new task 
 * based on priority. The currentTask pointer will always point to the task to 
 * be executed. I've made a pending list within the list of tasks and made it 
 * so that no task can jump in front of the current task. Except if the pending
 * task list is empty. Then the new task becomes the head of the list.
 * 
 * @param newTask  the pending task to be added
 */
static void AddToPending(MCUTask *newTask)
{
    static MCUTask *task;
    task = currentTask;

    if(task == NULL)
    {
        /* Make the new task point to the current head of the list */
        newTask->nextPending = currentTask;
        /* Move the head to point to the new task */
        currentTask = newTask;
    }
    else
    {
        MCUTask *tmp;
        while(task->nextPending != NULL)
        {
            tmp = task->nextPending;

            if(newTask->priority < tmp->priority)
                break;

            task = tmp;
        }
        /* Insert after task. Make new task point to previous->nextPending */
        newTask->nextPending = task->nextPending;
        /* Make previous pending task point to our new task */
        task->nextPending = newTask;
    }
    newTask->pending = true;
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