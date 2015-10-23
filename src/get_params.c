/*
 * get_params.c
 *
 * Code generation for function 'get_params'
 *
 * C source code generated on: Tue Oct 20 19:40:12 2015
 *
 */

/* Include files */
#include "get_controller.h"
#include "get_params.h"

/* Function Definitions */
void get_params(b_struct_T *parameters)
{
  parameters->M = 0.85223;

  /* Mass of chassis (kg) */
  parameters->l = 0.1352;

  /* distance from axle to centre of mass of chassis (m) */
  parameters->m = 0.45678;

  /* Mass of wheels (kg) */
  parameters->r = 0.04;

  /* radius of wheels (m) */
  parameters->I = 0.0148;

  /* mass moment of inertia of chassis (kg.m^2) */
  parameters->J = 0.000634016274964375;

  /* mass moment of inertia of wheels (kg.m^2) */
  parameters->g = 9.82;

  /*  Acceleration due to gravity (m/s/s) */
}

/* End of code generation (get_params.c) */
