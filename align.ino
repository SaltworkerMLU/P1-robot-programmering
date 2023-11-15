#include <Wire.h>
#include <Zumo32U4.h>
#include <PololuSH1106Main.h>
#include <Zumo32U4Encoders.h>

Zumo32U4LineSensors lineSensors;
Zumo32U4Encoders encoders;
Zumo32U4Motors motors;

#define NUM_SENSORS 3
uint16_t lineSensorValues[NUM_SENSORS];

int threshold = 600;
int speed = 100;
int caseStep = 0;
bool leftReachedTape = false, leftOverTape = false;
bool rightReachedTape = false, rightOverTape = false;

void setup() {
  // put your setup code here, to run once:
  lineSensors.initThreeSensors();
}

void loop(){


  readLineSensors();

  if (lineSensorValues[0] < threshold && lineSensorValues[2] < threshold){
    caseStep = 0;
  } else if (lineSensorValues[0] >= threshold){
    caseStep = 2;
    leftReachedTape = true;
  } else if (lineSensorValues[2] >= threshold){
    caseStep = 2;
    rightReachedTape = true;
  }

  switch (caseStep){
    case 0: forward(speed, speed);
            break;
    case 1: stop();
            break;
    case 2: stop();
            checkOver();
            break;
  }

  delay(100);
}

void forward(int spdL, int spdR){
  motors.setSpeeds(spdL, spdR);
}
void stop(){
  motors.setSpeeds(0, 0);
}

void checkOver(){
  delay(500);
  forward(speed/2, speed/2);

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

  forward(speed, speed);

  while (run){
    readLineSensors();

    //  Checks left sid
    if (leftReachedTape && lineSensorValues[0] < threshold){      //  Goes back if over tape.
      motors.setLeftSpeed(-speed);
      leftOverTape = true;
    } else if(leftOverTape && lineSensorValues[0] >= threshold){  //  Only stops if it have been over the tape once.
      motors.setLeftSpeed(0);
    }

    //  Checks right side
    if (rightReachedTape && lineSensorValues[2] < threshold){     //  Goes back if over tape.
      motors.setRightSpeed(-speed);
      rightOverTape = true;
    } else if (rightOverTape && lineSensorValues[2] >= threshold){  //  Only stops if it have been over the tape once.
      motors.setRightSpeed(0);
    }
  }
}

void readLineSensors(){
  lineSensors.read(lineSensorValues, QTR_EMITTERS_ON);
}

