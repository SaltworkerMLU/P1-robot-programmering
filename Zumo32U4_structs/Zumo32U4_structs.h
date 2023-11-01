#include <Arduino.h>
#include <Zumo32U4.h>
#include <Wire.h>

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

struct buttons {
  char release = "O";

  int checkPress() {
    return buttonA.isPressed() + 
           buttonB.isPressed() * 2 + 
           buttonC.isPressed() * 4;
  }

  void getRelease() {
    if (buttonA.getSingleDebouncedRelease() == true) { release = 'A'; }
    else if (buttonB.getSingleDebouncedRelease() == true) { release = 'B'; }
    else if (buttonC.getSingleDebouncedRelease() == true) { release = 'C'; }
  }
};

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

struct proximity {
  uint8_t sensorValue[6]; // Raw proximity sensor values. Assumes all 3 proximity sensors are in use

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

struct line {
  uint16_t sensorValue[5]; // Raw line sensor values. Assumes all 5 line sensors are in use.

  void getSensorValue() { lineSensors.read(sensorValue, QTR_EMITTERS_ON); }
};

struct imu {
  int16_t* mag[3] = {&IMU.m.x, &IMU.m.y, &IMU.m.z};
  int16_t* acc[3] = {&IMU.a.x, &IMU.a.y, &IMU.a.z}; 
  int16_t* gyro[3] = {&IMU.g.x, &IMU.g.y, &IMU.g.z};

  int16_t offset;
  uint16_t lastUpdate;
  uint32_t turnAngle = 0;
  uint32_t zumoAngle = 0;

  void initMAG() {
    Wire.begin();
    IMU.init();
    IMU.enableDefault();
    IMU.configureForBalancing();
  }

  void calibrateTurning(int iterations=1000) {
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

  void gyroAngle() {
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

struct oled {
  int8_t displayLine=0;

  void displayPrint(String input) {
    OLED.print(input);
    displayLine++;
    OLED.gotoXY(0, displayLine);
  }
};

struct lcd {
  int8_t displayLine=0;

  void displayPrint(String input) {
    LCD.print(input);
    displayLine++;
    LCD.gotoXY(0, displayLine);
  }
};