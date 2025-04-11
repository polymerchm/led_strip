
/**
 * 
 * gaussian.c
 * 
 * return the y value for a gaussian centered 
 * 
 * Author: spollack
 * Date: 2025-04-09 18:50:45
 */

 #include <math.h>
 #include <stdlib.h>

 #ifndef M_PI
 #define M_PI		3.14159265358979323846
 #endif // !M_PI


double gaussian(double x, float mean, float std_dev) {
    // Function to calculate the Gaussian curve value
    double exponent = -0.5 * pow((x - mean) / std_dev, 2);
    double coefficient = 1.0 / (std_dev * sqrt(2 * M_PI));
    return coefficient * exp(exponent);
}
