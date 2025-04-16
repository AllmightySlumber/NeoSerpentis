#ifndef FOOD_H
#define FOOD_H

#include <Arduino.h>
#include <DFRobot_RGBMatrix.h>
#include "struct.h"


void generateFood(Food foodStock[], Position snake[], int couleur);
void showFood(DFRobot_RGBMatrix& matrix, Food foodStock[]);
uint16_t Wheel(byte WheelPos);

#endif
