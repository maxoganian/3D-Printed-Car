#define BLYNK_PRINT SerialUSB


#include <SPI.h>
#include <WiFi101.h>
#include <BlynkSimpleWiFiShield101.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "4RUqJGhZ-xODDol8xjXVnzyUShF07ADK";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Tuna";
char pass[] = "O'Ganian";

//variables for motor driver
int leftPin1 = 0;
int leftPin2 = 1;
int leftPinSpeed = 2;

int rightPin1 = 6;
int rightPin2 = 7;
int rightPinSpeed = 5;

boolean turningInPlace = false;

int x;
int y;

int rightForwardSpeed;
int rightBackwardSpeed;

int leftForwardSpeed;
int leftBackwardSpeed;

//Every time we receive values from either of these pins these functions will execute
//v0 is the button pin
BLYNK_WRITE(V0) {
  //convert the blynk info into a true or false value
  int y = param.asInt();
  //set turning in place flag
  if (y) {
    Serial.println("Turning in Place");
    turningInPlace = true;
  }
  else {
    Serial.println("Not Turning in Place");
    turningInPlace = false;
  }
}

//v1 is the joystick pin
BLYNK_WRITE(V1) {
  //receive the joystick values and save them as x and y
  x = param[0].asInt();
  y = param[1].asInt();

  // Print Joystick values to the Serial Moniter
  SerialUSB.print("X = ");
  SerialUSB.print(x);
  SerialUSB.print("; Y = ");
  SerialUSB.println(y);
 
  //do we use the joystick for spinning or regular motion
  if (turningInPlace == false) {
    turning();
  }
  else {
    turningInPlace1();
  }
}

void setup() {
  // put your setup code here, to run once:
  // Debug console
  SerialUSB.begin(9600);

  Blynk.begin(auth, ssid, pass);

  for (int i = 0; i < 8; i++) {
    pinMode(i, OUTPUT);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  Blynk.run();

}

void driveLeftMotor(int speed, boolean reverse) {
  analogWrite(leftPinSpeed, speed);
  digitalWrite(leftPin1, reverse);
  digitalWrite(leftPin2, !reverse);
}

void driveRightMotor(int speed, boolean reverse) {
  analogWrite(rightPinSpeed, speed);
  digitalWrite(rightPin1, !reverse);
  digitalWrite(rightPin2, reverse);
}
void motorStop () {
  digitalWrite(rightPin1, LOW);
  digitalWrite(rightPin2, LOW);
  digitalWrite(leftPin1, LOW);
  digitalWrite(leftPin2, LOW);
}

//converts x/y joystick values to motor speed and direction
//the joystick goes from 0 to 500
//250,250 is the center
//all the way up is 500
//all the way right is 500
//We need to map the coordanites of this joystick to two pins that control direction
//and one that controls speed, for each motor

int jOrigin = 250;
int jMax = 500;
int jMin = 0;

int speedMax = 255;
int  speedMin = 0;

int turnValueMax = map(x, jOrigin, jMax, speedMin, speedMax);
int turnValueMin = map(x, jOrigin, jMin, speedMin, speedMax);

int driveValueMax = map(y, jOrigin, jMax, speedMin, speedMax);
int driveValueMin = map(y, jOrigin, jMin, speedMin, speedMax);

void turning () {
  
  boolean isYPositive = y > jOrigin;
   
  if (y == jOrigin){
    motorStop();
  }
  else{
    if (isYPositive) {
      if (x < jOrigin) {
        leftForwardSpeed = driveValueMax;
        rightForwardSpeed = driveValueMax + turnValueMin;
      }
      else{
        leftForwardSpeed = driveValueMax + turnValueMax;
        rightForwardSpeed = driveValueMax;
      }
    }
    else{
      if (x < jOrigin) {
        leftForwardSpeed = driveValueMin;
        rightForwardSpeed = driveValueMin + turnValueMin;
      }
      else{
        leftForwardSpeed = driveValueMin + turnValueMax;
        rightForwardSpeed = driveValueMin;
      }
    }
  
    leftForwardSpeed = min(leftForwardSpeed,speedMax);
    rightForwardSpeed = min(rightForwardSpeed, speedMax);
      
    Serial.println(leftForwardSpeed);
    Serial.println(rightForwardSpeed);
    
    driveLeftMotor(leftForwardSpeed, !isYPositive);
    driveRightMotor(rightForwardSpeed, !isYPositive);
  }
}
void turningInPlace1(){
  if (y > 250) {
      if (x < 230) {
        leftForwardSpeed = driveValueMax + turnValueMin;
        rightForwardSpeed = driveValueMax + turnValueMin;
        if (leftForwardSpeed > 255) {
          leftForwardSpeed = 255;
        }
        if (rightForwardSpeed > 255) {
          rightForwardSpeed = 255;
        }
        Serial.println(leftForwardSpeed);
        Serial.println(rightForwardSpeed);
        
        driveLeftMotor(leftForwardSpeed, true);
        driveRightMotor(rightForwardSpeed, false);
      }
      else if (x > 280) {
        leftForwardSpeed = driveValueMax  + turnValueMax;
        rightForwardSpeed = driveValueMax + turnValueMax;
        if (leftForwardSpeed > 255) {
          leftForwardSpeed = 255;
        }
        if (rightForwardSpeed > 255) {
          rightForwardSpeed = 255;
        }
        driveLeftMotor(leftForwardSpeed, false);
        driveRightMotor(rightForwardSpeed, true);
      }
      else {
        leftForwardSpeed = driveValueMax;
        rightForwardSpeed = driveValueMax;

        driveLeftMotor(leftForwardSpeed, false);
        driveRightMotor(rightForwardSpeed, false);
      }
      
      Serial.println(leftForwardSpeed);
      Serial.println(rightForwardSpeed);
  }
  else if (y < 250){
    if (x < 230) {
        leftForwardSpeed = map(y, 500, 250, 0, 255) + turnValueMin;
        rightForwardSpeed = map(y, 500, 250, 0, 255) + turnValueMin;
        if (leftForwardSpeed > 255) {
          leftForwardSpeed = 255;
        }
        if (rightForwardSpeed > 255) {
          rightForwardSpeed = 255;
        }
        driveLeftMotor(leftForwardSpeed, true);
        driveRightMotor(rightForwardSpeed, false);
      }
      else if (x > 280) {
        leftForwardSpeed = map(y, 500, 250, 0, 255) + turnValueMax;
        rightForwardSpeed = map(y, 500, 250, 0, 255) + turnValueMax;
        if (leftForwardSpeed > 255) {
          leftForwardSpeed = 255;
        }
        if (rightForwardSpeed > 255) {
          rightForwardSpeed = 255;
        }
        driveLeftMotor(leftForwardSpeed, false);
        driveRightMotor(rightForwardSpeed, true);
      }
      else {
        leftForwardSpeed = map(y,500, 250, 0, 255);
        rightForwardSpeed = map(y, 500, 250, 0, 255);

        driveLeftMotor(leftForwardSpeed, true);
        driveRightMotor(rightForwardSpeed, true);
      }
      
      Serial.println(leftForwardSpeed);
      Serial.println(rightForwardSpeed);
  }
  else {
    motorStop();
  }
}
