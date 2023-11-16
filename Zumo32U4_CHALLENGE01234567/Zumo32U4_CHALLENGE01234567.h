#include "Zumo32U4_components.h"

/*! This struct contains all 8 challenges from 0 to 7. */
struct challenge {

  /*! CHALLENGE ZERO.
   *  You shall program the Zumo to have a programmable interface using 
   *  - the encoders.
   *  - the LCD/OLED screen.
   *  - the buttons.
   *  so that you can launch set solutions with set parameters. */
  void zero() {
    /* Move&rotate Matthias function */
    buttons.pressAnyKeyToContinue();
    buttons.getProtocol();
    buzzer.intermission();
  }

  void alignZumo(String side){
    int threshold = 600;
    int speed = 100;
    bool run = true;
    bool leftReachedTape = false, leftOverTape = false, leftDone = false;
    bool rightReachedTape = false, rightOverTape = false, rightDone = false;

    delay(500);
    motors.forward(speed/2);

    while(run){
      lineSensors.read();
      if(side == "left" && lineSensors.value[2] >= threshold){
        leftReachedTape = true;
        rightReachedTape = true;
        motors.stop();
        run = false;
      }
      else if (side == "right" && lineSensors.value[0] >= threshold){
        leftReachedTape = true;
        rightReachedTape = true;
        motors.stop();
        run = false;
      }
    }

    delay(1000);
    run = true;
    motors.forward(speed);

    while (run){
      lineSensors.read();

      if (!leftDone || !rightDone){   //  Runs if and only if one of the sides is not aligned.
        //  Checks left side
        if (leftReachedTape && lineSensors.value[0] < threshold){      //  Goes back if over tape.
          MOTORS.setLeftSpeed(-speed);
          leftOverTape = true;
          leftDone = false;
        }
        else if(leftOverTape && lineSensors.value[0] >= threshold){  //  Only stops if it have been over the tape once.
          MOTORS.setLeftSpeed(0);
          leftDone = true;
        }

        //  Checks right side
        if (rightReachedTape && lineSensors.value[2] < threshold){     //  Goes back if over tape.
          MOTORS.setRightSpeed(-speed);
          rightOverTape = true;
          rightDone = false;
        }
        else if (rightOverTape && lineSensors.value[2] >= threshold){  //  Only stops if it have been over the tape once.
          MOTORS.setRightSpeed(0);
          rightDone = true;
        }
      }
      else {
        run = false;
      }    //  Stops aligning the Zumo when both sides have stopped.
    }


  }
float getDistance(){
  int wheelCirc = 13;
  int countsL = ENCODERS.getCountsLeft();
  int countsR = ENCODERS.getCountsRight();

  float distanceL = countsL/900.0 * wheelCirc;
  float distanceR = countsR/900.0 * wheelCirc;

  return (distanceL + distanceR)/2;
}
void resetEncoders(){
  ENCODERS.getCountsAndResetLeft();
  ENCODERS.getCountsAndResetRight();
}
void stopAtLength() {
  delay(1000);
  int movementParameter = 35;
  int maxDistance = 41;
  int speed = 100;
  bool lastRun = true;
  float num = maxDistance - movementParameter;

  resetEncoders();

  motors.forward(speed);

  while (lastRun){
    if (getDistance() >= num){
      motors.stop();
    }
  }

}

  /*! CHALLENGE ONE.
   *  The Zumo shall be able to.
   *  1. Drive to the white line.
   *  2. Align itself.
   *  3. Then drive.
   *  4. untill it is at a specific distance...
   *     ...to the back wall at the end of the hallway. */
  void one() {
    int threshold = 600;
    int speed = 100;
    String side = "";
    bool tapeReached = false;

    lineSensors.read();

    if (lineSensors.value[0] < threshold && lineSensors.value[2] < threshold) {
      motors.forward(speed);
    }
    else if (lineSensors.value[0] >= threshold && !tapeReached){
      motors.stop();
      tapeReached = true;
      side = "left";
    }
    else if (lineSensors.value[2] >= threshold && !tapeReached){
      motors.stop();
      tapeReached = true;
      side = "right";
    }
    
    if (tapeReached){
      alignZumo(side);
      //  Drive a certain amount of length.
      stopAtLength();
    }
    

    delay(100);
  }

