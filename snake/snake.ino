
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

List<Position> snake; // Tableau de position du serpent de base
List<Food> foodStock; // Tableau de nourriture

const int NOURRITURE_MAX = 15;
unsigned  long greenFoodTime = 0;
unsigned  long redFoodTime = 0;
unsigned  long blueFoodTime = 0;
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



  // Initalise les positions du serpent en début de partie
  snake.add({0,2}); // (0, 2)
  snake.add({0,1}); // (0, 1)
  snake.add({0,0}); // (0, 0)

  foodStock.add({{35, 35}, 8}); // food{ p= {35, 35}, couleur = 8}
  foodStock.add({{24, 56}, 16}); // food{ p= {24, 56}, couleur = 16}
}

// dessine le serpent sur la matrice de leds
void drawSnake(){
  // Pour toute les partie du serpent(0 à taille(serpent)-1) pris en sens inverse
  for(int i = snake.getSize()-1; i>=0; i--){
    matrix.drawPixel(snake.get(i).x, snake.get(i).y, Wheel(3)); // Allumage du pixel correspondant à cette partie
  }
}

// Cette fonction créer de la nourriture de la couleur qui est passé en paramètre
// Elle sera utiliser avec un timer
void generateFood(int couleur){
  if(foodStock.getSize() <= NOURRITURE_MAX){
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
  for (int i=0; i<foodStock.getSize(); i++){ // 
    matrix.drawPixel(foodStock.get(i).p.x, foodStock.get(i).p.y, Wheel(foodStock.get(i).couleur));
  }
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
      snake.add(snake[ snake.getSize()-1 ]); // On ajoute au serpent son dernier pixel
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
  Serial.println(foodStock.getSize());
  changeMov();
  moveSnake(axis, sens);
  showFood();
  snakeEat();
  drawSnake();
  // Générer la nourriture toutes bleu les 10 secondes
  if (millis() - blueFoodTime > foodInterval) {
    generateFood(16); // ou une autre couleur
    blueFoodTime = millis();
  }
  // Générer la nourriture toutes rouge les 15 secondes
  if (millis() - redFoodTime > (foodInterval * 1.5)) {
    generateFood(0); // ou une autre couleur
    redFoodTime = millis();
  }
  // Générer la nourriture toutes verte les 20 secondes
  if (millis() - greenFoodTime > (foodInterval * 2)) {
    generateFood(8); // ou une autre couleur
    greenFoodTime = millis();
  }
  delay(200);
}