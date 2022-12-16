/***************************************************************************//**
 * @brief PID C Library
 * 
 * @author Matthew Spinks
 * 
 * @date 12/17/22  Original Creation
 * 
 * @file PID.c
 * 
 * @details
 *      TODO
 * 
 ******************************************************************************/

#include "PID.h"

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************


// ***** Static Functions Prototypes *******************************************


// *****************************************************************************

void PID_Init(PID *self, float Kp, float Ki, float Kd, float min, float max)
{
    self->Kp = Kp;
    self->Ki = Ki;
    self->Kd = Kd;
    self->min = min;
    self->max = max;
    self->enable = false;
    /* do some other stuff here */
}

// *****************************************************************************

void PID_AdjustSetPoint(PID *self, float setPoint)
{
    self->setPoint = setPoint;
}

// *****************************************************************************

float PID_Compute(PID *self, float processVariable)
{
    if(self->enable == false)
        return;

    float error = self->setPoint - processVariable;
    self->integral += error;
    float derivative = error - self->prevError;

    /* TODO integral windup */

    /* TODO limit max and min */

    /* Compute the output. Output = P_Term + I_Term + D_Term */
    self->controlVariable = (self->Kp * error) + (self->Ki * self->integral) +
                            (self->Kd * derivative);
    
    /* Store previous error for the derivative term */
    self->prevError = error;

    return self->controlVariable;
}

// *****************************************************************************

float PID_GetOutput(PID *self)
{
    return self->controlVariable;
}

// *****************************************************************************

float PID_AdjustConstants(PID *self, float Kp, float Ki, float Kd)
{
    self->Kp = Kp;
    self->Ki = Ki;
    self->Kd = Kd;
}

// *****************************************************************************

void PID_Enable(PID *self)
{
    self->enable = true;
}

// *****************************************************************************

void PID_Disable(PID *self)
{
    self->enable = false;
}

/*
 End of File
 */