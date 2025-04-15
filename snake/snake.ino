/*!
 * @file testRGBMatrix.ino
 * @brief Run the routine to test the RGB LED Matrix Panel
 * @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license     The MIT License (MIT)
 * @author [TangJie]](jie.tang@dfrobot.com)
 * @version  V1.0.1
 * @date  2022-03-23
 * @url https://github.com/DFRobot/DFRobot_RGBMatrix
 */
 
#include <DFRobot_RGBMatrix.h> // Hardware-specific library

#define OE   	9
#define LAT 	10
#define CLK 	11
#define A   	A0
#define B   	A1
#define C   	A2
#define D   	A3
#define E   	A4
#define WIDTH 64
#define _HIGH	64
// Les entrées analogiques du joystick
#define JOYSTICK_Y A7
#define JOYSTICK_X A6


DFRobot_RGBMatrix matrix(A, B, C, D, E, CLK, LAT, OE, false, WIDTH, _HIGH);

// Structure pour créer des positions tels que (x,y) et pouvoir accéder à x et y.
struct Position{
  int x;
  int y;
};
struct Food{
  Position p;
  int couleur;
};

Position snake[3] = {}; // Tableau de position du serpent de base
Food foodStock[1] = {}; // Tableau de nourriture

unsigned  long greenFoodTime = 0;
unsigned  long redFoodTime = 0;
unsigned  long blueFoodTime = 0;
unsigned long foodInterval = 10000; // 10 secondes

// direction de base
String axis = "verticale"; 
int sens = 1;
// variables pour bloquer les changements en sens opposé
int lastDir = sens; 
String lastAxis = axis;

void setup() {
  matrix.fillScreen(matrix.Color333(5, 66, 0)); // Éteint tout les leds
  delay(1000);
  matrix.fillScreen(matrix.Color333(0, 0, 0)); // Éteint tout les leds
  delay(1000);
  Serial.begin(9600);


  // Initalise les positions du serpent en début de partie
  // (0, 0)
  snake[0].x = 0;
  snake[0].y = 2;
  // (1, 0)
  snake[1].x = 0;
  snake[1].y = 1;
  // (2, 0)
  snake[2].x = 0;
  snake[2].y = 0;
}

// dessine le serpent sur la matrice de leds
void drawSnake(){
  // Pour toute les partie du serpent(0 à taille(serpent)-1) pris en sens inverse
  for(int i = sizeof(snake)-1; i>=0; i--){
    matrix.drawPixel(snake[i].x, snake[i].y, Wheel(3)); // Allumage du pixel correspondant à cette partie
  }
}

// Cette fonction créer de la nourriture de la couleur qui est passé en paramètre
// Elle sera utiliser avec un timer
void generateFood(int couleur){
  bool isFood = false; // booléen servant à valider les coordonées de la nourriture générer
  int food_x;
  int food_y;
  Food food;

  while(!isFood){ // Tant que les coordonées de la nourriture ne sont pas validé.
    // génèration des coordonées x  et y entre 0 et 64
    food_x = random(0, 65); 
    food_y = random(0, 65); 

    isFood = true; // On a temporairement générer de la nourriture
    
    // On vérifie que ce ne soit pas déjà un pixel du serpent
    int snakeSize = sizeof(snake) / sizeof(snake[0]);
    for(int i =0; i<snakeSize; i++){
      if (food_x == snake[i].x && food_y == snake[i].y){ // Si la nourriture fait partie des coordonées du serpent
        isFood = false;
      }
    }
  }

  // on ajoute a la liste de nourriture
  food.p.x = food_x;
  food.p.y = food_y;
  food.couleur = couleur;
}

void showFood(){
  int stockSize = sizeof(foodStock) / sizeof(foodStock[0]); // Calcule de la taille de la liste de nourriture
  for (int i=0; i<stockSize; i++){ // 
    matrix.drawPixel(foodStock[i].p.x, foodStock[i].p.y, Wheel(foodStock[i].couleur));
  }
}

// Fait bouger le serpent dans selon le bon axe
// Si le sens est positif le serpent va soit vers la droite soit vers le haut selon l'axe, 
// Si le sens est négatif il va soit vers le bas soit vers la gauche.
void moveSnake(String axis, int sens) {
  int snakeSize = sizeof(snake) / sizeof(snake[0]);

  int dx = 0, dy = 0;
  // Calcule de la nouvelle direction
  if (axis == "horizontale") dx = sens;
  else if (axis == "verticale") dy = sens;

  for (int i = snakeSize - 1; i > 0; i--) {
    snake[i] = snake[i - 1];
  }

  snake[0].x += dx;
  snake[0].y += dy;
}

// Permet de changer axis et dir avec un joystick
void changeMov() {
  int x = analogRead(JOYSTICK_X);
  int y = analogRead(JOYSTICK_Y);

  // Création d'une zone pour laquel les valeurs n'affectent pas le mouvement
  int zoneMorteMin = 400;
  int zoneMorteMax = 700;
  // Changement de mouvement pour aller vers la droite
  if (x > zoneMorteMax && lastAxis != "horizontale") {
    axis = "horizontale";
    sens = 1;
    lastAxis = "horizontale";
    lastDir = sens;
  } // Changement de mouvement pour aller vers la gauche
  else if (x < zoneMorteMin && lastAxis != "horizontale") {
    axis = "horizontale";
    sens = -1;
    lastAxis = "horizontale";
    lastDir = sens;
  } // Changement de mouvement pour aller vers le haut
  else if (y > zoneMorteMax && lastAxis != "verticale") {
    axis = "verticale";
    sens = -1;
    lastAxis = "verticale";
    lastDir = sens;
  } // Changement de mouvement pour aller vers le bas
  else if (y < zoneMorteMin && lastAxis != "verticale") {
    axis = "verticale";
    sens = 1;
    lastAxis = "verticale";
    lastDir = sens;
  }
}



// Implemente ce qui se passe lorsque le serpent mange quelque chose
void snakeEat(Food food){
  
}

// Input a value 0 to 24 to get a color value.
// The colours are a transition r - g - b - back to r.
uint16_t Wheel(byte WheelPos) {
  if(WheelPos < 8) {
   return matrix.Color333(7 - WheelPos, WheelPos, 0);
  } else if(WheelPos < 16) {
   WheelPos -= 8;
   return matrix.Color333(0, 7-WheelPos, WheelPos);
  } else {
   WheelPos -= 16;
   return matrix.Color333(0, WheelPos, 7 - WheelPos);
  }
}


void loop() {
  Serial.println(sizeof(snake)/sizeof(snake[0]));
  matrix.fillScreen(matrix.Color333(0, 0, 0)); // Éteint la matrice pour allumer a nouveaux tout les éléments
  changeMov();
  moveSnake(axis, sens);
  drawSnake();
  showFood();
  // Générer la nourriture toutes bleu les 10 secondes
  if (millis() - blueFoodTime > foodInterval) {
    generateFood(3); // ou une autre couleur
    blueFoodTime = millis();
  }
  // Générer la nourriture toutes rouge les 10 secondes
  if (millis() - redFoodTime > (foodInterval * 1.5)) {
    generateFood(3); // ou une autre couleur
    redFoodTime = millis();
  }
  // Générer la nourriture toutes verte les 10 secondes
  if (millis() - greenFoodTime > (foodInterval * 2)) {
    generateFood(3); // ou une autre couleur
    greenFoodTime = millis();
  }

  delay(500);
}