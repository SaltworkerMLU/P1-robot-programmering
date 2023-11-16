#include "Arduino.h" // Needed for any header file to run arduino code
#include <Wire.h>
#include <Zumo32U4.h>

Zumo32U4ButtonA BUTTONA;
Zumo32U4ButtonB BUTTONB;
Zumo32U4ButtonC BUTTONC;
Zumo32U4Buzzer BUZZER;
Zumo32U4OLED SCREEN;
Zumo32U4Encoders ENCODERS;
Zumo32U4IMU IMU;
Zumo32U4LineSensors LINESENSORS;
Zumo32U4Motors MOTORS;
Zumo32U4ProximitySensors PROXSENSORS;

/*! This struct makes use of all 3 buttons. */
struct buttons {
  /*! The challenge which the Zumo must complete.
   *  To start of, protocol/challenge 0 is run. */
  uint8_t protocol = 0;

  /*! While neither button A, B, nor C has been pressed: Do nothing. */
  void pressAnyKeyToContinue() { 
    SCREEN.clear();
    SCREEN.print("A=+; B=-");
    SCREEN.gotoXY(0, 1);
    SCREEN.print("C=onfirm");
    while (!BUTTONA.isPressed() && !BUTTONB.isPressed() && !BUTTONC.isPressed()) {} 
    SCREEN.clear();
  }

  /*! Function to acquire variable protocol. Update protocol by pressing and releasing:
   *  * buttonA -> Add 1 to protocol.
   *  * buttonB -> Subtract 1 to protocol.
   *  * buttonC -> end function. 
   *  Whilst function is running, keep protocol in range 1-7 and show variable protocol on display. */
  void getProtocol() {
    while (!BUTTONC.getSingleDebouncedRelease()) {
      SCREEN.print((String)protocol);
      SCREEN.gotoXY(0, 0);
      if (BUTTONA.getSingleDebouncedRelease()) { protocol++; }
      if (BUTTONB.getSingleDebouncedRelease()) { protocol--; }
      if (protocol % 8 == 0) { protocol = -(protocol/8)*6 + 7; } // if (protocol == 0) { protocol = 7; } else if (protocol == 8) { protocol = 1; }
    }
    SCREEN.clear();
  }
};

struct lineSensors {
  /*! Line sensor values are stored here after lineSensors.read() has been called. */
  uint16_t value[5];

  /*! Stored threshold regarding black/white. */
  int threshold = 800;

  /*! CONSTRUCTOR. It executes as the very first thing when struct object called. */
  lineSensors() { LINESENSORS.initFiveSensors(); }

  /*! Reads line follower sensors and updates value[3]. */
  void read() { LINESENSORS.read(value, QTR_EMITTERS_ON); }

};

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
    PROXSENSORS.initThreeSensors();
    PROXSENSORS.setPulseOffTimeUs(400);
    PROXSENSORS.setPulseOnTimeUs(300);
  }

  /*! Reads proximity sensors and updates value[6]. */
  void read() {
    PROXSENSORS.read();

    value[0] = PROXSENSORS.countsLeftWithLeftLeds();
    value[1] = PROXSENSORS.countsLeftWithRightLeds();

    value[2] = PROXSENSORS.countsFrontWithLeftLeds();
    value[3] = PROXSENSORS.countsFrontWithRightLeds();

    value[4] = PROXSENSORS.countsRightWithLeftLeds();
    value[5] = PROXSENSORS.countsRightWithRightLeds();
  }

  /*! Returns error of side sensors. */
  float getError() { return value[5]-(value[0]-0.2); }

  /*! Returns error of front sensors. */
  float getErrorFront() { return value[3]-value[2]; }
};

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
    SCREEN.print("Gyro cal");
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
    SCREEN.clear();
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
};

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
};

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
};
