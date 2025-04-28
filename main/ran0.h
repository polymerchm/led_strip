/**
 *
 * ran0.h
 *
 * Author: spollack
 * Date: 2025-04-11 21:41:27
 */

#include <stdint.h>

/**
 * @brief a "reliable" random number gnerator from
 *            "Numerical Recipes in C".
 *
 * @param idum: a seed value.   Should not be changed between calls
 * @return float  a float between 0 and 1.0
 */

#define IA 16807
#define IM 2147483647
#define AM (1.0 / IM)
#define IQ 127773
#define IR 2836
#define MASK 123459876
float ran0(long *idum);

/**
 * @brief a set of random range values in various ouutput types
 *
 * @param a : min (or max)
 * @param b : max (or min)
 * @param seed : pointer to the seed value
 * @return float:  a number between a and b
 */
float random_float_in_range(float a, float b, long *seed);
double random_double_in_range(double a, double b, long *seed);
int random_int_in_range(int a, int b, long *seed);
int random_long_in_range(long a, long b, long *seed);
uint8_t random_uint8_in_range(uint8_t a, uint8_t b, long *seed);
uint16_t random_uint16_in_range(uint16_t a, uint16_t b, long *seed);
uint32_t random_uint32_in_range(uint32_t a, uint32_t b, long *seed);

/**
 * @brief macro to generate required version of the function
 * NOTE: there is no checking to ensure both are of the same type
 *
 */
#define random_in_range(a, b, c) _Generic((a), \
    int: random_int_in_range,                  \
    float: random_float_in_range,              \
    double: random_double_in_range,            \
    uint8_t: random_uint8_in_range,            \
    uint16_t: random_uint16_in_range,          \
    uint32_t: random_uint32_in_range)(a, b, c)
