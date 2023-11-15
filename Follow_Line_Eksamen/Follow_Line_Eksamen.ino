#include <Wire.h>
#include <Zumo32U4.h>
Zumo32U4Motors motors;
Zumo32U4LineSensors lineSensors;
#define NUM_SENSORS 3
uint16_t lineSensorValues[NUM_SENSORS];
int s1, s2, s3;

//paramaters:
//base speed:
int speed = 400;
//threshold for black line:
int threshold = 600;
void forward(int spd){
  motors.setSpeeds(spd,spd);
}
void turn(int spd){
  //first turns on the spot and then moves slightly forward while turning
  motors.setSpeeds(spd,-spd);
  delay(20);
  motors.setSpeeds(spd,(-0.75*spd));
  delay(75);
}
void readLineSensors(){
  lineSensors.read(lineSensorValues, QTR_EMITTERS_ON);
}
void setup() {
lineSensors.initThreeSensors();
}
void loop() {
readLineSensors();
if (lineSensorValues[0]>=threshold){
  turn(-speed); //same as turning left
}
else{
  if (lineSensorValues[2]>=threshold){
    turn(speed); //same as turning right
  }
  else{
    forward(speed);
  }}}