  /*! CHALLENGE TWO.
   *  The Zumo starts on one end of the whiteline. 
   *  The Zumo shall be able to follow the line all the way to the other end...
   *  ...without skipping major sections. */
  void two() {
    int speed = 400;
    int threshold = 600;
    while (true) {
      lineSensors.read();
      if (lineSensors.value[0]>=threshold){ motors.turn(-speed); } //same as turning left
      else if (lineSensors.value[4]>=threshold){ motors.turn(speed); } //same as turning right
      else { motors.forward(speed); }
    }
  }

  /*! CHALLENGE THREE.
   *  The Zumo shall.
   *  1. Drive in between the walls with equal distance to the walls at each side.
   *  2. Untill detection of the white line at the far end of the challenge. */
  void three() {
    for (int i = 0; i < proxSensors.levelTwo; i++) { proxSensors.brightnessTwo[i] = i; }
    PROXSENSORS.setBrightnessLevels(proxSensors.brightnessTwo, proxSensors.levelTwo);
    do {
      proxSensors.read();
      lineSensors.read();
      MOTORS.setSpeeds(150-30*(proxSensors.value[5]-(proxSensors.value[0]-0.2))-15*(proxSensors.value[3]-proxSensors.value[2]),
                       150+30*(proxSensors.value[5]-(proxSensors.value[0]-0.2))+15*(proxSensors.value[3]-proxSensors.value[2]));
      delay(5);
      Serial.println((String)proxSensors.value[0] + "\t" + proxSensors.value[2] + " " + proxSensors.value[3] + "\t" + proxSensors.value[5]);
    } while(lineSensors.value[0]<lineSensors.threshold && lineSensors.value[2]<lineSensors.threshold && lineSensors.value[4]<lineSensors.threshold);
    motors.stop();
    buttons.protocol = 255; // CHALLGENE TWO COMPLETE. Go to protocol 255.
  }

  /*! CHALLENGE FOUR.
   *  The Zumo shall. 
   *  1. Be able to drive to the whiteline. 
   *  2. Align it self.
   *  3. Drive a set distance straight forward. */
  void four() {
    int alignStage = 0;
    int sideStage = 0;
    int parameter = 25;
    switch (alignStage) {
    case 0:
      lineSensors.read();
      motors.forward();
      if (lineSensors.value[0] > lineSensors.threshold || lineSensors.value[4] > lineSensors.threshold) {
        if (lineSensors.value[0] > lineSensors.threshold)
          sideStage = 0;
        if (lineSensors.value[4] > lineSensors.threshold) {
          sideStage = 1;
        }
        alignStage++;
      }
      break;
    case 1:
      switch (sideStage) {
        case 0:
          encoders.reset();
          while (lineSensors.value[4] < lineSensors.threshold) {
            lineSensors.read();
            motors.forward();
          }
          motors.stop();
          do {
            encoders.read();
            MOTORS.setSpeeds(-100, 0);
          } while (encoders.value[0] + 30 > 0);
          alignStage++;
          break;
        case 1:
          encoders.reset();
          do {
            lineSensors.read();
            motors.forward();
          } while (lineSensors.value[0] < lineSensors.threshold);
          motors.stop();
          do {
            encoders.read();
            MOTORS.setSpeeds(0, -100);
          } while (encoders.value[0] + 30 > 0);
          encoders.reset();
          alignStage++;
          break;
      }
      //alt før her er align
    case 2:
      while (encoders.getDistance() < parameter) { motors.forward(); }
      motors.stop();
  }
  }

