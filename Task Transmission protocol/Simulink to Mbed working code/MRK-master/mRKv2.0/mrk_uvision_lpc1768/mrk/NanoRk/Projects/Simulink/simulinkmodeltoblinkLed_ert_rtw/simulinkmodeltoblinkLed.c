/*
 * File: simulinkmodeltoblinkLed.c
 *
 * Code generated for Simulink model 'simulinkmodeltoblinkLed'.
 *
 * Model version                  : 1.1
 * Simulink Coder version         : 8.5 (R2013b) 08-Aug-2013
 * C/C++ source code generated on : Fri Nov 08 20:41:42 2013
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: ARM Compatible->ARM Cortex
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#include "simulinkmodeltoblinkLed.h"
#include "simulinkmodeltoblinkLed_private.h"

/* Block states (auto storage) */
DW_simulinkmodeltoblinkLed_T simulinkmodeltoblinkLed_DW;

/* External outputs (root outports fed by signals with auto storage) */
ExtY_simulinkmodeltoblinkLed_T simulinkmodeltoblinkLed_Y;

/* Real-time model */
RT_MODEL_simulinkmodeltoblink_T simulinkmodeltoblinkLed_M_;
RT_MODEL_simulinkmodeltoblink_T *const simulinkmodeltoblinkLed_M =
  &simulinkmodeltoblinkLed_M_;

/* Model step function */
void simulinkmodeltoblinkLed_step(void)
{
  /* Outport: '<Root>/Out1' incorporates:
   *  DiscretePulseGenerator: '<Root>/Pulse Generator'
   */
  simulinkmodeltoblinkLed_Y.Out1 = (simulinkmodeltoblinkLed_DW.clockTickCounter <
    simulinkmodeltoblinkLed_P.PulseGenerator_Duty) &&
    (simulinkmodeltoblinkLed_DW.clockTickCounter >= 0) ?
    simulinkmodeltoblinkLed_P.PulseGenerator_Amp : 0.0;

  /* DiscretePulseGenerator: '<Root>/Pulse Generator' */
  if (simulinkmodeltoblinkLed_DW.clockTickCounter >=
      simulinkmodeltoblinkLed_P.PulseGenerator_Period - 1.0) {
    simulinkmodeltoblinkLed_DW.clockTickCounter = 0;
  } else {
    simulinkmodeltoblinkLed_DW.clockTickCounter++;
  }
}

/* Model initialize function */
void simulinkmodeltoblinkLed_initialize(void)
{
  /* Registration code */

  /* initialize error status */
  rtmSetErrorStatus(simulinkmodeltoblinkLed_M, (NULL));

  /* states (dwork) */
  (void) memset((void *)&simulinkmodeltoblinkLed_DW, 0,
                sizeof(DW_simulinkmodeltoblinkLed_T));

  /* external outputs */
  simulinkmodeltoblinkLed_Y.Out1 = 0.0;

  /* Start for DiscretePulseGenerator: '<Root>/Pulse Generator' */
  simulinkmodeltoblinkLed_DW.clockTickCounter = 0;
}

/* Model terminate function */
void simulinkmodeltoblinkLed_terminate(void)
{
  /* (no terminate code required) */
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
