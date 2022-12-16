/***************************************************************************//**
 * @brief PID Library Header File
 * 
 * @author Matthew Spinks
 * 
 * @date 12/17/22  Original Creation
 * 
 * @file PID.h
 * 
 * @details
 *      TODO
 * 
 ******************************************************************************/

#ifndef PID_H
#define PID_H

#include <stdint.h>
#include <stdbool.h>

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

/* Class specific variables */
typedef struct PIDTag
{
    float Kp;
    float Ki;
    float Kd;
    float min;
    float max;
    float setPoint;
    float controlVariable;
    float integral;
    float prevError;
    bool enable;
} PID;

/** 
 * Description of struct
 * 
 * member1      description of variable member1
 * 
 */

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Function Prototypes *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

void PID_Init(PID *self, float Kp, float Ki, float Kd, float min, float max);

void PID_AdjustSetPoint(PID *self, float setPoint);

float PID_Compute(PID *self, float processVariable);

float PID_GetOutput(PID *self);

float PID_AdjustConstants(PID *self, float Kp, float Ki, float Kd);

void PID_Enable(PID *self);

void PID_Disable(PID *self);

#endif  /* PID_H */