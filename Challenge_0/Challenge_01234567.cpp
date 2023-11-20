#include "Challenge_01234567.h"

bool zumo::aboveThreshold() {
  lineSensors::read();
  if (lineSensors::value[0]>threshold || 
      lineSensors::value[2]>threshold || 
      lineSensors::value[4]>threshold) { return true; }
  else { return false; }
}

void zumo::alignZumo(){
    run = true, miniRun = true;
    leftReachedTape = false, leftOverTape = false, leftDone = false;
    rightReachedTape = false, rightOverTape = false, rightDone = false;

    delay(500);

    encoders::reset();

    while(run){
      lineSensors::read();

      if(encoders::getDistance() >= 4 && side == "right"){
        encoders::reset();
        delay(100);
        motors::forward(-speed);
        while(miniRun){
          if(encoders::getDistance() <= -4){
            motors::stop();
            delay(200);
            motors::right();
            delay(800);
            motors::stop();
            miniRun = false;
            side = "left";
          }
        }
      }
      else if(encoders::getDistance() >= 4 && side == "left"){
        encoders::reset();
        delay(100);
        motors::forward(-speed);
        while(miniRun){
          if(encoders::getDistance() <= -4){
            motors::stop();
            delay(200);
            motors::left();
            delay(600);
            motors::stop();
            miniRun = false;
            side = "right";
          }
        }
      }
      else if(side == "left" && lineSensors::value[4] >= lineSensors::threshold){
        motors::stop();
        run = false;
      }
      else if (side == "right" && lineSensors::value[0] >= lineSensors::threshold){
        motors::stop();
        run = false;
      }
      else {
        motors::forward(speed/2);
      }
    }

    delay(1000);

    leftReachedTape = true;
    rightReachedTape = true;
    run = true;

    motors::forward(speed);

    while (run){
      lineSensors::read();

      if (!leftDone || !rightDone){   //  Runs if and only if one of the sides is not aligned.
        //  Checks left side
        if (leftReachedTape && lineSensors::value[0] < lineSensors::threshold){      //  Goes back if over tape.
          Zumo32U4Motors::setLeftSpeed(-speed);
          leftOverTape = true;
          leftDone = false;
        }
        else if(leftOverTape && lineSensors::value[0] >= lineSensors::threshold){  //  Only stops if it have been over the tape once.
          Zumo32U4Motors::setLeftSpeed(0);
          leftDone = true;
        }

        //  Checks right side
        if (rightReachedTape && lineSensors::value[4] < lineSensors::threshold){     //  Goes back if over tape.
          Zumo32U4Motors::setRightSpeed(-speed);
          rightOverTape = true;
          rightDone = false;
        }
        else if (rightOverTape && lineSensors::value[4] >= lineSensors::threshold){  //  Only stops if it have been over the tape once.
          Zumo32U4Motors::setRightSpeed(0);
          rightDone = true;
        }
      }
      else {
        run = false;
      }    //  Stops aligning the Zumo when both sides have stopped.
    }


  }

void zumo::align() {
  motors::forward(speed); // Start of by setting the Zumo to drive forward
  while (1) {
    lineSensors::read();

    if (lineSensors::value[0] < threshold && lineSensors::value[4] < threshold) {
      motors::forward(speed);
    }
    else if (lineSensors::value[0] >= threshold){
      motors::stop();
      side = "left";
      break;
    }
    else if (lineSensors::value[4] >= threshold){
      motors::stop();
      side = "right";
      break;
    }
  }
  motors::stop();
  zumo::alignZumo();
}

void zumo::stopAtLength() {
  delay(1000);
  int movementParameter = 35;
  int maxDistance = 41;
  int speed = 200;
  bool lastRun = true;
  float num = maxDistance - movementParameter;

  encoders::reset();

  motors::forward(speed);

  while (lastRun){
    if (encoders::getDistance() >= num){
      motors::stop();
    }
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

void challenge::one() {
  int distance = 20;
  zumo::align();
  delay(100);
  motors::forward();
  encoders::reset();
  while (encoders::getDistance() < distance) {}
  motors::stop();
}

void challenge::two() {
  int speed = 200;
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
  int distance = 20;
  zumo::align();
  delay(100);
  motors::forward();
  encoders::reset();
  while (encoders::getDistance() < distance) {}
  motors::stop();
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
  int degrees = 100;
  imu::calibrateTurn(1024);
  zumo::align();
  motors::turn(100);
   while (!aboveThreshold()) {}
  motors::stop();
}

void challenge::seven() {
  imu::calibrateTurn(1024);
  zumo::align();
  motors::forward(100);
  delay(1000);
  while (!aboveThreshold()) {}
  motors::stop();
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