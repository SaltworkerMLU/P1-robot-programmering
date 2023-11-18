#include <Wire.h>
#include <Zumo32U4.h>
#include <PololuSH1106Main.h>
#include <Zumo32U4Encoders.h>

Zumo32U4LineSensors lineSensors;
Zumo32U4Encoders encoders;
Zumo32U4Motors motors;
Zumo32U4IMU imu;

#define NUM_SENSORS 3
uint16_t lineSensorValues[NUM_SENSORS];

int threshold = 600;
int speed = 100;
int movementParameter = 11;
int maxDistance = 41;
float wheelCirc = 13;
int caseStep = 0;
bool leftReachedTape = false, leftOverTape = false, leftDone = false;
bool rightReachedTape = false, rightOverTape = false, rightDone = false;

void setup() {
  // put your setup code here, to run once:
  lineSensors.initThreeSensors();
}

void loop(){

  readLineSensors();

  if (lineSensorValues[0] < threshold && lineSensorValues[2] < threshold){  //  Goes forward when the zumo have not reached the tape.
    caseStep = 0;
  } else if (lineSensorValues[0] >= threshold && !leftReachedTape){         //  Aligns from the left side.
    caseStep = 1;
    leftReachedTape = true;
  } else if (lineSensorValues[2] >= threshold && !rightReachedTape){        //  Aligns from the right side
    caseStep = 1;
    rightReachedTape = true;
  } else {                                                                  //  Goes forward when the Zumo is aligned.
    caseStep = 2;
  }

  switch (caseStep){    //    Either the zumo goes forward or
    case 0: forward(speed);
            break;
    case 1: stop();
            checkOver();
            break;
    case 2: stopAtlength();
            break;
    default:
            break;
  }

  delay(100);
}

void stopAtlength() {
  delay(1000);
  bool lastRun = true;
  float num = maxDistance - movementParameter;

  resetEncoders();

  motors.setSpeeds(speed, speed);

  while (lastRun){
    if (getDistance() >= num){
      stop();
    }
  }

}

void forward(int spd){
  motors.setSpeeds(spd, spd);
}
void stop(){
  motors.setSpeeds(0, 0);
}

void checkOver(){
  delay(500);
  forward(speed/2);

  bool run = true;

  while(run){
    readLineSensors();
    if(leftReachedTape && lineSensorValues[2] >= threshold){
      stop();
      rightReachedTape = true;
      run = false;
    } else if (rightReachedTape && lineSensorValues[0] >= threshold){
      stop();
      leftReachedTape = true;
      run = false;
    }
  }

  delay(1000);

  run = true;

  forward(speed);

  while (run){
    readLineSensors();

    if (!leftDone || !rightDone){   //  Runs if and only if one of the sides is not aligned.
      //  Checks left side
      if (leftReachedTape && lineSensorValues[0] < threshold){      //  Goes back if over tape.
        motors.setLeftSpeed(-speed);
        leftOverTape = true;
        leftDone = false;
      } else if(leftOverTape && lineSensorValues[0] >= threshold){  //  Only stops if it have been over the tape once.
 
        motors.setLeftSpeed(0);
        leftDone = true;
      }

      //  Checks right side
      if (rightReachedTape && lineSensorValues[2] < threshold){     //  Goes back if over tape.
        motors.setRightSpeed(-speed);
        rightOverTape = true;
        rightDone = false;
      } else if (rightOverTape && lineSensorValues[2] >= threshold){  //  Only stops if it have been over the tape once.
       
        motors.setRightSpeed(0);
        rightDone = true;
      }
    } else {    //  Stops aligning the Zumo when both sides have stopped.
        run = false;
    }
  }
}

void readLineSensors(){
  lineSensors.read(lineSensorValues, QTR_EMITTERS_ON);    //    Reads the three linesensors and insert their value into the array lineSensorValues[].
}

float getDistance(){
  int countsL = encoders.getCountsLeft();
  int countsR = encoders.getCountsRight();

  float distanceL = countsL/900.0 * wheelCirc;
  float distanceR = countsR/900.0 * wheelCirc;

  return (distanceL + distanceR)/2;
}
void resetEncoders(){
  encoders.getCountsAndResetLeft();
  encoders.getCountsAndResetRight();
}
