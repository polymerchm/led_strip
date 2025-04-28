# Sparkle - a 20-NEOPIXEL LED "Firefly" Light Strand


## based on 

https://github.com/espressif/esp-idf/tree/master/examples/peripherals/rmt/led_strip


## Summary

This program will program a 20-pixel LED strand to randomly pulse sets of the LEDS with a gaussian intensity profile of a prescribed std-dev at varying start times.

The "pulses" are all contained within the total "repeat time" by preventing a median from being any closer to the start or end than 3 std_devs.

There are two tasks, one for building the sequences of intensities
on all of the LEDS and one to display the LEDS. The build task runs on CPU0 and the Display task runs on CPU1.   They coordinate using the xTaskNotify and xTaskNotifyWait functions.

Once a pattern is stored, it is displayed (using the RMT hardware) while the next pattern is created   When the new pattern is ready, the build task will signal the display task to use it.    The display task signals the build task to generate the next pattern in the a buffer when it is through with its buffer.

20 NEOPIXELS is about the limit for an ESP32 WROOM-32e (uses 75% of memory).


### Future

Other patterns triggered by a touch switch

## Bonus

There are "utilities" for producing  numbers in ranges of varying types using "generics" in the ran0.c file and tools for printing to the terminal with ANSI styles and colors in the color_print_utils.c files.   The latter is handy for flagging outputs from multiple tasks.