  /*! CHALLGEN FIVE.
   *  The Zumo shall 
   *  1. Drive slalom between the cylinders.
   *  2. Whilst using the white lines to count successfull passes from line to line on the display. 
   *  You are challenged to get as many successfull passes as possible in a set amountof time. */
  void five() {
    for (int i = 0; i < proxSensors.levelFive; i++) { proxSensors.brightnessFive[i] = 2 * i; }
    PROXSENSORS.setBrightnessLevels(proxSensors.brightnessFive, proxSensors.levelFive);
    bool eightFig = false;
    bool middlePoint = false;
    int count = 0;
    while(true) {
      lineSensors.read();
      proxSensors.read();
      if (proxSensors.value[2] + proxSensors.value[3] > 20) { middlePoint = true; }
      if (proxSensors.value[2] + proxSensors.value[3] <= 14 && middlePoint == true) {
        if (eightFig == false) {
          MOTORS.setSpeeds(100, 240);
          delay(600);
        }
        else {
          MOTORS.setSpeeds(100, -100);
          delay(300);
          MOTORS.setSpeeds(100, 100);
          delay(300);
        }
        eightFig = !eightFig;
        middlePoint = false;
      }
      if (eightFig == false) {
        MOTORS.setSpeeds(100 + 15 * (proxSensors.value[5] - proxSensors.value[0]), 100 - 15 * (proxSensors.value[5] - proxSensors.value[0]));
      }
      else {
        MOTORS.setSpeeds(100 - 20 * (proxSensors.value[5] - 13), 100 + 20 * (proxSensors.value[5] - 13));
      }
      if (lineSensors.value[4] > 400) { 
        count++;
        while (lineSensors.value[4] > 400) { lineSensors.read(); }
      }
      screen.reRender();
      screen.show((String)proxSensors.value[2] + "\t" + proxSensors.value[3]);
      screen.line = 1;
      screen.show((String)count);
      Serial.println((String)lineSensors.value[0] + "\t" +  lineSensors.value[2] + "\t" +  lineSensors.value[4]); // OPTIONAL
    }
  }

  /*! CHALLENGE SIX.
   *  The Zumo shall 
   *  1. Be able to drive to the whiteline.
   *  2. Align it self.
   *  3. Turn the a set amount of degrees
   *  4. Before driving forward untill detecting the other white line. */
  void six() {
    
  }

 /*! CHALLENGE SEVEN.
  *  The Zumo shall.
  *  1. Be able to drive to the whiteline.
  *  2. Align it self 
  *  3. Drive straight. 
  *  4. Until hitting the whiteline at the other end of the board. */
  void seven() {
    int alignStage = 0, sideStage = 0;
    imu.calibrateTurn(1024);
    while (true) {
      switch (alignStage){
        case 0:
          lineSensors.read();
          motors.forward();
          if (lineSensors.value[0] > lineSensors.threshold || lineSensors.value[4] > lineSensors.threshold){
            if (lineSensors.value[0] > lineSensors.threshold) { sideStage = 0; }
            if (lineSensors.value[4] > lineSensors.threshold) { sideStage = 1; }
            alignStage++;
          }
          break;
        case 1:
          switch (sideStage){
            case 0:
              encoders.reset();
              while (lineSensors.value[4] < lineSensors.threshold){
                lineSensors.read();
                motors.forward();
              }
              motors.stop();
              do {
                encoders.read();
                motors.left();
              } while (encoders.value[0]+30 > 0);
              alignStage++;
              break;
            case 1:
              encoders.reset();
              while (lineSensors.value[0] < lineSensors.threshold){
                lineSensors.read();
                motors.forward();
              }
              motors.stop();
              do {
                encoders.read();
                motors.right();
              } while (encoders.value[1]+30 > 0);
              alignStage++;
              break;
          }    
        case 2:
          motors.stop();
          imu.reset();
          encoders.reset();
          motors.forward();
          delay(100);
          alignStage++; 
        case 3:
          lineSensors.read();
          if (lineSensors.value[0] > lineSensors.threshold || lineSensors.value[2] > lineSensors.threshold || lineSensors.value[4] > lineSensors.threshold) { motors.stop(); }
          else{
            screen.reRender();
            screen.show((String)imu.zumoAngle);
            imu.dAngle();
            MOTORS.setSpeeds(200+imu.zumoAngle,200-imu.zumoAngle);
          }
      } 
    }
  }
} challenge;
