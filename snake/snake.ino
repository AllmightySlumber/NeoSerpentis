#include <DFRobot_RGBMatrix.h> // Hardware-specific library
#include <List.hpp>
#include <EEPROM.h>
#include <TimerFour.h>

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
struct Obstacle{
  List<Position> v;
};

const int BLEU=16;
const int VERT=8;
const int ORANGE=2;
const int ROUGE=0;
const int V_MAX = 80;
const int V_MIN = 200;
int VITESSE = 200;
int VIE = 1;
bool startGame = false;
bool pauseMenu = false;

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

bool showGameOver = true;
unsigned long lastBlinkTime = 0;
unsigned long blinkInterval = 1000; // 1s allumé

bool gameOver = false;
bool homeMenu = true;
bool commandeBool = false;
bool regleBool = false; 
int homeChoice = 1;
int pauseChoice = 1;
bool justArrived = false;

unsigned long homeLastChoix = 0;
unsigned long homeChoixInterval = 400;
unsigned  long regleLastChange = 0;
unsigned  long regleChangeInterval = 400;
unsigned  long commandeLastChange = 0;
unsigned  long commandeChangeInterval = 400;
unsigned  long pauseMenuLastChoice = 0;
unsigned  long pauseMenuInterval = 400;


// direction de base
String axis = "horizontale"; 
int sens = 1;
String axisStored = axis;
int sensStored = sens;
// variables pour bloquer les changements en sens opposé
int lastDir = sens; 
String lastAxis = axis;

int joystickX;
int joystickY;
bool joystickPressed = false;

void setup() {
  matrix.begin();
  Serial.begin(9600);
  Serial1.begin(9600);
  size_t const address {0};
  unsigned int seed {};
  EEPROM.get(address, seed);
  randomSeed(seed);
  EEPROM.put(address, seed +1 );

  snake.add({0,2}); // (0, 2)
  snake.add({0,1}); // (0, 1)
  snake.add({0,0}); // (0, 0)

  Food food = {{random(0,64), random(0,64)}, VERT}; // génère une nourriture verte aléatoire
  foodStock.add(food);

  food = {{random(0,64), random(0,64)}, BLEU}; // génère une nourriture verte aléatoire
  foodStock.add(food);
  showFood();

  int randTime = random(10, 16) * 1000000;; // Temps aléatoire entre 10s et 15s
  timer.initialize(randTime);
  timer.attachInterrupt(deleteFood);

}

