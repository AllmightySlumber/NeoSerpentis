#ifndef SNAKE_H
#define SNAKE_H

#include <Arduino.h>
#include "food.h"
#include "struct.h"



void initSnake(Position snake[]);
void drawSnake(DFRobot_RGBMatrix& matrix, Position snake[]);
void moveSnake(Position snake[], String& axis, int& sens, String& lastAxis, int& lastDir);
void changeMov(int JOYSTICK_X, int JOYSTICK_Y, String& axis, int& sens, String& lastAxis, int& lastDir);

#endif
