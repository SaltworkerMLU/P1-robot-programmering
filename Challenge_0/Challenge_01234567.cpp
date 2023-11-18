#include "Challenge_01234567.h"

bool zumo::aboveThreshold() {
  lineSensors::read();
  if (lineSensors::value[0]>threshold || 
      lineSensors::value[2]>threshold || 
      lineSensors::value[4]>threshold) { return true; }
  else { return false; }
}

void zumo::align() {
  if (lineSensors::value[0] >= threshold && !leftReachedTape){ // Aligns from the left side.
    leftReachedTape = true;
    motors::stop();
    checkOver();
  } 
  else if (lineSensors::value[2] >= threshold && !rightReachedTape){ // Aligns from the right side
    rightReachedTape = true;
    motors::stop();
    checkOver();
  }
}

void zumo::checkOver() {
  delay(500);
  motors::forward(speed/2);

  bool run = true;

  while(run){
    lineSensors::read();
    if(leftReachedTape && lineSensors::value[2] >= threshold){
      motors::stop();
      rightReachedTape = true;
      run = false;
    } else if (rightReachedTape && lineSensors::value[0] >= threshold){
      motors::stop();
      leftReachedTape = true;
      run = false;
    }
  }

  delay(1000);

  run = true;

  motors::forward(speed);

  while (run){
    lineSensors::read();

    if (!leftDone || !rightDone){   //  Runs if and only if one of the sides is not aligned.
      //  Checks left side
      if (leftReachedTape && lineSensors::value[0] < threshold){      //  Goes back if over tape.
        motors::setLeftSpeed(-speed);
        leftOverTape = true;
        leftDone = false;
      } 
      else if(leftOverTape && lineSensors::value[0] >= threshold){  //  Only stops if it have been over the tape once.
        motors::setLeftSpeed(0);
        leftDone = true;
      }

      //  Checks right side
      if (rightReachedTape && lineSensors::value[2] < threshold){     //  Goes back if over tape.
        motors::setRightSpeed(-speed);
        rightOverTape = true;
        rightDone = false;
      } 
      else if (rightOverTape && lineSensors::value[2] >= threshold){  //  Only stops if it have been over the tape once.
        motors::setRightSpeed(0);
        rightDone = true;
      }
    } 
    else { run = false; }   //  Stops aligning the Zumo when both sides have stopped.
  }
}

void zumo::turnByDegree(int degree){
  imu::reset();
  imu::dAngle();
  if (degree<0){
    while (degree<imu::zumoAngle){
      motors::turn(speed);
      imu::dAngle();
    }
  }
  if (parameter>0){
    while (parameter>imu::zumoAngle){
      motors::turn(-speed);
      imu::dAngle();
    }
  }
  motors::stop();
}

int challenge::zero() {
  /* Move&rotate Matthias function */
  display::clear();
  display::print("A=+; B=-");
  display::gotoXY(0, 1);
  display::print("C=onfirm");
  buttons::pressAnyKeyToContinue();
  display::clear();
  while (buttons::getProtocol() == false) {
    display::print((String)buttons::protocol);
    display::gotoXY(0, 0);
  }
  intermission();
  return buttons::protocol;
}

void challenge::alignZumo(String side) {
  int threshold = 600;
  int speed = 100;
  bool run = true;
  bool leftReachedTape = false, leftOverTape = false, leftDone = false;
  bool rightReachedTape = false, rightOverTape = false, rightDone = false;

  delay(500);
  motors::forward(speed/2);

  while(run){
    lineSensors::read();
    if(side == "left" && lineSensors::value[2] >= threshold){
      leftReachedTape = true;
      rightReachedTape = true;
      motors::stop();
      run = false;
    }
    else if (side == "right" && lineSensors::value[0] >= threshold){
      leftReachedTape = true;
      rightReachedTape = true;
      motors::stop();
      run = false;
    }
  }

  delay(1000);
  run = true;
  motors::forward(speed);

  while (run){
    lineSensors::read();

    if (!leftDone || !rightDone){   //  Runs if and only if one of the sides is not aligned.
      //  Checks left side
      if (leftReachedTape && lineSensors::value[0] < threshold){      //  Goes back if over tape.
        motors::setLeftSpeed(-speed);
        leftOverTape = true;
        leftDone = false;
      } 
      else if(leftOverTape && lineSensors::value[0] >= threshold){  //  Only stops if it have been over the tape once.
        motors::setLeftSpeed(0);
        leftDone = true;
      }

      //  Checks right side
      if (rightReachedTape && lineSensors::value[2] < threshold){     //  Goes back if over tape.
        motors::setRightSpeed(-speed);
        rightOverTape = true;
        rightDone = false;
      }
      else if (rightOverTape && lineSensors::value[2] >= threshold){  //  Only stops if it have been over the tape once.
        motors::setRightSpeed(0);
        rightDone = true;
      }
    }
    else {
      run = false;
    }    //  Stops aligning the Zumo when both sides have stopped.
  }
}

