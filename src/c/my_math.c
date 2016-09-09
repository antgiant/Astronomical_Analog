/*
 * loosely based on 
 * - http://stackoverflow.com/questions/11261170/c-and-maths-fast-approximation-of-a-trigonometric-function
 * - http://www.codeproject.com/Articles/69941/Best-Square-Root-Method-Algorithm-Function-Precisi
 */
#include "my_math.h"

#define SQRT_MAGIC_F 0x5f3759df 
float my_sqrt(const float x)
{
  const float xhalf = 0.5f*x;
 
  union // get bits for floating value
  {
    float x;
    int i;
  } u;
  u.x = x;
  u.i = SQRT_MAGIC_F - (u.i >> 1);  // gives initial guess y0
  return x*u.x*(1.5f - xhalf*u.x*u.x);// Newton step, repeating increases accuracy 
}   


float my_fabs(float x)
{
  if (x<0) return -x;
  return x;
}

/* minimax approximation to arcsin on [0, 0.5625] with rel. err. ~= 1.5e-11 */
float asin_core (float x)
{
  float x8, x4, x2;
  x2 = x * x;
  x4 = x2 * x2;
  x8 = x4 * x4;
  /* evaluate polynomial using a mix of Estrin's and Horner's scheme */
  return (((4.5334220547132049e-2 * x2 - 1.1226216762576600e-2) * x4 +
           (2.6334281471361822e-2 * x2 + 2.0596336163223834e-2)) * x8 +
          (3.0582043602875735e-2 * x2 + 4.4630538556294605e-2) * x4 +
          (7.5000364034134126e-2 * x2 + 1.6666666300567365e-1)) * x2 * x + x; 
}

/* relative error < 2e-11 on [-1, 1] */
float my_acos (float x)
{
  float xa, t;
  xa = my_fabs (x);
  /* arcsin(x) = pi/2 - 2 * arcsin (sqrt ((1-x) / 2)) 
   * arccos(x) = pi/2 - arcsin(x)
   * arccos(x) = 2 * arcsin (sqrt ((1-x) / 2))
   */
  if (xa > 0.5625) {
    t = 2.0 * asin_core (my_sqrt (0.5 * (1.0 - xa)));
  } else {
    t = 1.5707963267948966 - asin_core (xa);
  }
  /* arccos (-x) = pi - arccos(x) */
  return (x < 0.0) ? (3.1415926535897932 - t) : t;
}

float my_asin (float x)
{
  return (M_PI/2) - my_acos(x);
}
