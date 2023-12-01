#include "Challenge_01234567.h"

bool zumo::aboveThreshold() {
  lineSensors::read();
  if (lineSensors::value[0]>threshold || 
      lineSensors::value[1]>threshold || 
      lineSensors::value[2]>threshold) { return true; }
  return false; // Else-statement not needed as return terminates function
}

void zumo::align(){
    bool side; // 0 = left; 1 = right

    motors::forward(speed); // Start off by setting the Zumo to drive forward...
    while (!aboveThreshold()) {} // ... until Zumo crosses a line.

    if (lineSensors::value[0] >= threshold) { side = 0; } // Zumo has left side above line
    else if (lineSensors::value[2] >= threshold) { side = 1; } // Zumo has right side above line

    while(lineSensors::value[2*!side] < threshold) { lineSensors::read(); } // While the other line sensor does not see the line...
    while(aboveThreshold()) { lineSensors::read(); } // While either line sensor sees the line...
    motors::forward(-speed); // Pull the brakes. We're going back to align that shit

    while (lineSensors::value[2] < threshold ||
           lineSensors::value[0] < threshold){
      lineSensors::read();

      if(lineSensors::value[0] >= threshold) { Zumo32U4Motors::setLeftSpeed(0); }
      else { Zumo32U4Motors::setLeftSpeed(-speed); } // Aligning left side

      if (lineSensors::value[2] >= threshold) { Zumo32U4Motors::setRightSpeed(0); }
      else { Zumo32U4Motors::setRightSpeed(-speed); } // Aligning right side
    }
}

int challenge::zero() {
  display::clear();
  display::print("A=+; B=-");
  display::gotoXY(0, 1);
  display::print("C=onfirm");
  while (buttons::pressAnyKeyToContinue()) {}
  display::clear();
  while (!buttons::getProtocol()) { // While protocol not acquired
    display::print((String)buttons::protocol);
    display::gotoXY(0, 0);
  }
  intermission();
  return buttons::protocol;
}

void challenge::one() {
  zumo::align();
  encoders::reset();
  motors::forward();
  while (encoders::readDistance() < zumo::moveParameter) {}
  motors::stop();
}

void challenge::two() {
  speed = 200;
  while (imu::failsafe()) { // Turn the Zumo upside down to end challenge
    lineSensors::read();
    if (lineSensors::value[0]>=threshold){ 
      motors::right(speed);
      delay(95);
    } 
    else if (lineSensors::value[2]>=threshold){ 
      motors::left(speed);
      delay(95);
    }
    else { motors::forward(speed); }
  }
  motors::stop();
  speed = 100;
}

void challenge::three() {
  do {
    proxSensors::read();
    lineSensors::read();
    Zumo32U4Motors::setSpeeds(150-30*(proxSensors::value[5]-(proxSensors::value[0]-0.2))-15*(proxSensors::value[3]-proxSensors::value[2]),
                     150+30*(proxSensors::value[5]-(proxSensors::value[0]-0.2))+15*(proxSensors::value[3]-proxSensors::value[2]));
    delay(5);
  } while(lineSensors::value[0]<zumo::threshold && lineSensors::value[1]<zumo::threshold && lineSensors::value[2]<zumo::threshold);
  motors::stop();
}

void challenge::four() {
  zumo::align();
  encoders::reset();
  motors::forward();
  while (encoders::readDistance() < zumo::moveParameter) {} // reads in centimeters cm
  motors::stop();
}

void challenge::five() {
  bool eightFig = false;
  bool middlePoint = false;
  int count = 0;
  while(imu::failsafe()) {
    lineSensors::read();
    proxSensors::read();
    if (proxSensors::value[2] + proxSensors::value[3] > 28) { middlePoint = true; }
    if (proxSensors::value[2] + proxSensors::value[3] <= 25 && middlePoint == true) {
      if (eightFig == false) {
        Zumo32U4Motors::setSpeeds(100, 240);
        delay(600);
      }
      else {
        motors::left(100);
        delay(300);
        motors::forward(100);
        delay(700);
      }
      eightFig = !eightFig;
      middlePoint = false;
    }
    if (eightFig == false) { 
      speed = 150;
      Zumo32U4Motors::setSpeeds(speed + 15 * (proxSensors::value[5] - proxSensors::value[0]), speed - 15 * (proxSensors::value[5] - proxSensors::value[0])); 
    }
    else { 
      speed = 400;
      Zumo32U4Motors::setSpeeds(speed - 80 * (proxSensors::value[5] - 18), speed + 80 * (proxSensors::value[5] - 18)); 
      }
    if (lineSensors::value[2] > threshold) { 
      count++;
      while (lineSensors::value[2] > threshold-100) { lineSensors::read(); }
    }
    display::clear();
    display::print((String)proxSensors::value[2] + "\t" + proxSensors::value[3]);
    display::gotoXY(0, 1);
    display::print((String)count);
    Serial.println((String)lineSensors::value[0] + "\t" +  lineSensors::value[1] + "\t" +  lineSensors::value[2]); // OPTIONAL
  }
  motors::stop();
  speed = 100;
}

void challenge::six() {
  display::clear();
  display::print("Gyro cal");
  imu::calibrateTurn(1024); // Needed for any imu-related function
  display::clear();
  zumo::align();
  
  imu::reset();
  if (turnParameter < 0){ // Negative turnParameter
    motors::left(speed);
    while (turnParameter < imu::zumoAngle){ imu::dAngle(); } // while turnParameter negative
  }
  if (turnParameter >= 0){ // Positive turnParameter
    motors::right(speed);
    while (turnParameter > imu::zumoAngle){ imu::dAngle(); }  // while turnParameter positive
  }
  motors::stop();

  motors::setSpeeds(200+((180-getTurnAngleInDegrees())/9),200-((180-getTurnAngleInDegrees())/9));
  delay(1000); // Let the Zumo drive past the first line
  while (!zumo::aboveThreshold()) {}

  motors::stop();
}

void challenge::seven() {
  zumo::align();
  do{
    imu::dAngle();
    motors::setSpeeds(200+((180-imu::zumoAngle)/9),200-((180-imu::zumoAngle)/9));
    delay(100);
  } while (!abovethreshold);
  delay(1000); // Let the Zumo drive past the first line
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

void challenge::selectParameters() {
  imu::initMAG(); // Needed for any imu-related function
  display::clear();
  display::print("Any key=");
  display::gotoXY(0, 1);
  display::print("Continue");
  encoders::reset();
  while (pressAnyKeyToContinue() && encoders::value[0] == 0 && encoders::value[1] == 0) { encoders::read(); }
  while (pressAnyKeyToContinue()) {
            display::clear();
            display::print(" /\\  -> ");
            encoders::readDistance();
            display::gotoXY(0, 1);
            display::print((String)encoders::value[0]);
            display::gotoXY(4, 1);
            display::print((String)encoders::value[1]);
            delay(100);
         }
  moveParameter = encoders::value[0]; // maxDistance = 41
  turnParameter = encoders::value[1];
  encoders::reset();
}
