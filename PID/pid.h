/**
  ******************************************************************************
  * @file    pid.h
  * @author  Jackie Chen
  * @version 1.0.0
  * @date    8-April-2019
  * @brief   Simplified PID.
  ******************************************************************************/

#ifndef __PID_H__
#define	__PID_H__

#include "libcfg.h"

typedef struct __PidGains_s {

	float p;
	float i;
	float d;
    float integralMaxABS;
} PidGains_t;

typedef struct __PidState_s {

	float error;
	float integral;
	float derivative;
} PidState_t;

float PID_CalcResponse (PidState_t*, PidGains_t*, float, float, float);
void PID_Reset(PidState_t* state);

#endif /* !__PID_H__ */