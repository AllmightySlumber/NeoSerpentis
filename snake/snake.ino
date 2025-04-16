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
#include <List.hpp>
#include <EEPROM.h>
#include "TimerFour.h"

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
const int BLEU=16;
const int VERT=8;
const int ORANGE=2;
const int ROUGE=0;
const int V_MAX = 80;
const int V_MIN = 320;
int VITESSE = 200;
int VIE = 1;
bool SNAKE_LIVING = true;

TimerFour timer;

List<Position> snake; // Tableau de position du serpent de base
volatile List<Food> foodStock; // Tableau de nourriture

const int NOURRITURE_MAX = 20;
unsigned  long greenFoodTime = 0;
unsigned  long redFoodTime = 0;
unsigned  long blueFoodTime = 0;
unsigned  long orangeFoodTime = 0;
unsigned  long foodInterval = 10000; // 10 secondes

unsigned  long mouvementTime = 0;
unsigned  long mouvementInterval = 100;

// direction de base
String axis = "horizontale"; 
int sens = 1;
// variables pour bloquer les changements en sens opposé
int lastDir = sens; 
String lastAxis = axis;

void setup() {
  matrix.begin();
  Serial.begin(9600);

  size_t const address {0};
  unsigned int seed {};
  EEPROM.get(address, seed);
  randomSeed(seed);
  EEPROM.put(address, seed +1 );

  // Initalise les positions du serpent en début de partie
  snake.add({0,2}); // (0, 2)
  snake.add({0,1}); // (0, 1)
  snake.add({0,0}); // (0, 0)

  Food food = {{random(0,64), random(0,64)}, VERT}; // génère une nourriture verte aléatoire
  foodStock.add(food); 

  food = {{random(0,64), random(0,64)}, BLEU}; // génère une nourriture verte aléatoire
  foodStock.add(food);

  int randTime = random(7, 11) * 1000000;; // Temps aléatoire entre 5s et 12s
  timer.initialize(randTime);
  timer.attachInterrupt(deleteFood);

}

// dessine le serpent sur la matrice de leds
void drawSnake(){
  // Pour toute les partie du serpent(0 à taille(serpent)-1) pris en sens inverse
  for(int i = snake.getSize()-1; i>=0; i--){
    matrix.drawPixel(snake.get(i).x, snake.get(i).y,matrix.Color333(34, 0, 150)); // Allumage du pixel correspondant à cette partie
  }
}

// Cette fonction créer de la nourriture de la couleur qui est passé en paramètre
// Elle sera utiliser avec un timer
void generateFood(int couleur){
  if(foodStock.getSize() < NOURRITURE_MAX){
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
      for(int i =0; i<snake.getSize() ; i++){
        if (food_x == snake.get(i).x && food_y == snake.get(i).y){ // Si la nourriture fait partie des coordonées du serpent
          isFood = false;
        }
      }
    }

    // on ajoute a la liste de nourriture
    food.p.x = food_x;
    food.p.y = food_y;
    food.couleur = couleur;
    foodStock.add(food);
  }
}

void showFood(){
  // Calcule de la taille de la liste de nourriture
  for (int i=0; i<foodStock.getSize(); i++){ 
    matrix.drawPixel(foodStock.get(i).p.x, foodStock.get(i).p.y, Wheel(foodStock.get(i).couleur));
  }
}
void deleteFood(){
  if(foodStock.getSize()<3){ // Si la taille des nourriture pourrie est inférieur à 3 on supprime le premier de la liste et l'éteint
    Food food = foodStock.get(0);
    matrix.drawPixel(food.p.x, food.p.y, matrix.Color333(0,0,0)); 
    foodStock.remove(0);
  }
  else{
    int randInt = random(0, 3);
    Food food = foodStock.get(randInt);
    matrix.drawPixel(food.p.x, food.p.y, matrix.Color333(0,0,0)); 
    foodStock.remove(randInt);
  }
  // // Rappelle de la fonction au bout de 5 - 12 secondes
  int randTime = random(5, 13) * 1000000;; // Temps aléatoire entre 5s et 12s
  timer.initialize(randTime);
  timer.attachInterrupt(deleteFood);
}

