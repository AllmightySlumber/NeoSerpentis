#include "food.h"


void generateFood(Food foodStock[], Position snake[], int couleur) {
  bool isFood = false;
  int food_x;
  int food_y;
  Food food;

  while (!isFood) {
    food_x = random(0, 65);
    food_y = random(0, 65);
    isFood = true;

    int snakeSize = sizeof(snake) / sizeof(snake[0]);
    for (int i = 0; i < snakeSize; i++) {
      if (food_x == snake[i].x && food_y == snake[i].y) {
        isFood = false;
      }
    }
  }

  food.p.x = food_x;
  food.p.y = food_y;
  food.couleur = couleur;
  foodStock[0] = food;
}

void showFood(DFRobot_RGBMatrix& matrix, Food foodStock[]) {
  int stockSize = sizeof(foodStock) / sizeof(foodStock[0]);
  for (int i = 0; i < stockSize; i++) {
    matrix.drawPixel(foodStock[i].p.x, foodStock[i].p.y, Wheel(foodStock[i].couleur));
  }
}

uint16_t Wheel(byte WheelPos) {
  if (WheelPos < 8) {
    return matrix.Color333(7 - WheelPos, WheelPos, 0);
  } else if (WheelPos < 16) {
    WheelPos -= 8;
    return matrix.Color333(0, 7 - WheelPos, WheelPos);
  } else {
    WheelPos -= 16;
    return matrix.Color333(0, WheelPos, 7 - WheelPos);
  }
}
