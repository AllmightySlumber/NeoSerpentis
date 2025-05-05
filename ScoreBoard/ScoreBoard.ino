#include <DFRobot_RGBMatrix.h> // Hardware-specific library
#include <List.hpp>
#include <EEPROM.h>
#include <TimerFour.h>

#define OE    9
#define LAT   10
#define CLK   11
#define A     A0
#define B     A1
#define C     A2
#define D     A3
#define E     A4
#define WIDTH 64
#define _HIGH 64
// Les entrées analogiques du joystick
#define JOYSTICK_Y A7
#define JOYSTICK_X A6

DFRobot_RGBMatrix matrix(A, B, C, D, E, CLK, LAT, OE, false, WIDTH, _HIGH);


TimerFour timer;

String message;
String score;
String speed;
bool gameOver = false;

unsigned long lastBlinkTime = 0;
unsigned long blinkInterval = 1000; // 1s allumé

bool homeMenuBool = true;
bool startGame = false;
bool commandeBool = false;
bool regleBool = false;
bool pause = false;

int homeMenuChoice = 1;
int commandeChecker = 1;
int pauseChoice = 1;
int regleChecker = 1;

unsigned  long homeLastChoix = 0;
unsigned  long homeChoixInterval = 400;
unsigned  long commandeLastChange = 0;
unsigned  long commandeChangeInterval = 400;
unsigned  long regleLastChange = 0;
unsigned  long regleChangeInterval = 400;
unsigned  long pauseLastChoice = 0;
unsigned  long pauseMenuInterval = 400;

const String startGameTexte = "Start Game";
const String regleTexte = "Regles";
const String commandeTexte = "Commandes";
const String regleTexte1 = "Le but du jeu est simple faire grandire le plus possible le serpent.";
const String regleTexte2 = "La nourriture bleu le fait grandir, et l'accelere avec 33% de chance.";
const String regleTexte3 = "La verte le fait grandir et a 25% de change de l'accelerer.";
const String regleTexte4 = " La orange le rappetisse et a 25% de chance de le ralentir.";
const String regleTexte5 = "La rouge met fin a la partie en tuant le serpent.";

