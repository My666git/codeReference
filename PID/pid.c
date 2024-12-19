/**
  ******************************************************************************
  * @file    pid.c
  * @author  Jackie Chen
  * @version 1.0.0
  * @date    8-April-2019
  * @brief   Simplified PID.
  ******************************************************************************/

#include "pid.h"

void PID_Reset(PidState_t* state)
{
    state->derivative = 0.0f;
    state->error = 0.0f;
    state->integral = 0.0f;
}

float PID_CalcResponse (PidState_t* state, PidGains_t *gains, float setPoint, float point, float dt) {
	float currentError;

	currentError = setPoint - point;

    state->integral += (currentError * dt);
//    /* Limit total integral */
//    if( state->integral > gains->integralMaxABS ){
//        state->integral = gains->integralMaxABS;
//    }
//    else if( state->integral < -gains->integralMaxABS ){
//        state->integral = -gains->integralMaxABS;
//    }

	state->derivative = (currentError - state->error) / dt;

	state->error = currentError;

	return (gains->p * state->error) + (gains->i * state->integral) + (gains->d * state->derivative);
}

#if 0
#define                 S_CURVE_VAR_MAX     100.0f
float EasyMotion(float Initial, float Target, u32 time)
{
    float   sfSCurveVar = S_CURVE_VAR_MAX;
    float   sfSurveResult = 0.0f;
    float   sfSCurveVarStep = 0.0f;

    /* Initial S_CURVE input var step */
    sfSCurveVarStep = S_CURVE_VAR_MAX * (float)(gSYSConfig_Run.ModelCfg.SysDevCfg.SysDevCfg.MotionProcTimeX5MS * NC_DEV_POLLING_INTERVAL) / (float)time;

    sfSCurveVar += sfSCurveVarStep;

    sfSurveResult = 1.0f / ( 1.0f + expf(NC_NAV_CTRL_S_CURVE_BOUND - NC_NAV_CTRL_S_CURVE_STEP * sfSCurveVar) );

    return sfSurveResult;
}
#endif
