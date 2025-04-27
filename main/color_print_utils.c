/**
 * @file color_print_utils.c
 * @author Steven Pollack (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-04-25
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

static int FG(int val) {
    return 30 + val;

}

static int BG(int val) {
    return 40 + val;
}

void get_ansi_codes(int style, int foreground, int background, char *out){
    sprintf(out,"\x1B[");
    if (style != -1) {
        sprintf(out + strlen(out), "%d;", style);
    }
    if (foreground != -1) {
        sprintf(out + strlen(out), "%d;", FG(foreground));
    }
    if (background != -1) {
        sprintf(out + strlen(out), "%d;", BG(background));
    }
    // remove trailing ";" is present
    if (out[strlen(out) - 1] == ';') {
        out[strlen(out) - 1] = '\0';
    }
    sprintf(out + strlen(out), "m");
}

void print_ansi_codes(int style, int foreground, int background){
    char buffer[12]; // hope this is big enough
    get_ansi_codes(style, foreground, background, buffer);
    printf(buffer);
}