void setup() {
  matrix.begin();
  matrix.setTextWrap(false);
  Serial.begin(9600);
  Serial1.begin(9600);
  startAnimation();
  delay(200);
  homeMenu();
  homeChoice();
}
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
void showPauseMenu(){
  matrix.fillScreen(matrix.Color333(0, 0, 0));
  matrix.setCursor(0, 0);
  matrix.println("Resume");
  matrix.setCursor(0, 16);
  matrix.println("Home Menu");
}
// Trace une ligne jaune sous le choix actuel du menu pause
void pauseMenuChoice(){
  String h = "Home Menu";
  String r = "Resume";
  if (pauseChoice == 1){
    for (int i = 0; i<h.length() * 5 + 8; i++){
      matrix.drawPixel(i, 25, matrix.Color333(0, 0, 0));
      if(i < r.length()*5 + 5) matrix.drawPixel(i, 8, matrix.Color333(7, 7, 0));
    }
  }
  else{
    for (int i = 0; i<h.length() * 5 + 8; i++){
      matrix.drawPixel(i, 25, matrix.Color333(7, 7, 0));
      if(i < r.length() * 5 + 5) matrix.drawPixel(i, 8, matrix.Color333(0, 0, 0));
    }
  }
}
// Présente les différents choix possible depuis le menu
void homeMenu(){
  matrix.fillScreen(matrix.Color333(0, 0, 0));
  matrix.setCursor(0,0);
  matrix.println(startGameTexte);
  matrix.setCursor(0, 16);
  matrix.println(commandeTexte);
  matrix.setCursor(0, 32);
  matrix.println(regleTexte);
}
// Les pages pour voir les commandes possibles
void commandesPages(int i){
  matrix.setTextColor(matrix.Color333(7, 0, 7));
  matrix.fillScreen(matrix.Color333(0, 0, 0));
  matrix.setCursor(8, 0);
  matrix.println("Joystick");
  if (i==1){
    matrix.setCursor(8, 11);
    matrix.println("Enfoncer:");
    matrix.setCursor(5, 22);
    matrix.println("Selection");
    matrix.setCursor(17, 33);
    matrix.println("Pause");
  }
  else if (i==2){
    matrix.setCursor(11, 11);
    matrix.println("droite:");
    matrix.setCursor(11, 22);
    matrix.println("vers la");
    matrix.setCursor(14, 33);
    matrix.println("droite");
  }
  else if (i==3){
    matrix.setCursor(11, 11);
    matrix.println("gauche:");
    matrix.setCursor(11, 22);
    matrix.println("vers la");
    matrix.setCursor(14, 33);
    matrix.println("gauche");
  }
  else if (i==4){
    matrix.setCursor(17, 11);
    matrix.println("haut:");
    matrix.setCursor(11, 22);
    matrix.println("vers le");
    matrix.setCursor(20, 33);
    matrix.println("haut");
  }
  else if (i==5){
    matrix.setCursor(20, 11);
    matrix.println("bas:");
    matrix.setCursor(11, 22);
    matrix.println("vers le");
    matrix.setCursor(23, 33);
    matrix.println("bas");
  }
  else if (i==6){
    matrix.setCursor(0, 11);
    matrix.println("bas droite:");
    matrix.setCursor(11, 22);
    matrix.println("vers le");
    matrix.setCursor(17, 33);
    matrix.println("bas a");
    matrix.setCursor(14, 44);
    matrix.println("droite");
  }
  else if (i==7){
    matrix.setCursor(0, 11);
    matrix.println("bas gauche:");
    matrix.setCursor(11, 22);
    matrix.println("vers le");
    matrix.setCursor(17, 33);
    matrix.println("bas a");
    matrix.setCursor(14, 44);
    matrix.println("gauche");
  }
  else if (i==8){
    matrix.setCursor(20, 11);
    matrix.println("haut");
    matrix.setCursor(11, 18);
    matrix.println("droite:");
    matrix.setCursor(11, 29);
    matrix.println("vers le");
    matrix.setCursor(14, 40);
    matrix.println("haut a");
    matrix.setCursor(14, 51);
    matrix.println("droite");
  }
  else if (i==9){
    matrix.setCursor(20, 11);
    matrix.println("haut");
    matrix.setCursor(11, 18);
    matrix.println("gauche:");
    matrix.setCursor(11, 29);
    matrix.println("vers le");
    matrix.setCursor(14, 40);
    matrix.println("haut a");
    matrix.setCursor(14, 51);
    matrix.println("gauche");
  }

  if (i==1){
    matrix.setCursor(56, 56);
    matrix.println(">");
  }
  else if (i == 9){
    matrix.setCursor(0, 56);
    matrix.println("<");
  }
  else{
    matrix.setCursor(0, 56);
    matrix.println("<");
    matrix.setCursor(56, 56);
    matrix.println(">");
  }
}
// Les pages de règle de jeu
void reglePages(int i){
  matrix.fillScreen(matrix.Color333(0, 0, 0));
  if (i == 1){
    matrix.setTextColor(matrix.Color333(7, 0, 7));
    matrix.setCursor(5, 0);
    matrix.println("Objectif:");
    matrix.setCursor(15, 12);
    matrix.println("manger");
    matrix.setCursor(26, 24);
    matrix.println("et");
    matrix.setCursor(11, 36);
    matrix.println("grandir");

  }
  else if (i == 2){
    for (int y=0; y<3; y++){
      for (int x=0; x<3; x++){
        matrix.drawPixel(30+x, 2+y, Wheel(16));
      }
    }
    matrix.setTextColor(matrix.Color333(7, 0, 7));
    matrix.setCursor(0, 8);
    matrix.println("Effet:");
    matrix.setCursor(0, 21);
    matrix.println("taille++");
    matrix.setCursor(0, 34);
    matrix.println("33% chance");
    matrix.setCursor(0, 43);
    matrix.println("vitesse++");
  }
  else if (i == 3){
    for (int y=0; y<3; y++){
      for (int x=0; x<3; x++){
        matrix.drawPixel(30+x, 2+y, Wheel(8));
      }
    }
    matrix.setTextColor(matrix.Color333(7, 0, 7));
    matrix.setCursor(0, 8);
    matrix.println("Effet:");
    matrix.setCursor(0, 21);
    matrix.println("taille++");
    matrix.setCursor(0, 34);
    matrix.println("25% chance");
    matrix.setCursor(0, 43);
    matrix.println("vitesse++");
  }
  else if (i == 4){
    for (int y=0; y<3; y++){
      for (int x=0; x<3; x++){
        matrix.drawPixel(30+x, 2+y, Wheel(2));
      }
    }
    matrix.setTextColor(matrix.Color333(7, 0, 7));
    matrix.setCursor(0, 8);
    matrix.println("Effet:");
    matrix.setCursor(0, 21);
    matrix.println("taille--");
    matrix.setCursor(0, 34);
    matrix.println("25% chance");
    matrix.setCursor(0, 43);
    matrix.println("vitesse--");
  }
  else if (i == 5){
    for (int y=0; y<3; y++){
      for (int x=0; x<3; x++){
        matrix.drawPixel(30+x, 2+y, Wheel(0));
      }
    }
    matrix.setTextColor(matrix.Color333(7, 0, 7));
    matrix.setCursor(0, 8);
    matrix.println("Effet:");
    matrix.setCursor(0, 21);
    matrix.println(("Game Over"));
  }

  if (i==1){
    matrix.setCursor(56, 56);
    matrix.println(">");
  }
  else if (i == 5){
    matrix.setCursor(0, 56);
    matrix.println("<");
  }
  else{
    matrix.setCursor(0, 56);
    matrix.println("<");
    matrix.setCursor(56, 56);
    matrix.println(">");
  }
}
// Trace une ligne jaune sous le choix actuel du menu home
void homeChoice(){
  if (homeMenuChoice == 1){
    for (int x=0; x < startGameTexte.length()*5 + 9; x++){
      matrix.drawPixel(x, 8, matrix.Color333(7, 7, 0)); // On trace la ligne en jaune
      // Efface les autres lignes
      if(x<commandeTexte.length()*5 + 8) matrix.drawPixel(x, 24, matrix.Color333(0, 0, 0));
      if(x<regleTexte.length()*5 + 6) matrix.drawPixel(x, 40, matrix.Color333(0, 0, 0));
    }
  }
  else if (homeMenuChoice == 2){
    for (int x=0; x < startGameTexte.length()*5 + 9; x++){
      matrix.drawPixel(x, 8, matrix.Color333(0, 0, 0));
      if(x<commandeTexte.length()*5 + 8) matrix.drawPixel(x, 24, matrix.Color333(7, 7, 0));
      if(x<regleTexte.length()*5 + 6) matrix.drawPixel(x, 40, matrix.Color333(0, 0, 0));
    }
  }
  else{
    for (int x=0; x < startGameTexte.length()*5 + 9; x++){
      matrix.drawPixel(x, 8, matrix.Color333(0, 0, 0));
      if(x<commandeTexte.length()*5 + 8) matrix.drawPixel(x, 24, matrix.Color333(0, 0, 0));
      if(x<regleTexte.length()*5 + 5) matrix.drawPixel(x, 40, matrix.Color333(7, 7, 0));
    }
  }
}
// Animation NéoSerpentis au lancement du jeu
void startAnimation() {
  matrix.fillScreen(matrix.Color333(0, 0, 0));
  matrix.setTextWrap(false); // ⚠️ Empêche les retours à la ligne !

  const char* title = "NeoSerpentis";
  int textLength = strlen(title) * 6; // 5px + 1px d'espacement
  int startX = 64;
  int endX = -textLength;
  int y = 26; // Ligne verticale centrale (ajuste si besoin)

  for (int x = startX; x >= endX; x--) {
    matrix.fillScreen(matrix.Color333(0, 0, 0));
    matrix.setCursor(x, y);
    matrix.setTextColor(matrix.Color333(7, 0, 7));
    matrix.print(title);
    delay(40);
  }
  delay(300);
  matrix.fillScreen(matrix.Color333(0, 0, 0));
}
// Création du scoreBoard
void beginScoreBoard(){
  matrix.fillScreen(matrix.Color333(0, 0, 0));
  score = "0";
  speed = "0";
  afficherScoreBoard();
}
// Affichage et MAJ du scoreBoard
void afficherScoreBoard() {
  // Suppression du score (seulement le nombre)
  for(int y =0; y < 7; y++){
    for (int x=0; x < 63-36; x++){
      matrix.drawPixel(36 + x, y, matrix.Color333(0, 0, 0));
    }
  }

  // Suppression de la vitesse (seulement le nombre)
  for(int y =0; y < 7; y++){
    for (int x=0; x < 63-48; x++){
      matrix.drawPixel(48 + x, 16 + y, matrix.Color333(0, 0, 0));
    }
  }

  matrix.setTextColor(matrix.Color333(7, 7, 0));
  
  matrix.setCursor(0, 0);
  matrix.print("Score:");
  matrix.print(score);

  matrix.setCursor(0, 16);
  matrix.print("Vitesse:");
  matrix.print(speed);
}
// Funcion de clignotement en cas de fin de jeu
void afficherGameOver() {
  matrix.setTextColor(matrix.Color333(7, 7, 0));

  // Suppression du Game Over
  for(int y =0; y < 7; y++){
    for (int x=0; x < 64; x++){
      matrix.drawPixel(x, 32 + y, matrix.Color333(0, 0, 0));
    }
  }
  delay(500);
  matrix.setCursor(0, 32);
  matrix.print("Game Over");
}
// Fonction de déroulement de ce qui se passe tant qu'on est dans le menu
void homeMenuFunction(){
  if(Serial1.available()){ // On lit les messages en entrée 
    if(millis() - homeLastChoix > homeChoixInterval) {// Si notre dernier choix remonte à 200 milisecondes on peut à nouveaux choisir
      homeLastChoix = millis(); 
      message = Serial1.readStringUntil('\n');
      if(message.startsWith("C:")){
        String choix = message.substring(2);
        Serial.println("Choix reçu : " + choix);
        homeMenuChoice = choix.toInt();
        homeChoice();
      }
      else{
        Serial.println("Reçu : " + message);
        if (message.startsWith("1")){
          homeMenuBool = false;
          startGame = true;
          beginScoreBoard(); 
        }
        else if(message.startsWith("2")){
          // montrer les commandes du jeu
          commandeBool = true;
          homeMenuBool = false;
          commandeChecker = 1;
          commandesPages(commandeChecker);
          Serial1.println("Commandes");
        }
        else if(message.startsWith("3")){
          // montrer les règles du jeu
          regleBool = true;
          homeMenuBool = false;
          regleChecker = 1;
          reglePages(regleChecker);
          Serial1.println("Règle");
        }
      }
    }
  }
}
// Fonction de déroulement de ce qui se passe tant qu'on est dans la page commande
void commandeFunction(){
  if (Serial1.available()){
    if (millis() - commandeLastChange > commandeChangeInterval){
      String message = Serial1.readStringUntil('\n');
      if (message.startsWith("M")){
        matrix.fillScreen(matrix.Color333(0, 0, 0));
        commandeBool = false;
        homeMenuBool = true;
        homeMenu();
        homeChoice();
      }
      else if (message.startsWith("S")){
        if (commandeChecker!=9){
          commandeChecker++;
          Serial.println("Message reçu " + message);
          commandesPages(commandeChecker);
        }
      }
      else if (message.startsWith("P")){
        if (commandeChecker!=1){
          commandeChecker--;
          Serial.println("Message reçu " + message);
          commandesPages(commandeChecker);
        }
      }
    }
  }
}
// Fonction de déroulement de ce qui se passe tant qu'on est dans la page des règles
void regleFunction(){
  if (Serial1.available()){
    if (millis() - regleLastChange > regleChangeInterval){
      String message = Serial1.readStringUntil('\n');
      if (message.startsWith("M")){
        matrix.fillScreen(matrix.Color333(0, 0, 0));
        regleBool = false;
        homeMenuBool = true;
        homeMenu();
        homeChoice();
      }
      else if (message.startsWith("S")){
        if (regleChecker!=5){
          regleChecker++;
          Serial.println("Message reçu " + message);
          reglePages(regleChecker);
        }
      }
      else if (message.startsWith("P")){
        if (regleChecker!=1){
          regleChecker--;
          Serial.println("Message reçu " + message);
          reglePages(regleChecker);
        }
      }
    }
  }
}
// Fonction de déroulement d'une partie
void startGameFunction(){
  if (Serial1.available()) {
    message = Serial1.readStringUntil('\n');
    if (message.startsWith("S:")) { // MAJ score
      score = message.substring(2);
      Serial.println("Score reçu : " + score);
    }
    else if (message.startsWith("V:")) { // MAJ vitesse
      speed = message.substring(2);
      Serial.println("Vitesse reçue : " + speed);
    }
    else if (message.startsWith("G:")) { // game Over
      gameOver = true;
      startGame = false;
    }
    afficherScoreBoard();
    if (message.startsWith("P")){
      pause = true;
      startGame = false;
      pauseChoice = 1;
      showPauseMenu();
      pauseMenuChoice();
    }
    
  }
}
// Fonction de déroulement de ce qui se passe en fin de partie
void gameOverFunction(){
  if(millis() - lastBlinkTime > blinkInterval ){ // Clignotement de Game Over
    lastBlinkTime = millis();
    afficherGameOver();
    delay(1000);
  }
  if (Serial1.available()) { // Relancement du jeu
    message = Serial1.readStringUntil('\n');
    if (message.startsWith("B")) {
      matrix.fillScreen(matrix.Color333(0, 0, 0));
      beginScoreBoard();
      gameOver = false;
      startGame = true;
    }
  }
}
// Fonction qui implémente la logique du menu pause
void pauseFunction(){
  if (Serial1.available()){
    if (millis() - pauseLastChoice > pauseMenuInterval){
      pauseLastChoice = millis();
      message = Serial1.readStringUntil('\n');
      if (message.startsWith("C")){
        String choix = message.substring(2);
        Serial.print("Choix :" + choix);
        pauseChoice = choix.toInt();
        pauseMenuChoice();
      }
      if (message.startsWith("1")){
        matrix.fillScreen(matrix.Color333(0, 0, 0));
        afficherScoreBoard();
        startGame = true;
        pause = false;
      }
      else if(message.startsWith("2")){
        pause = false;
        homeMenuBool = true;
        homeMenuChoice = 1;
        homeMenu();
        homeChoice();
      }
    }
  }
}
void loop() {
  if (homeMenuBool){
    homeMenuFunction();
  }
  else if(commandeBool){
    commandeFunction();
  }
  else if (regleBool){
    regleFunction();
  }
  else if(startGame){ // Si on joue
    startGameFunction();
  }
  else if(pause){
    pauseFunction();
  }
  else if(gameOver){
    gameOverFunction();
  } 
}