#include <Wire.h>
#include <Zumo32U4.h>

Zumo32U4OLED OLED;
Zumo32U4LineSensors lineSensors;
Zumo32U4ButtonA buttonA;
Zumo32U4ButtonB buttonB;
Zumo32U4ButtonC buttonC;
Zumo32U4Buzzer buzzer;
Zumo32U4Motors motors;
Zumo32U4Encoders encoders;

#define NUM_SENSORS 3
uint16_t lineSensorValues[NUM_SENSORS];

const int speed = 100;
const int threshold = 600;
const float wheelCirc = 11.9;

void setup() {
  Serial.begin(9600);
  lineSensors.initThreeSensors();
}

void loop(){
  mainFunc();
}

void mainFunc(){
  int maxLength = 41;
  int length;
  bool runLastPart = true;

  //  Gets a value returned from the function.
  length = chsValue();

  countdown();
  alignZumo();

  delay(200);

  resetEncoders();
  forward(speed);

  while (runLastPart){
    if (getDistance() >= maxLength - length){
      stop();
      runLastPart = false;
    }
  }
}


void alignZumo(){
  bool run = true;
  bool lOver = false, rOver = false;

  forward( speed );
  
  while ( run ){
    readLineSensors();

    if ( lineSensorValues[0] >= threshold && lineSensorValues[2] >= threshold ){
      stop();
      run = false;
    }
    else if ( lineSensorValues[0] >= threshold ){
      motors.setLeftSpeed(0);
    }
    else if ( lineSensorValues[2] >= threshold ){
      motors.setRightSpeed(0);
    }

  }

  run = true;

  forward( speed );

  while ( run ){
    readLineSensors();

    if ( lineSensorValues[0] < threshold ){
      motors.setLeftSpeed(0);
      lOver = true;
    }

    if ( lineSensorValues[2] < threshold ){
      motors.setRightSpeed(0);
      rOver = true;
    }

    if (lOver && rOver){
      run = false;
    }
  }
  
}

int chsValue(){
  int value = 0;
  bool chsValue = true;

  printValue( value );

  while (chsValue){
    if (buttonA.isPressed()){
      value++;

      if ( value > 50){
        value = 50;
      }

      buttonA.waitForRelease();
      printValue( value );

      buzz();
    }
    else if (buttonB.isPressed()){
      value--;
      
      if ( value < 1){
        value = 0;
      }

      buttonB.waitForRelease();
      printValue( value );

      buzz();
    }
    else if (buttonC.isPressed()){
      chsValue = false;

      buttonC.waitForRelease();
      buzz();
    }
  }

  return value;
}



void forward( int spd ){
  motors.setSpeeds( spd, spd );
  ledRed(false);
  ledGreen(true);
}

void stop(){
  motors.setSpeeds(0, 0);
  ledRed(true);
  ledGreen(false);
}

float getDistance(){
  int countsL = encoders.getCountsLeft();
  int countsR = encoders.getCountsRight();

  float distanceL = countsL/909.7 * wheelCirc;
  float distanceR = countsR/909.7 * wheelCirc;

  return (distanceL + distanceR)/2;
}

void resetEncoders(){
  encoders.getCountsAndResetLeft();
  encoders.getCountsAndResetRight();
}

void countdown(){
  for (int time = 5; time > 0; time--){
    buzz();
    OLED.clear();
    OLED.print(time);
    delay(1000);
  }
  buzzer.playNote(330, 200, 10);
  OLED.clear();
}

void readLineSensors(){
  lineSensors.read(lineSensorValues, QTR_EMITTERS_ON);
}

void printValue( int value ){
  OLED.clear();
  OLED.print(value);
}

void buzz(){
  buzzer.playNote(300, 100, 10);
}

