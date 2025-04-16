#include "snake.h"

void initSnake(Position snake[]) {
  snake[0] = {0, 2};
  snake[1] = {0, 1};
  snake[2] = {0, 0};
}

void drawSnake(DFRobot_RGBMatrix& matrix, Position snake[]) {
  int snakeSize = sizeof(snake) / sizeof(snake[0]);
  for (int i = snakeSize - 1; i >= 0; i--) {
    matrix.drawPixel(snake[i].x, snake[i].y, Wheel(3));
  }
}

void moveSnake(Position snake[], String& axis, int& sens, String& lastAxis, int& lastDir) {
  int snakeSize = sizeof(snake) / sizeof(snake[0]);
  int dx = 0, dy = 0;

  if (axis == "horizontale") dx = sens;
  else if (axis == "verticale") dy = sens;

  for (int i = snakeSize - 1; i > 0; i--) {
    snake[i] = snake[i - 1];
  }

  snake[0].x += dx;
  snake[0].y += dy;

  if (snake[0].x == 64 && axis == "horizontale") snake[0].x = 0;
  if (snake[0].x == -1 && axis == "horizontale") snake[0].x = 63;
  if (snake[0].y == -1 && axis == "verticale") snake[0].y = 63;
  if (snake[0].y == 64 && axis == "verticale") snake[0].y = 0;
}

void changeMov(int JOYSTICK_X, int JOYSTICK_Y, String& axis, int& sens, String& lastAxis, int& lastDir) {
  int x = analogRead(JOYSTICK_X);
  int y = analogRead(JOYSTICK_Y);

  int zoneMorteMin = 400;
  int zoneMorteMax = 700;

  if (x > zoneMorteMax && lastAxis != "horizontale") {
    axis = "horizontale";
    sens = 1;
    lastAxis = "horizontale";
    lastDir = sens;
  } else if (x < zoneMorteMin && lastAxis != "horizontale") {
    axis = "horizontale";
    sens = -1;
    lastAxis = "horizontale";
    lastDir = sens;
  } else if (y > zoneMorteMax && lastAxis != "verticale") {
    axis = "verticale";
    sens = -1;
    lastAxis = "verticale";
    lastDir = sens;
  } else if (y < zoneMorteMin && lastAxis != "verticale") {
    axis = "verticale";
    sens = 1;
    lastAxis = "verticale";
    lastDir = sens;
  }
}
