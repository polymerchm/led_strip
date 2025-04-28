
/**
 *
 * ran0.c
 *
 * Author: spollack
 * Date: 2025-04-11 21:41:06
 *
 * from Numerical Recipes in C, page 279
 * Park and Miller
 */

#include "ran0.h"

float ran0(long *idum)
{

    long k;
    float ans;

    *idum ^= MASK;
    k = (*idum) / IQ;
    *idum = IA * (*idum - k * IQ) - IR * k;
    if (*idum < 0)
        *idum += IM;
    ans = AM * (*idum);
    *idum ^= MASK;
    return ans;
}

float random_float_in_range(float a, float b, long *seed) {
    float max, min;
    if (a < b) {
        max = b;
        min = a;
    } else {
        max = a;
        min = b;
    }
   

   return (max - min)*ran0(seed) + min;
}

double random_double_in_range(double a, double b, long *seed)
{
    double max, min;
    if (a < b)
    {
        max = b;
        min = a;
    }
    else
    {
        max = a;
        min = b;
    }
    return (double)((max - min) * ran0(seed) + min);
}

int random_int_in_range(int a, int b, long *seed)
{
    int max, min;
    if (a < b)
    {
        max = b;
        min = a;
    }
    else
    {
        max = a;
        min = b;
    }
    return (int)((max - min) * ran0(seed)) + min;
}

int random_long_in_range(long a, long b, long *seed)
{
    int max, min;
    if (a < b)
    {
        max = b;
        min = a;
    }
    else
    {
        max = a;
        min = b;
    }
    return (long)((max - min) * ran0(seed)) + min;
}
uint8_t random_uint8_in_range(uint8_t a, uint8_t b, long *seed) 
{
    uint8_t max, min;
    if (a < b)
    {
        max = b;
        min = a;
    }
    else
    {
        max = a;
        min = b;
    }
    return (uint8_t)((max - min) * ran0(seed) + min);
}

uint16_t random_uint16_in_range(uint16_t a, uint16_t b, long *seed) 
{
    uint16_t max, min;
    if (a < b)
    {
        max = b;
        min = a;
    }
    else
    {
        max = a;
        min = b;
    }
    return (uint16_t)((max - min) * ran0(seed) + min);
}
uint32_t random_uint32_in_range(uint32_t a, uint32_t b, long *seed)
{ 
    uint32_t max, min;
    if (a < b)
    {
        max = b;
        min = a;
    }
    else
    {
        max = a;
        min = b;
    }
    return (uint32_t)((max - min) * ran0(seed) + min);
}