// Fait bouger le serpent dans selon le bon axe
// Si le sens est positif le serpent va soit vers la droite soit vers le haut selon l'axe, 
// Si le sens est négatif il va soit vers le bas soit vers la gauche.
void moveSnake(String axis, int sens) {
  //int snakeSize = sizeof(snake) / sizeof(snake[0]);

  int dx = 0, dy = 0;
  // Calcule de la nouvelle direction
  if (axis == "horizontale") dx = sens;
  else if (axis == "verticale") dy = sens;

  int taille = snake.getSize() - 1;
  for (int i = taille; i > 0; i--) {
    if(i == taille){
      matrix.drawPixel(snake.get(i).x,  snake.get(i).y, matrix.Color333(0, 0, 0));
    }
    snake.remove(i);
    snake.addAtIndex(i, snake.get(i - 1));
    // snake[i] = snake.get(i - 1);
  }

  // On ajoute à l'élément en tête dx et dy en x et y
  int nouvelle_x = snake.get(0).x + dx;
  int nouvelle_y = snake.get(0).y + dy;
  snake.remove(0);
  snake.addAtIndex(0, {nouvelle_x, nouvelle_y}) ;
  // vérification des bordures
  if(snake.get(0).x == 64 && axis == "horizontale"){
    int y = snake.get(0).y;
    snake.remove(0);
    snake.addAtIndex(0, {0, y}) ;
  }
  if(snake.get(0).x == -1 && axis == "horizontale"){
    int y = snake.get(0).y;
    snake.remove(0);
    snake.addAtIndex(0, {63, y}) ;
  }
  if(snake.get(0).y == -1 && axis == "verticale"){
    int x = snake.get(0).x;
    snake.remove(0);
    snake.addAtIndex(0, {x, 63}) ;
  }
  if(snake.get(0).y == 64 && axis == "verticale"){
    int x = snake.get(0).x;
    snake.remove(0);
    snake.addAtIndex(0, {x, 0}) ;
  }
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
void snakeEat(){

  int snake_x = snake.get(0).x;
  int snake_y = snake.get(0).y;

  for (int i=0; i<foodStock.getSize(); i++){ // On parcours la liste de nourriture
    Food food = foodStock.get(i);
    if(food.p.x==snake_x && food.p.y == snake_y){ // Si le serpent entre en contacte avec de la nourriture avec sa tête
      // Selon la couleur de la nourriture
      int food_couleur = food.couleur;
      if(food_couleur==BLEU){ // Si c'est bleu on augment la taille
        snake.add(snake[ snake.getSize()-1 ]); // On ajoute au serpent son dernier pixel
      }
      else if(food_couleur==VERT){ // Si c'est vert on augmente la vitesse en la divisant par deux et on ajoute une vie
        if(VITESSE > V_MAX){
          VITESSE = VITESSE -= 15;
          Serial.println(VITESSE);
        }
        VIE++;
      }
      else if(food_couleur==ORANGE){ // Si c'est orange on ralentit la vitesse en la multipliant par deux et on enlève une vie
        if(VITESSE < V_MIN){
          VITESSE = VITESSE += 15;
          Serial.println(VITESSE);
        }
        VIE--;
      }
      else if(food_couleur==ROUGE){ // On tue le serpent en faisant une animation
        SNAKE_LIVING = false;
      }
      foodStock.remove(i); // Enlève la nourriture de la liste.
    }
  }

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
  if(SNAKE_LIVING){
    changeMov();
    moveSnake(axis, sens);
    showFood();
    snakeEat();
    drawSnake();
    // Générer la nourriture toutes les 5 à 13 secondes aléatoirement
    if (millis() - blueFoodTime > random(foodInterval * 0.5, foodInterval * 1.3)) {
      generateFood(BLEU); // ou une autre couleur
      blueFoodTime = millis();
    }
    // Générer la nourriture toutes les 10 à 18 secondes aléatoirement
    if (millis() - greenFoodTime > random(foodInterval, foodInterval * 1.8)) {
      generateFood(VERT); // ou une autre couleur
      greenFoodTime = millis();
    }
    // Générer la nourriture toutes les 10 à 18 secondes aléatoirement
    if (millis() - orangeFoodTime > random(foodInterval, foodInterval * 1.8)) {
      generateFood(ORANGE); // ou une autre couleur
      orangeFoodTime = millis();
    }

    // Générer la nourriture toutes rouge les 15 à 23 secondes aléatoirement
    if (millis() - redFoodTime > random(foodInterval * 1.5, foodInterval * 1.8)) {
      generateFood(ROUGE); // ou une autre couleur
      redFoodTime = millis();
    }
    
    delay(320);
  }
  else{
    
    while(!SNAKE_LIVING){
      matrix.fillScreen(matrix.Color333(0, 0, 0));
      delay(100);
      matrix.setCursor(21, 21);
      matrix.setTextColor(matrix.Color333(7,7,7));
      matrix.println("GAME");

      matrix.setCursor(21, 21 + 7*2);
      matrix.println("OVER");

      delay(1000);    
    }
  }
}