// Fonction pour naviguer et sélection une option dans le home menu
void changeHomeChoice(){
  // Lecture Joystick
  joystickY = analogRead(JOYSTICK_Y);
  joystickX = analogRead(JOYSTICK_X);

  if(joystickX == 1023 && !joystickPressed){ // Détection d'appui sur le joystick
    joystickPressed = true;
    homeLastChoix = millis();
    Serial1.println(homeChoice);
    Serial.println("Sélectionné : " + String(homeChoice));
    if (homeChoice == 1){ // Lancement du jeu
      homeMenu = false;
      startGame = true;
      axis = axisStored; 
      sens = sensStored;
      restartGame();
      Serial.println("On lance le jeu et quitte le menu home.");
      delay(500);
    }
    else if(homeChoice == 2){ // Lancement de la page des commandes
      commandeBool = true;
      homeMenu = false;
      Serial.println("On quit le menu home pour entrer dans les commandes.");
    }
    else if (homeChoice == 3){ // Lancement de la page des règles
      Serial.println("On quit le menu home pour entrer dans les règles.");
      regleBool = true;
      homeMenu = false;
    }
  }
  else if(joystickX < 1000){ // Joystick relaché
    joystickPressed = false;
    if (joystickY > 700){ // Sélection de l'option du dessus
      homeLastChoix = millis();
      homeChoice = (homeChoice == 1) ? 3 : (homeChoice == 2) ? 1 : 2; // si on est déjà tout en haut on repasse en bas 
      // sinon si on est au deuxième on passe au 1 etc
      Serial1.println("C:"+ String(homeChoice));
      Serial.println("Envoyé : " + String(homeChoice));
    }
    else if (joystickY < 400){ // Sélection de l'option du dessous
      homeLastChoix = millis();
      homeChoice = (homeChoice == 1) ? 2 : (homeChoice == 2) ? 3 : 1;
      Serial1.println("C:"+ String(homeChoice));
      Serial.println("Envoyé : " + String(homeChoice));
    }
  }
}
// Fonction pour naviguer entre les pages de règle et revenir au home menu
void changeRegleTexte(){
  joystickX = analogRead(JOYSTICK_X);
  if (joystickX == 1023 && !joystickPressed){
    joystickPressed = true;
    Serial.println("Retour au menu");
    regleLastChange = millis();
    homeMenu = true;
    regleBool = false;
    Serial1.println("M");
  }
  else if (joystickX < 1000){
    joystickPressed = false;
    if (joystickX < 400){
      regleLastChange = millis();
      Serial1.println("P"); // P pour précédent
      Serial.println("Page de règle précédente");
    }
    else if (joystickX > 700 && joystickX <1000){
      regleLastChange = millis();
      Serial1.println("S"); // S pour suivant
      Serial.println("Page de règle suivante");
    }
  }
}
// Fonction pour naviguer entre les commande et revenir au home menu
void changeCommande(){
  joystickX = analogRead(JOYSTICK_X);
  if (joystickX == 1023 && !joystickPressed){
    joystickPressed = true;
    Serial.println("Retour au menu");
    commandeLastChange = millis();
    homeMenu = true;
    commandeBool = false;
    Serial1.println("M");
  }
  else if (joystickX < 1000){
    joystickPressed = false;
    if (joystickX < 400){
      commandeLastChange = millis();
      Serial1.println("P"); // P pour précédent
      Serial.println("Page de commande précédente");
    }
    else if (joystickX > 700 && joystickX <1000){
      commandeLastChange = millis();
      Serial1.println("S"); // S pour suivant
      Serial.println("Page de commande suivante");
    }
  }
}
// Fonction pour relancer le jeu proprement une fois une partie terminé ou abandonner
void restartGame(){
  VITESSE = 200;
  snake.clear();
  snake.add({0,2}); // (0, 2)
  snake.add({0,1}); // (0, 1)
  snake.add({0,0}); // (0, 0)

  Serial1.println(VITESSE);
  Serial1.println(snake.getSize()-3);

  foodStock.clear();
  Food food = {{random(0,64), random(0,64)}, VERT}; // génère une nourriture verte aléatoire
  foodStock.add(food);

  food = {{random(0,64), random(0,64)}, BLEU}; // génère une nourriture verte aléatoire
  foodStock.add(food);
  showFood();

  int randTime = random(10, 16) * 1000000;; // Temps aléatoire entre 10s et 15s
  timer.initialize(randTime);
  timer.attachInterrupt(deleteFood);
}
// Fonction pour dessiner le serpent sur la matrice
void drawSnake(){
  // Extinction du dernier pixel
  // Pour toute les partie du serpent(0 à taille(serpent)-1) pris en sens inverse
  for(int i = snake.getSize()-1; i>=0; i--){
    matrix.drawPixel(snake.get(i).x, snake.get(i).y, matrix.Color333(7, 0, 7)); // Allumage du pixel correspondant à cette partie
  }
}
// Fonction pour créer une nourriture de la couleur passé en paramètre
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
// Fonction pour afficher de la nourriture
void showFood(){
  // Calcule de la taille de la liste de nourriture
  for (int i=0; i<foodStock.getSize(); i++){ 
    matrix.drawPixel(foodStock.get(i).p.x, foodStock.get(i).p.y, Wheel(foodStock.get(i).couleur));
  }
}
// Fonction pour supprimer de la nourriture et la rendre invisible
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
  int randTime = random(10, 16) * 1000000;; // Temps aléatoire entre 10s et 15s
  timer.initialize(randTime);
  timer.attachInterrupt(deleteFood);
}
// Fonction pour faire avancer le serpent
void moveSnake(String axis, int sens) {
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
  checkSelfCollision();
}
// Fonction pour changer la direction et le sens dans lequel avance le serpent
void changeMov() {
  joystickX = analogRead(JOYSTICK_X);
  joystickY = analogRead(JOYSTICK_Y);

  // Création d'une zone pour laquel les valeurs n'affectent pas le mouvement
  int zoneMorteMin = 400;
  int zoneMorteMax = 700;
  // Changement de mouvement pour aller vers la droite
  if (joystickX > zoneMorteMax && lastAxis != "horizontale") {
    axis = "horizontale";
    sens = 1;
    lastAxis = "horizontale";
    lastDir = sens;
  } // Changement de mouvement pour aller vers la gauche
  else if (joystickX < zoneMorteMin && lastAxis != "horizontale") {
    axis = "horizontale";
    sens = -1;
    lastAxis = "horizontale";
    lastDir = sens;
  } // Changement de mouvement pour aller vers le haut
  else if (joystickY > zoneMorteMax && lastAxis != "verticale") {
    axis = "verticale";
    sens = -1;
    lastAxis = "verticale";
    lastDir = sens;
  } // Changement de mouvement pour aller vers le bas
  else if (joystickY < zoneMorteMin && lastAxis != "verticale") {
    axis = "verticale";
    sens = 1;
    lastAxis = "verticale";
    lastDir = sens;
  }
}
// Fonction qui implémente ce qui se passe si le serpent entre en collision avec lui même
void checkSelfCollision() {
  Position head = snake.get(0);
  for (int i = 1; i < snake.getSize(); i++) {
    if (head.x == snake.get(i).x && head.y == snake.get(i).y) {
      startGame = false;
      gameOver = true;
      Serial1.println("G:OVER");
      GameOver();
      break;
    }
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
      if(VITESSE > V_MAX){
          VITESSE = (random(1,4) == 3) ? VITESSE -= 10 : VITESSE;
          Serial1.println("V:" + String((200 - VITESSE)/10));
          Serial.println("envoyé : VITESSE " + String((200 - VITESSE) /10));
        }
        snake.add(snake[ snake.getSize()-1 ]); // On ajoute au serpent son dernier pixel
        Serial1.println("S:" + String(snake.getSize() - 3));
      }
      else if(food_couleur==VERT){ // Si c'est vert on augmente la vitesse en la divisant par deux et on ajoute une vie
        if(VITESSE > V_MAX){
          VITESSE = (random(1,5) == 4) ? VITESSE -= 10 : VITESSE;
          Serial1.println("V:" + String((200 - VITESSE)/10));
          Serial.println("envoyé : VITESSE " + String((200 - VITESSE) /10));
        }
        snake.add(snake[ snake.getSize()-1 ]); // On ajoute au serpent son dernier pixel
        Serial1.println("S:" + String(snake.getSize() - 3));
        Serial.println("envoyé : " + String(snake.getSize()-3));
      }
      else if(food_couleur==ORANGE){ // Si c'est orange on ralentit la vitesse en la multipliant par deux et on enlève une vie
        if(VITESSE < V_MIN){
          VITESSE = (random(1,5) == 4) ? VITESSE += 10 : VITESSE;
          Serial.println("envoyé : VITESSE " + String((200 - VITESSE) /10));
          Serial1.println("V:" + String( (200 - VITESSE) /10));
        }
        // Enlève un pixel au serpent
        Position p = snake.get(snake.getSize()-1);
        snake.removeLast();
        matrix.drawPixel(p.x, p.y, matrix.Color333(0, 0, 0));
        Serial1.println("S:" + String(snake.getSize() - 3));
        if(snake.getSize() < 3) {
          startGame = false; // Si la taille du serpent est inférieur à trois le serpent meurt (de base la taille c'est 3)
          gameOver = true;
          Serial1.println("G:OVER");
          GameOver();
        }
      }
      else if(food_couleur==ROUGE){ // On tue le serpent en faisant une animation
        startGame = false;
        gameOver = true;
        Serial1.println("G:OVER");
        GameOver();
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
// Fonction qui est appelé lorsqu'une partie viens de finir
void GameOver(){
  gameOver = true;
  showGameOver = true;
  lastBlinkTime = millis();
  blinkInterval = 1000;
}
// Fonction qui implémente le déroulement d'une partie
void gameFunction(){
  joystickX = analogRead(JOYSTICK_X);
  if (joystickX == 1023 && !joystickPressed){
    joystickPressed = true;
    pauseMenu =true;
    startGame = false;
    Serial1.println("P"); // Envoie de pause au l'autre Arduino
    Serial.println("Mise en pause du jeu");
    timer.stop(); // Arrêt de la fonction deleteFood
  }
  if (joystickX < 1000) joystickPressed = false;
  changeMov();
  moveSnake(axis, sens);
  snakeEat();
  
  //Position snakeLastPixel = snake.get(snake.getSize()-1);
  matrix.drawPixel(snake.get(snake.getSize()).x, snake.get(snake.getSize()).y, matrix.Color333(0, 0, 0));
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
  showFood();
}
// Fonction qui implémente ce qui se passe lorsqu'une partie est finie
void gameOverFunction(){
  unsigned long currentMillis = millis();
  // Clignotement de Game Over
  if (currentMillis - lastBlinkTime >= blinkInterval) {
    lastBlinkTime = currentMillis;
    showGameOver = !showGameOver;
    if (showGameOver) {
      // Affiche "Game Over"
      matrix.fillScreen(matrix.Color333(0, 0, 0));
      matrix.setCursor(21, 21);
      matrix.setTextColor(matrix.Color333(7,7,7));
      matrix.println("GAME");
      matrix.setCursor(21, 21 + 7*2);
      matrix.println("OVER");
      blinkInterval = 1000; // Texte affiché 1 seconde
    } else {
      // Éteint l’écran
      matrix.fillScreen(matrix.Color333(0, 0, 0));
      blinkInterval = 500; // Pause 0.5 seconde
    }
  }
  // Relancement du jeu
  joystickX = analogRead(JOYSTICK_X);
  if (joystickX == 1023) {
    joystickPressed = true;
    axis = axisStored; 
    sens = sensStored;
    startGame = true;
    gameOver = false;
    matrix.fillScreen(matrix.Color333(0, 0, 0));
    restartGame(); // Redémarrage de la partie proprement
    Serial1.println("B");
  }
}

void pauseFunction(){
  // Lecture Joystick
  joystickY = analogRead(JOYSTICK_Y);
  joystickX = analogRead(JOYSTICK_X);

  if(joystickX == 1023 && !joystickPressed){ // Détection d'appui sur le joystick
    joystickPressed = true;
    pauseMenuLastChoice = millis();
    Serial1.println(pauseChoice);
    Serial.println("Sélectionné : " + String(pauseChoice));
    if (pauseChoice == 1){ // Lancement du jeu
      pauseMenu = false;
      startGame = true;
      timer.start();
      Serial.println("Choix : Continuer le jeu");
      delay(500);
    }
    else if(pauseChoice == 2){ // Lancement de la page des commandes
      homeChoice = 1;
      pauseChoice = 1;
      homeMenu = true;
      pauseMenu = false;
      matrix.fillScreen(matrix.Color333(0, 0, 0));
      Serial.println("Choix : Menu home");
    }
  }
  else if(joystickX < 1000){ // Joystick relaché
    joystickPressed = false;
    if (joystickY > 700){ // Sélection de l'option du dessus
      pauseMenuLastChoice = millis();
      pauseChoice = (pauseChoice == 1) ? 2 : 1;
      Serial1.println("C:"+ String(pauseChoice));
      Serial.println("Changement de la sélection");
    }
    else if (joystickY < 400){ // Sélection de l'option du dessous
      pauseMenuLastChoice = millis();
      pauseChoice = (pauseChoice == 1) ? 2 : 1;
      Serial1.println("C:"+ String(pauseChoice));
      Serial.println("Changement de la sélection");
    }
  }
}

void loop() {
  if (homeMenu){
    if(millis() - homeLastChoix > homeChoixInterval) {// Si notre dernier choix remonte à 200 milisecondes on peut à nouveaux choisir
      changeHomeChoice();
    }
    if (Serial1.available()){
      String message = Serial1.readStringUntil('\n');
    }
  }
  else if(commandeBool){
    if (millis() - commandeLastChange > commandeChangeInterval){
      changeCommande();
    }
  }
  else if(regleBool){
    if (millis() - regleLastChange > regleChangeInterval){
      changeRegleTexte();
    }
  }
  else if(startGame){
    gameFunction();
    delay(VITESSE);
  }
  else if(pauseMenu){
    if (millis() - pauseMenuLastChoice > pauseMenuInterval){
      pauseFunction();
    }
  }
  else if (gameOver){
    gameOverFunction();
  }
}