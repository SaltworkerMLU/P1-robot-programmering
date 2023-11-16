#include "Arduino.h" // Needed for any header file to run arduino code
#include <Wire.h>
#include <Zumo32U4.h>

Zumo32U4ButtonA buttonA;
Zumo32U4ButtonB buttonB;
Zumo32U4ButtonC buttonC;
Zumo32U4Buzzer BUZZER;
Zumo32U4LCD display;
Zumo32U4Encoders ENCODERS;
Zumo32U4IMU IMU;
Zumo32U4LineSensors lineFollowerSensors;
Zumo32U4Motors MOTORS;
Zumo32U4ProximitySensors proximitySensors;

/*! This struct makes use of the provided display. */
struct screen {
  uint8_t line = 0;

  /*! Prints {input} at displayLine. */
  void show(String input) {
  {
    display.gotoXY(0, line);
    display.print(input);
  }
}

  /*! Clears display and sets displayLine to be {firstLine}. */
  void reRender() { 
    display.clear();
    line = 0; 
  }
} screen;

struct buzzer {
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
} buzzer;

/*! This struct makes use of all 3 buttons. */
struct buttons {
  /*! The challenge which the Zumo must complete.
   *  To start of, protocol/challenge 0 is run. */
  uint8_t protocol = 0;

  /*! While neither button A, B, nor C has been pressed: Do nothing. */
  void pressAnyKeyToContinue() { 
    screen.show("A=+; B=-");
    screen.line = 1;
    screen.show("C=onfirm");
    while (!buttonA.isPressed() && !buttonB.isPressed() && !buttonC.isPressed()) {} 
    screen.reRender();
  }

  /*! Function to acquire variable protocol. Update protocol by pressing and releasing:
   *  * buttonA -> Add 1 to protocol.
   *  * buttonB -> Subtract 1 to protocol.
   *  * buttonC -> end function. 
   *  Whilst function is running, keep protocol in range 1-7 and show variable protocol on display. */
  void getProtocol() {
    while (!buttonC.getSingleDebouncedRelease()) {
      screen.show((String)protocol);
      if (buttonA.getSingleDebouncedRelease()) { protocol++; }
      if (buttonB.getSingleDebouncedRelease()) { protocol--; }
      if (protocol % 8 == 0) { protocol = -(protocol/8)*6 + 7; } // if (protocol == 0) { protocol = 7; } else if (protocol == 8) { protocol = 1; }
    }
    screen.reRender();
  }
} buttons;

struct lineSensors {
  /*! Line sensor values are stored here after lineSensors.read() has been called. */
  uint16_t value[5];

  /*! Stored threshold regarding black/white. */
  int threshold = 800;

  /*! CONSTRUCTOR. It executes as the very first thing when struct object called. */
  lineSensors() { lineFollowerSensors.initFiveSensors(); }

  /*! Reads line follower sensors and updates value[3]. */
  void read() { lineFollowerSensors.read(value, QTR_EMITTERS_ON); }

} lineSensors;

struct proxSensors {
  /*! Proximity sensor values are stored here after proxSensors.read() has been called. */
  uint8_t value[6];

  /*! Length of array brightness. Note how it is a "const".
   *  Because it is in a struct, "static" has to be put first. */
  static const uint8_t levelTwo = 20;

  /*! Array of brightness levels. */
  uint16_t brightnessTwo[levelTwo];

  /*! Length of array brightness. Note how it is a "const".
   *  Because it is in a struct, "static" has to be put first. */
  static const uint8_t levelFive = 13;

  /*! Array of brightness levels. */
  uint16_t brightnessFive[levelFive];

  /*! CONSTRUCTOR. It executes as the very first thing when struct object called. */
  proxSensors() {
    proximitySensors.initThreeSensors();
    proximitySensors.setPulseOffTimeUs(400);
    proximitySensors.setPulseOnTimeUs(300);
  }

  /*! Reads proximity sensors and updates value[6]. */
  void read() {
    proximitySensors.read();

    value[0] = proximitySensors.countsLeftWithLeftLeds();
    value[1] = proximitySensors.countsLeftWithRightLeds();

    value[2] = proximitySensors.countsFrontWithLeftLeds();
    value[3] = proximitySensors.countsFrontWithRightLeds();

    value[4] = proximitySensors.countsRightWithLeftLeds();
    value[5] = proximitySensors.countsRightWithRightLeds();
  }

  /*! Returns error of side sensors. */
  float getError() { return value[5]-(value[0]-0.2); }

  /*! Returns error of front sensors. */
  float getErrorFront() { return value[3]-value[2]; }
} proxSensors;

/*! This struct represents the Zumo32U4 IMU */
struct imu {
  int16_t* mag[3] = {&IMU.m.x, &IMU.m.y, &IMU.m.z};  // All IMU data is represented in the struct using...
  int16_t* acc[3] = {&IMU.a.x, &IMU.a.y, &IMU.a.z};  // ... the address of the specific dimensions in...
  int16_t* gyro[3] = {&IMU.g.x, &IMU.g.y, &IMU.g.z}; // ... format: x = [0]; y = [1]; z = [2]

