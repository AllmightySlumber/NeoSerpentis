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



void setup() {
  matrix.begin();
  matrix.setTextWrap(false);
  Serial.begin(9600);
  Serial1.begin(9600);
  startAnimation();
  delay(500);
  
  beginScoreBoard();
}

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






void beginScoreBoard(){
  score = "0";
  speed = "0";
  afficherScoreBoard;
}

void afficherScoreBoard() {
  matrix.fillScreen(matrix.Color333(0, 0, 0));
  matrix.setTextColor(matrix.Color333(7, 7, 0));
  
  matrix.setCursor(0, 0);
  matrix.print("Score:");
  matrix.print(score);

  matrix.setCursor(0, 16);
  matrix.print("Vitesse:");
  matrix.print(speed);
}

void afficherGameOver() {
  matrix.fillScreen(matrix.Color333(0, 0, 0));
  matrix.setTextColor(matrix.Color333(7, 7, 0));
  
  matrix.setCursor(0, 0);
  matrix.print("Score:");
  matrix.print(score);

  matrix.setCursor(0, 16);
  matrix.print("Vitesse:");
  matrix.print(speed);

  matrix.setCursor(0, 32);
  matrix.print("Game Over");
  
}



void loop() {
  if(!gameOver){
    if (Serial1.available()) {
      message = Serial1.readStringUntil('\n');

      if (message.startsWith("S:")) {
        score = message.substring(2);
        Serial.println("Score reçu : " + score);
      }
      else if (message.startsWith("V:")) {
        speed = message.substring(2);
        Serial.println("Vitesse reçue : " + speed);
      }
      else if (message.startsWith("G:")) {
        gameOver = true;
      }

      afficherScoreBoard();
    }
  }
  else{
    if(millis() - lastBlinkTime > blinkInterval ){
      lastBlinkTime = millis();
      afficherGameOver();
      delay(1000);
      afficherScoreBoard();
      delay(500);
    }
    if (Serial1.available()) {
      message = Serial1.readStringUntil('\n');
      if (message.startsWith("B")) {
        beginScoreBoard();
        gameOver = false;
      }
    }
  }
 
}