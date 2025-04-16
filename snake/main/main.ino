#include <DFRobot_RGBMatrix.h>
#include "snake.h"
#include "food.h"

Position snake[3] = {};
Food foodStock[1] = {};

unsigned long greenFoodTime = 0;
unsigned long redFoodTime = 0;
unsigned long blueFoodTime = 0;
unsigned long foodInterval = 10000;

unsigned long mouvementTime = 0;
unsigned long mouvementInterval = 100;

String axis = "horizontale";
int sens = 1;
int lastDir = sens;
String lastAxis = axis;

void setup() {
  matrix.begin();
  Serial.begin(9600);

  initSnake(snake);
}

void loop() {
  matrix.fillScreen(matrix.Color333(0, 0, 0));
  changeMov(JOYSTICK_X, JOYSTICK_Y, axis, sens, lastAxis, lastDir);
  moveSnake(snake, axis, sens, lastAxis, lastDir);
  drawSnake(matrix, snake);

  if (millis() - blueFoodTime > foodInterval) {
    generateFood(foodStock, snake, 3);
    blueFoodTime = millis();
  }

  if (millis() - redFoodTime > (foodInterval * 1.5)) {
    generateFood(foodStock, snake, 3);
    redFoodTime = millis();
  }

  if (millis() - greenFoodTime > (foodInterval * 2)) {
    generateFood(foodStock, snake, 3);
    greenFoodTime = millis();
  }

  showFood(matrix, foodStock);
  delay(200);
}
