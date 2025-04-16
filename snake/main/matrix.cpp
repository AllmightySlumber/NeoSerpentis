#include <DFRobot_RGBMatrix.h>

#define OE   9
#define LAT  10
#define CLK  11
#define A    A0
#define B    A1
#define C    A2
#define D    A3
#define E    A4
#define WIDTH 64
#define _HIGH 64
#define JOYSTICK_Y A7
#define JOYSTICK_X A6

DFRobot_RGBMatrix matrix(A, B, C, D, E, CLK, LAT, OE, false, WIDTH, _HIGH);