  //These values are explicitly used for the functions calibrateTurn() & dAngle()
  int16_t offset; // Average reading using calibrateTurn()... an offset
  uint16_t lastUpdate; // Stores old time in microseconds using micros()
  uint32_t turnAngle = 0; // Turn angle which needs further conversion
  uint32_t zumoAngle = 0; // Zumo32U4 turn angle (THE ONE)

  /*! Initializing the IMU takes multiple commands, thus this singular function has been made */
  void initMAG() {
    Wire.begin();
    IMU.init();
    IMU.enableDefault();
    IMU.configureForTurnSensing();
  }

  /*! Takes measurements of gyrometer z-axis {iterations} amount of times. Then updates several struct variables */
  void calibrateTurn(int iterations=1000) {
    initMAG();
    screen.show("Gyro cal");
    delay(500);
    int32_t total = 0;
    for (int i = 0; i < iterations; i++) {
        while(!IMU.gyroDataReady()) {}
        IMU.readGyro();
        total += IMU.g.z;
    }
    lastUpdate = micros(); // Resets timer
    turnAngle = 0; // Resets angle
    zumoAngle = 0; // Resets Zumo32U4 angle (the important one)
    offset = total / iterations; // The average of all reading under calibration
    screen.reRender();
  }

  /*! Used to update zumoAngle. 
   *  DO NOT DELAY CODE WHILST USING THIS USING e.g. delay(). Inconsistencies will occur. */
  void dAngle() { 
    IMU.readGyro(); // Only gyrometer is needed for Zumo32U4 angle readings
    int16_t turnRate = IMU.g.z - offset;
    uint16_t m = micros(); // "New time"
    uint16_t dt = m - lastUpdate;
    lastUpdate = m; // "New time" becomes "old time"
    int32_t d = (int32_t)turnRate * dt; // Angle = Angular_velocity * time
    turnAngle += (int64_t)d * 14680064 / 17578125; // Fancy maths
    zumoAngle = (((int32_t)turnAngle >> 16) * 360) >> 16; // Even more fancy maths
  }

  void reset() {
    lastUpdate = micros();
    turnAngle = 0;
    zumoAngle = 0;
  }

  int gyroAdjust() { 
    dAngle();
    return (180-zumoAngle)/9; 
  }
} imu;

struct motors {
  void forward(int speed=100) { MOTORS.setSpeeds(speed, speed); }
  void stop() { MOTORS.setSpeeds(0, 0); }
  void left() { MOTORS.setSpeeds(-100,0); }
  void right() { MOTORS.setSpeeds(0,-100); }
  void turn(int spd) {
    //first turns on the spot and then moves slightly forward while turning
    MOTORS.setSpeeds(spd,-spd);
    delay(20);
    MOTORS.setSpeeds(spd,(-0.75*spd));
    delay(75);
  }
} motors;

struct encoders {
  int16_t value[2];

  void read() {
    value[0] = ENCODERS.getCountsLeft();
    value[1] = ENCODERS.getCountsRight();
  }

  void reset() {
    value[0] = ENCODERS.getCountsAndResetLeft();
    value[1] = ENCODERS.getCountsAndResetRight();
  }

  float getDistance() {
    float wheelCirc = 13;
    int countsL = ENCODERS.getCountsLeft();
    int countsR = ENCODERS.getCountsRight();

    float distanceL = countsL/900.0 * wheelCirc;
    float distanceR = countsR/900.0 * wheelCirc;

    return (distanceL + distanceR)/2;
  }
} encoders;

void align() {
  int threshold = 600;
  int speed = 100;
  int caseStep = 0;
  bool leftReachedTape = false, leftOverTape = false;
  bool rightReachedTape = false, rightOverTape = false;
  lineSensors.read();

  if (lineSensors.value[0] < threshold && lineSensors.value[4] < threshold) { caseStep = 0;} 
  else if (lineSensors.value[0] >= threshold) {
    caseStep = 2;
    leftReachedTape = true;
  } 
  else if (lineSensors.value[4] >= threshold) {
    caseStep = 2;
    rightReachedTape = true;
  }

  switch (caseStep){
    case 0: 
      motors.forward(speed);
      break;
    case 1: 
      motors.stop();
      break;
    case 2: 
      motors.stop();
      delay(500);
      motors.forward(speed/2);

      bool run = true;

      while(run){
        lineSensors.read();
        if(leftReachedTape && lineSensors.value[4] >= threshold){
          motors.stop();
          rightReachedTape = true;
          run = false;
        } 
        else if (rightReachedTape && lineSensors.value[0] >= threshold){
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

        //  Checks left sid
        if (leftReachedTape && lineSensors.value[0] < threshold){      //  Goes back if over tape.
          MOTORS.setLeftSpeed(-speed);
          leftOverTape = true;
        } 
        else if(leftOverTape && lineSensors.value[0] >= threshold){ MOTORS.setLeftSpeed(0); } //  Only stops if it have been over the tape once.

        //  Checks right side
        if (rightReachedTape && lineSensors.value[2] < threshold){     //  Goes back if over tape.
          MOTORS.setRightSpeed(-speed);
          rightOverTape = true;
        } 
        else if (rightOverTape && lineSensors.value[2] >= threshold){ MOTORS.setRightSpeed(0); }  //  Only stops if it have been over the tape once.
      }
      break;
  }

  delay(100);
}