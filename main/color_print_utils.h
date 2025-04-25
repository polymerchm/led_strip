/**
 * @file color_print_utils.h
 * 
 * tools for ascii formating output to a terminal
 * 
 * @author Steven Pollack
 * @brief 
 * @version 0.1
 * @date 2025-04-25
 * 
 * @copyright Copyright (c) 2025
 * 
 */

 /*
\e[<style>;<color>m

Styles

0	Reset or normal	
1	Bold 
2	Faint
3	Italic	
4	Underline

 Foreground

30: Black
31: Red
32: Green
33: Yellow
34: Blue
35: Magenta
36: Cyan
37: White

Background

40: Black
41: Red
42: Green
43: Yellow
44: Blue
45: Magenta
46: Cyan
47: White


*/

#define A_BLACK 0
#define A_RED 1
#define A_GREEN 2
#define A_YELLOW 3
#define A_BLUE 4
#define A_MAGENTA 5
#define A_CYAN 6
#define A_WHITE 7

#define A_NORMAL 0
#define A_BOLD 1
#define A_FAINT 2
#define A_ITALIC 3
#define A_UNDERLINE 4


/**
 * @brief return appropriate foreground code
 * 
 * @param val 
 * @return int 
 */

static int FG(int val);

/**
 * @brief return appropriate background code
 * 
 * @param val 
 * @return int 
 */
static int BG(int val); 

/**
 * @brief return an an ascii sequence  
 * 
 * @param style an integer between 0-4.   If -1, ignore
 * @param foreground an interger beween 30-37, if -1, ignore
 * @param background an integer between 40-47, if -1, ignore
 * @param out char string to return results to.
 */

void get_ansi_codes(int style, int foreground, int background, char *out);

/**
 * @brief print andi codes to stdout 
 * 
 * @param style an integer between 0-4.   If -1, ignore
 * @param foreground an interger beween 30-37, if -1, ignore
 * @param background an integer between 40-47, if -1, ignore

 */
 void printf_ansi_codes(int style, int foreground, int background);

 



#ifndef __COLOR_PRINT_UTILS_H__
#define __COLOR_PRINT_UTILS_H__


#endif /* __COLOR_PRINT_UTILS_H__ */
