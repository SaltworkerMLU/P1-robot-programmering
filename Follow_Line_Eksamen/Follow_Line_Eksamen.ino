#include <Wire.h>
#include <Zumo32U4.h>
Zumo32U4Motors motors;
Zumo32U4LineSensors lineSensors;

#define NUM_SENSORS 3
uint16_t lineSensorValues[NUM_SENSORS];
int s1, s2, s3;

//paramaters:
//base speed:
int speed = 300;
//threshold for black line:
int threshold = 600;
//number for case:
int nr = 0;
void forward(){
  int spd = speed;
  motors.setSpeeds(spd,spd);
}
void turnLeft(){
  int spd = speed;
/*  motors.setSpeeds(-spd,-spd);
  delay(20);*/
  motors.setSpeeds(-spd,spd);
  delay(20);
  motors.setSpeeds((-0.75*spd),spd);
  delay(75);
}
void turnRight(){
  int spd = speed;
/*  motors.setSpeeds(-spd,-spd);
  delay(20);*/
  motors.setSpeeds(spd,-spd);
  motors.setSpeeds(spd,(-0.75*spd));
  delay(75);
}
void printReadingsToSerial(){
  char buffer[80];
  sprintf(buffer, "%4d %4d %4d\n",
    lineSensorValues[0],
    lineSensorValues[1],
    lineSensorValues[2]
  );
  Serial.print(buffer);
}
void readLineSensors(){
  lineSensors.read(lineSensorValues, QTR_EMITTERS_ON);
  printReadingsToSerial();
}
void setup() {
  // put your setup code here, to run once:
lineSensors.initThreeSensors();
}
void loop() {
  // put your main code here, to run repeatedly:
readLineSensors();
if (lineSensorValues[0]>=threshold){
  turnLeft();
}
else{
  if (lineSensorValues[2]>=threshold){
    turnRight();
  }
  else{
    forward();
  }
}}
