
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
 #include <stdio.h>

 #ifndef M_PI
 #define M_PI		3.14159265358979323846
 #endif // !M_PI

float round_to_tenth(float num) {
    return roundf(num * 50) / 50;
}

double gaussian(double x, float mean, float std_dev) {

 // Function to calculate the Gaussian curve value (unnormalized)

    return round_to_tenth(exp(-pow(x -mean, 2) / (2 * pow(std_dev, 2)))); 
}
