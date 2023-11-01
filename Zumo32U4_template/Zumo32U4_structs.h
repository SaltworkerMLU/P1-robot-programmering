#include <Arduino.h>  // A header file, originally from C, required the library that makes .ino what it is.
#include <Wire.h>     // Zumo32U4.h depends on this library to function properly
#include <Zumo32U4.h> // Access Zumo32U4.h library here: https://pololu.github.io/zumo-32u4-arduino-library/

Zumo32U4ButtonA buttonA;
Zumo32U4ButtonB buttonB;
Zumo32U4ButtonC buttonC;
Zumo32U4Encoders motorEncoders;
Zumo32U4ProximitySensors proximitySensors;
Zumo32U4LineSensors lineSensors;
Zumo32U4IMU IMU;
Zumo32U4Motors motors;
Zumo32U4OLED OLED;
Zumo32U4LCD LCD;

/*! This struct makes use of all 3 buttons. */
struct buttons {
  /*! Whatever button gets pressed and released whilst
   *  getRelease() is running, this variable represents.  */
  char release = "O";

  /*! Check button press state.
   *  NONE=0; A=1; B=2; A+B=3; C=4; A+C=5; B+C=6; A+B+C=7; */
  int checkPress() {
    return buttonA.isPressed() + 
           buttonB.isPressed() * 2 + 
           buttonC.isPressed() * 4;
  }

  /*! Checks if either A, B or C has been pressed and released.
   *  If so, return the specific character of the button pressed and released. */
  void getRelease() {
    if (buttonA.getSingleDebouncedRelease() == true) { release = 'A'; }
    else if (buttonB.getSingleDebouncedRelease() == true) { release = 'B'; }
    else if (buttonC.getSingleDebouncedRelease() == true) { release = 'C'; }
  }
};

/*! This struct represents the encoders, aka. the motor turns, based on wheel circumference, measurer */
struct encoders {
  float distance[2]; // Motor distance in centimeters cm. motorDistance[0] = left motor; motorDistance[1] = right motor

  void getDistance() { // Updates motorDistance[] in cm based on raw encoder readings
    distance[0] = 8 * acos(0.0) * motorEncoders.getCountsLeft() / 900; // [...] = 8 * acos(0.0) * count / 900... CPR table value: https://www.pololu.com/docs/0J63/3.4
    distance[1] = 8 * acos(0.0) * motorEncoders.getCountsRight() / 900; // If distance does not measure up, adjust using class variable CPR.
  }

  void getDistanceReset() { // Like getMotorDistance() but also resets raw encoder readings
    distance[0] = 8 * acos(0.0) * motorEncoders.getCountsAndResetLeft() / 900; // [...] = 8 * acos(0.0) * count / 900... CPR table value: https://www.pololu.com/docs/0J63/3.4
    distance[1] = 8 * acos(0.0) * motorEncoders.getCountsAndResetRight() / 900; // If distance does not measure up, adjust using class variable CPR.
  }
};

/*! This struct represents the Zumo32U4 Proximity Sensors. */
struct proximity {
  /*! Raw proximity sensor values. Assumes all 3 proximity sensors are in use.
   *  * sensorValue[0] and sensorValue[1] -> Left proximity sensor LFT {OPTIONAL}
   *  * sensorValue[2] and sensorValue[3] -> Center proximity sensor FRONT
   *  * sensorValue[4] and sensorValue[5] -> right proximity sensor RGT {OPTIONAL}*/
  uint8_t sensorValue[6];

  void getSensorValue() { // Extracts proximity sensor values, and then update proximitySensorValue[]
    proximitySensors.read(); 
    sensorValue[0] = proximitySensors.countsLeftWithLeftLeds();
    sensorValue[1] = proximitySensors.countsLeftWithRightLeds();

    sensorValue[2] = proximitySensors.countsFrontWithLeftLeds();
    sensorValue[3] = proximitySensors.countsFrontWithRightLeds();

    sensorValue[4] = proximitySensors.countsRightWithLeftLeds();
    sensorValue[5] = proximitySensors.countsRightWithRightLeds();
  }
};

/*! This struct represents the Zumo32U4 Line Follower Sensors. */
struct line {
  /*! Raw line sensor values. Assumes all 5 line sensors are in use.
   *  * sensorValue[0] -> Utmost left line sensor DN1
   *  * sensorValue[1] -> Center left line sensor DN2 {OPTIONAL}
   *  * sensorValue[2] -> Dead center line sensor DN3
   *  * sensorValue[3] -> Center right line sensor DN4 {OPTIONAL}
   *  * sensorValue[4] -> Utmost left line sensor DN5 */
  uint16_t sensorValue[5];

  void getSensorValue() { lineSensors.read(sensorValue, QTR_EMITTERS_ON); } // Extracts raw line sensor values
};

/*! This struct represents the Zumo32U4 IMU */
struct imu {
  int16_t* mag[3] = {&IMU.m.x, &IMU.m.y, &IMU.m.z};  // All IMU data is represented in the struct using...
  int16_t* acc[3] = {&IMU.a.x, &IMU.a.y, &IMU.a.z};  // ... the address of the specific dimensions in...
  int16_t* gyro[3] = {&IMU.g.x, &IMU.g.y, &IMU.g.z}; // ... format: x = [0]; y = [1]; z = [2]

  //These values are explicitly used for the functions calibrateTurn() & turnAngle()
  int16_t offset; // Average reading using calibrateTurn()... an offset
  uint16_t lastUpdate; // Stores old time in microseconds using micros()
  uint32_t turnAngle = 0; // Turn angle which needs further conversion
  uint32_t zumoAngle = 0; // Zumo32U4 turn angle (THE ONE)

  /*! Initializing the IMU takes multiple commands, thus this singular function has been made */
  void initMAG() {
    Wire.begin();
    IMU.init();
    IMU.enableDefault();
    IMU.configureForBalancing();
  }

  /*! Takes measurements of gyrometer z-axis {iterations} amount of times. Then updates several struct variables */
  void calibrateTurn(int iterations=1000) {
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
  }

  /*! Used to update zumoAngle. 
   *  DO NOT DELAY CODE WHILST USING THIS USING e.g. delay(). Inconsistencies will occur. */
  void turnAngle() { 
    IMU.readGyro(); // Only gyrometer is needed for Zumo32U4 angle readings
    int16_t turnRate = IMU.g.z - offset;
    uint16_t m = micros(); // "New time"
    uint16_t dt = m - lastUpdate;
    lastUpdate = m; // "New time" becomes "old time"
    int32_t d = (int32_t)turnRate * dt; // Angle = Angular_velocity * time
    turnAngle += (int64_t)d * 14680064 / 17578125; // Fancy maths
    zumoAngle = (((int32_t)turnAngle >> 16) * 360) >> 16; // Even more fancy maths
  }
};

/*! Use either struct oled... */
struct oled {
  int8_t displayLine=0;

  /*! Sets display line and prints {input} */
  void displayPrint(String input) {
    OLED.gotoXY(0, displayLine);
    OLED.print(input);
    displayLine++;
  }
};

/*! ... or struct lcd */
struct lcd {
  int8_t displayLine=0;

  /*! Sets display line and prints {input} */
  void displayPrint(String input) {
    LCD.gotoXY(0, displayLine);
    LCD.print(input);
    displayLine++;
  }
};