void challenge::one() {
  int threshold = 600;
  int speed = 100;
  int movementParameter = 11;
  int maxDistance = 41;
  String side = "";
  bool tapeReached = false;

  lineSensors::read();

  if (lineSensors::value[0] < threshold && lineSensors::value[2] < threshold) {
    motors::forward(speed);
  }
  else if (lineSensors::value[0] >= threshold && !tapeReached){
    motors::stop();
    tapeReached = true;
    side = "left";
  }
  else if (lineSensors::value[2] >= threshold && !tapeReached){
    motors::stop();
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

void challenge::two() {
  int speed = 400;
  int threshold = 600;
  while (true) {
    lineSensors::read();
    if (lineSensors::value[0]>=threshold){ motors::turn(-speed); } //same as turning left
    else if (lineSensors::value[4]>=threshold){ motors::turn(speed); } //same as turning right
    else { motors::forward(speed); }
  }
}

void challenge::three() {
  for (int i = 0; i < proxSensors::levelTwo; i++) { proxSensors::brightnessTwo[i] = i; }
  proxSensors::setBrightnessLevels(proxSensors::brightnessTwo, proxSensors::levelTwo);
  do {
    proxSensors::read();
    lineSensors::read();
    motors::setSpeeds(150-30*(proxSensors::value[5]-(proxSensors::value[0]-0.2))-15*(proxSensors::value[3]-proxSensors::value[2]),
                     150+30*(proxSensors::value[5]-(proxSensors::value[0]-0.2))+15*(proxSensors::value[3]-proxSensors::value[2]));
    delay(5);
    Serial.println((String)proxSensors::value[0] + "\t" + proxSensors::value[2] + " " + proxSensors::value[3] + "\t" + proxSensors::value[5]);
  } while(lineSensors::value[0]<lineSensors::threshold && lineSensors::value[2]<lineSensors::threshold && lineSensors::value[4]<lineSensors::threshold);
  motors::stop();
  buttons::protocol = 255; // CHALLGENE TWO COMPLETE. Go to protocol 255.
}

void challenge::four() {
  int alignStage = 0;
  int sideStage = 0;
  int parameter = 25;
  switch (alignStage) {
    case 0:
      lineSensors::read();
      motors::forward();
      if (lineSensors::value[0] > lineSensors::threshold || lineSensors::value[4] > lineSensors::threshold) {
        if (lineSensors::value[0] > lineSensors::threshold)
          sideStage = 0;
        if (lineSensors::value[4] > lineSensors::threshold) {
          sideStage = 1;
        }
        alignStage++;
      }
      break;
    case 1:
      switch (sideStage) {
        case 0:
          encoders::reset();
          while (lineSensors::value[4] < lineSensors::threshold) {
            lineSensors::read();
            motors::forward();
          }
          motors::stop();
          do {
            encoders::read();
            motors::setSpeeds(-100, 0);
          } while (encoders::value[0] + 30 > 0);
          alignStage++;
          break;
        case 1:
          encoders::reset();
          do {
            lineSensors::read();
            motors::forward();
          } while (lineSensors::value[0] < lineSensors::threshold);
          motors::stop();
          do {
            encoders::read();
            motors::setSpeeds(0, -100);
          } while (encoders::value[0] + 30 > 0);
          encoders::reset();
          alignStage++;
          break;
      }
      //alt før her er align
    case 2:
      while (encoders::getDistance() < parameter) { motors::forward(); }
      motors::stop();
  }
}

void challenge::five() {
  for (int i = 0; i < proxSensors::levelFive; i++) { proxSensors::brightnessFive[i] = 2 * i; }
  proxSensors::setBrightnessLevels(proxSensors::brightnessFive, proxSensors::levelFive);
  bool eightFig = false;
  bool middlePoint = false;
  int count = 0;
  while(true) {
    lineSensors::read();
    proxSensors::read();
    if (proxSensors::value[2] + proxSensors::value[3] > 20) { middlePoint = true; }
    if (proxSensors::value[2] + proxSensors::value[3] <= 14 && middlePoint == true) {
      if (eightFig == false) {
        motors::setSpeeds(100, 240);
        delay(600);
      }
      else {
        motors::setSpeeds(100, -100);
        delay(300);
        motors::setSpeeds(100, 100);
        delay(300);
      }
      eightFig = !eightFig;
      middlePoint = false;
    }
    if (eightFig == false) {
      motors::setSpeeds(100 + 15 * (proxSensors::value[5] - proxSensors::value[0]), 100 - 15 * (proxSensors::value[5] - proxSensors::value[0]));
    }
    else {
      motors::setSpeeds(100 - 20 * (proxSensors::value[5] - 13), 100 + 20 * (proxSensors::value[5] - 13));
    }
    if (lineSensors::value[4] > 400) { 
      count++;
      while (lineSensors::value[4] > 400) { lineSensors::read(); }
    }
    display::clear();
    display::print((String)proxSensors::value[2] + "\t" + proxSensors::value[3]);
    display::gotoXY(0, 1);
    display::print((String)count);
    Serial.println((String)lineSensors::value[0] + "\t" +  lineSensors::value[2] + "\t" +  lineSensors::value[4]); // OPTIONAL
  }
}

void challenge::six() {
  imu::calibrateTurn(1024);
  while (true) {
    lineSensors::read();
    if(!zumo::alignment && !zumo::aboveThreshold()){ motors::forward(zumo::speed); }
    if(!zumo::alignment && zumo::aboveThreshold()){
      display::print("STOP!");
      display::gotoXY(0, 0);
      motors::stop();
      zumo::align();
      zumo::alignment=true;
      delay(2000);
      zumo::turnByDegree(zumo::parameter);
      motors::forward(zumo::speed);
      delay(2000);
    }
    if(zumo::alignment && zumo::aboveThreshold()) { motors::stop(); }
  }
}

void challenge::seven() {
  int alignStage = 0, sideStage = 0;
  imu::calibrateTurn(1024);
  while (true) {
    switch (alignStage){
      case 0:
        lineSensors::read();
        motors::forward();
        if (lineSensors::value[0] > lineSensors::threshold || lineSensors::value[4] > lineSensors::threshold){
          if (lineSensors::value[0] > lineSensors::threshold) { sideStage = 0; }
          if (lineSensors::value[4] > lineSensors::threshold) { sideStage = 1; }
          alignStage++;
        }
        break;
      case 1:
        switch (sideStage){
          case 0:
            encoders::reset();
            while (lineSensors::value[4] < lineSensors::threshold){
              lineSensors::read();
              motors::forward();
            }
            motors::stop();
            do {
              encoders::read();
              motors::left();
            } while (encoders::value[0]+30 > 0);
            alignStage++;
            break;
          case 1:
            encoders::reset();
            while (lineSensors::value[0] < lineSensors::threshold){
              lineSensors::read();
              motors::forward();
            }
            motors::stop();
            do {
              encoders::read();
              motors::right();
            } while (encoders::value[1]+30 > 0);
            alignStage++;
            break;
        }    
      case 2:
        motors::stop();
        imu::reset();
        encoders::reset();
        motors::forward();
        delay(100);
        alignStage++; 
      case 3:
        lineSensors::read();
        if (lineSensors::value[0] > lineSensors::threshold || lineSensors::value[2] > lineSensors::threshold || lineSensors::value[4] > lineSensors::threshold) { motors::stop(); }
        else{
          display::clear();
          display::print((String)imu::zumoAngle);
          imu::dAngle();
          motors::setSpeeds(200+imu::zumoAngle,200-imu::zumoAngle);
        }
    } 
  }
}

void challenge::intermission(uint8_t attention=10, int windup=800) {
  display::clear();
  display::print("GOGOGO!");
  ledRed(1);    // Turn on red LED
  buzzer::playFrequency(400, windup/2, attention); // 400Hz sound, ms/2, 15=max volume & 0 = min volume
  delay(windup/2);                                // Prevent the prior function from being overwritten
  ledYellow(1); // Turn on yellow LED
  buzzer::playFrequency(800, windup/2, attention); // 800Hz sound
  delay(windup/2);                                // Prevent the prior function from being overwritten
  ledRed(0);    // Turn off red LED
  ledYellow(0); // Turn off yellow LED
  ledGreen(1);  // Turn on green LED. No need to turn it off. Green LED lights up when Serial monitor active
  buzzer::playFrequency(1200, windup, attention);  // 1200Hz lyd
  delayMicroseconds(1); // After 1us, execute next line in the program. Without the delay, the buzzer will play forever.
  display::clear();
}