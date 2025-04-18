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
  Serial.begin(9600);
  Serial1.begin(9600);
  
  beginScoreBoard();
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