#include "DFRobotDFPlayerMini.h"
#include <ESP32Servo.h>

HardwareSerial mySerial(2);
DFRobotDFPlayerMini player;
Servo myServo;

int servoPin = 25;
int A1A = 19;  // IN4
int A1B = 18;  // IN3
int B1A = 33;  // IN2
int B1B = 32;  // IN1

unsigned long startTime;

bool file1Done       = false;
bool movingForward   = false;
bool servoBend1Done  = false;
bool movingBack      = false;
bool servoReturn1Done = false;
bool file2Playing    = false;
bool servoBend2Done  = false;
bool done            = false;

unsigned long file1StartTime    = 0;
unsigned long forwardStartTime  = 0;
unsigned long bend1StartTime    = 0;
unsigned long backStartTime     = 0;
unsigned long return1StartTime  = 0;
unsigned long file2StartTime    = 0;

void setup() {
  Serial.begin(115200);
  mySerial.begin(9600, SERIAL_8N1, 27, 26);

  myServo.attach(servoPin);
  myServo.write(0);
  myServo.detach();

  pinMode(A1A, OUTPUT);
  pinMode(A1B, OUTPUT);
  pinMode(B1A, OUTPUT);
  pinMode(B1B, OUTPUT);

  if (!player.begin(mySerial)) {
    Serial.println("DFPlayer Mini not detected");
    while (true);
  }
  Serial.println("DFPlayer Mini OK");
  player.volume(20);

  // Play first file
  player.play(1);
  file1StartTime = millis();
  startTime = millis();
}

void moveBack() {
  digitalWrite(A1A, HIGH);
  digitalWrite(A1B, LOW);
  digitalWrite(B1A, LOW);
  digitalWrite(B1B, HIGH);
}

void moveForward() {
  digitalWrite(A1A, LOW);
  digitalWrite(A1B, HIGH);
  digitalWrite(B1A, HIGH);
  digitalWrite(B1B, LOW);
}

void stopMotors() {
  digitalWrite(A1A, LOW);
  digitalWrite(A1B, LOW);
  digitalWrite(B1A, LOW);
  digitalWrite(B1B, LOW);
}

void turnRight() {
  digitalWrite(A1A, LOW);
  digitalWrite(A1B, LOW);

  digitalWrite(B1A, HIGH);
  digitalWrite(B1B, LOW);
}

void turnLeft() {
  digitalWrite(A1A, LOW); //A is right motor
  digitalWrite(A1B, HIGH);

  digitalWrite(B1A, LOW);
  digitalWrite(B1B, LOW);
}

void loop() {
  if (done) return;

  unsigned long currentTime = millis();

  if (!file1Done && currentTime - file1StartTime >= 4000) {
    file1Done = true;
  }

  if (file1Done && !movingForward) {
    turnLeft();
    delay(700);

    stopMotors();
    delay(500);

    moveForward();

    forwardStartTime = currentTime;
    movingForward = true;
  }

  if (movingForward && !servoBend1Done && currentTime - forwardStartTime >= 2200) {
    stopMotors();
    
    delay(500);
    myServo.attach(servoPin);
    myServo.write(30);

    delay(500);
    myServo.detach();

    bend1StartTime = currentTime;
    servoBend1Done = true;
  }

  if (servoBend1Done && !movingBack && currentTime - bend1StartTime >= 1200) {
    delay(500);
    moveBack();

    backStartTime = currentTime;
    movingBack = true;
  }

  if (movingBack && !servoReturn1Done && currentTime - backStartTime >= 1500) {
    stopMotors();
    delay(500);

    myServo.attach(servoPin);
    delay(500);
    myServo.write(0);

    delay(500);
    myServo.detach();

    turnRight();

    delay(800);
    stopMotors();

    return1StartTime = currentTime;
    servoReturn1Done = true;
  }

  if (servoReturn1Done && !file2Playing && currentTime - return1StartTime >= 3000) {
    delay(100);
    player.play(2); // play second audio file
    
    file2Playing = true;
    file2StartTime = currentTime;
  }

  if (file2Playing && !servoBend2Done && currentTime - file2StartTime >= 5000) {
    myServo.attach(servoPin);
    myServo.write(30);
    delay(4000); 
    
    myServo.write(0); 
    delay(1000); 
    myServo.detach(); 
    servoBend2Done = true;
    done = true;
  }
}