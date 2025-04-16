#ifndef  STRUCT_H
#define  STRUCT_H

#include <Arduino.h>

struct Position {
  int x;
  int y;
};

struct Food {
  Position p;
  int couleur;
};

#endif