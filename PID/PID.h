/***************************************************************************//**
 * @brief PID Library Header File
 * 
 * @file PID.h
 * 
 * @author Matthew Spinks <https://github.com/mspinksosu>
 * 
 * @date 12/17/22  Original Creation
 * 
 * @details
 *      // TODO Work in progress. Ready to test
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
    float iReductionFactor;
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

void PID_Create(PID *self, float Kp, float Ki, float Kd, float min, float max);

void PID_AdjustSetPoint(PID *self, float setPoint);

float PID_Compute(PID *self, float processVariable);

float PID_GetOutput(PID *self);

float PID_AdjustConstants(PID *self, float Kp, float Ki, float Kd);

void PID_Enable(PID *self);

void PID_Disable(PID *self);

void PID_AdjustIReductionFactor(PID *self, float r);

#endif  /* PID_H */