#include "Zumo32U4_components.h"

buttons buttons;
encoders encoders;
imu imu;
lineSensors lineSensors;
proxSensors proxSensors;
motors motors;

struct alignment {
  int speed = 150;
  int threshold = 600;
  bool alignment = false;
  int parameter = 45; //angle in degrees
  bool leftReachedTape = false, leftOverTape = false, leftDone = false; //booleans necessary for the align function
  bool rightReachedTape = false, rightOverTape = false, rightDone = false; //booleans necessary for the align function

  bool aboveThreshold(){
    lineSensors.read();
    if (lineSensors.value[0]>threshold || lineSensors.value[2]>threshold || lineSensors.value[4]>threshold) { 
      return true; 
    } else { return false; }
  }

  /*! Align function.
   *  As it is heavily used, it will be a reserved function for this struct. */
   void align() {
    if (lineSensors.value[0] >= threshold && !leftReachedTape){         //  Aligns from the left side.
      leftReachedTape = true;
      motors.stop();
      checkOver();
    } 
    else if (lineSensors.value[2] >= threshold && !rightReachedTape){        //  Aligns from the right side
      rightReachedTape = true;
      motors.stop();
      checkOver();
    }
  }

  void checkOver(){
    delay(500);
    motors.forward(speed/2);

    bool run = true;

    while(run){
      lineSensors.read();
      if(leftReachedTape && lineSensors.value[2] >= threshold){
        motors.stop();
        rightReachedTape = true;
        run = false;
      } else if (rightReachedTape && lineSensors.value[0] >= threshold){
        motors.stop();
        leftReachedTape = true;
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
      else { run = false; }   //  Stops aligning the Zumo when both sides have stopped.
    }
  }
  void turnByDegree(int degree){
    imu.reset();
    imu.dAngle();
    if (degree<0){
      while (degree<imu.zumoAngle){
        motors.turn(speed);
        imu.dAngle();
      }
    }
    if (parameter>0){
      while (parameter>imu.zumoAngle){
        motors.turn(-speed);
        imu.dAngle();
      }
    }
    motors.stop();
  }
} alignment;

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
    intermission();
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
    int movementParameter = 11;
    int maxDistance = 41;
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
    else if (tapeReached){
      alignZumo(side);
      while (1){
        
      }
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
      SCREEN.clear();
      SCREEN.print((String)proxSensors.value[2] + "\t" + proxSensors.value[3]);
      SCREEN.gotoXY(0, 1);
      SCREEN.print((String)count);
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
    imu.calibrateTurn(1024);
    while (true) {
      lineSensors.read();
      if(!alignment.alignment && !alignment.aboveThreshold()){ motors.forward(alignment.speed); }
      if(!alignment.alignment && alignment.aboveThreshold()){
        SCREEN.print("STOP!");
        SCREEN.gotoXY(0, 0);
        motors.stop();
        alignment.align();
        alignment.alignment=true;
        delay(2000);
        alignment.turnByDegree(alignment.parameter);
        motors.forward(alignment.speed);
        delay(2000);
      }
      if(alignment.alignment && alignment.aboveThreshold()) { motors.stop(); }
    }
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
            SCREEN.clear();
            SCREEN.print((String)imu.zumoAngle);
            imu.dAngle();
            MOTORS.setSpeeds(200+imu.zumoAngle,200-imu.zumoAngle);
          }
      } 
    }
  }

  /*! Indicates challenge start and finish. */
  void intermission(uint8_t attention=10, int windup=800) {
    ledRed(1);    // Turn on red LED
    BUZZER.playFrequency(400, windup/2, attention); // 400Hz sound, ms/2, 15=max volume & 0 = min volume
    delay(windup/2);                                // Prevent the prior function from being overwritten
    ledYellow(1); // Turn on yellow LED
    BUZZER.playFrequency(800, windup/2, attention); // 800Hz sound
    delay(windup/2);                                // Prevent the prior function from being overwritten
    ledRed(0);    // Turn off red LED
    ledYellow(0); // Turn off yellow LED
    ledGreen(1);  // Turn on green LED. No need to turn it off. Green LED lights up when Serial monitor active
    BUZZER.playFrequency(1200, windup, attention);  // 1200Hz lyd
    delayMicroseconds(1); // After 1us, execute next line in the program. Without the delay, the buzzer will play forever.
  }
} challenge;
