#include "Zumo32U4_components.h"

void buttons::pressAnyKeyToContinue() { 
  while (!Zumo32U4ButtonA::isPressed() && 
         !Zumo32U4ButtonB::isPressed() && 
         !Zumo32U4ButtonC::isPressed()) {} 
}

bool buttons::getProtocol() {
  if (Zumo32U4ButtonA::getSingleDebouncedRelease()) { protocol++; }
  if (Zumo32U4ButtonB::getSingleDebouncedRelease()) { protocol--; }
  if (protocol % 8 == 0) { protocol = -(protocol/8)*6 + 7; } // if (protocol == 0) { protocol = 7; } else if (protocol == 8) { protocol = 1; }
  if (Zumo32U4ButtonC::getSingleDebouncedRelease()) { return true; }
  else { return false; }
}

lineSensors::lineSensors() { Zumo32U4LineSensors::initFiveSensors(); }

void lineSensors::read() { Zumo32U4LineSensors::read(value, QTR_EMITTERS_ON); }

proxSensors::proxSensors() {
  Zumo32U4ProximitySensors::initThreeSensors();
  Zumo32U4ProximitySensors::setPulseOffTimeUs(400);
  Zumo32U4ProximitySensors::setPulseOnTimeUs(300);
}

void proxSensors::read() {
  Zumo32U4ProximitySensors::read();

  value[0] = Zumo32U4ProximitySensors::countsLeftWithLeftLeds();
  value[1] = Zumo32U4ProximitySensors::countsLeftWithRightLeds();

  value[2] = Zumo32U4ProximitySensors::countsFrontWithLeftLeds();
  value[3] = Zumo32U4ProximitySensors::countsFrontWithRightLeds();

  value[4] = Zumo32U4ProximitySensors::countsRightWithLeftLeds();
  value[5] = Zumo32U4ProximitySensors::countsRightWithRightLeds();
}

float proxSensors::getError() { return value[5]-(value[0]-0.2); }

float proxSensors::getErrorFront() { return value[3]-value[2]; }

void imu::initMAG() {
  Wire.begin();
  Zumo32U4IMU::init();
  Zumo32U4IMU::enableDefault();
  Zumo32U4IMU::configureForTurnSensing();
}

void imu::reset() {
  lastUpdate = micros(); // Resets timer
  turnAngle = 0; // Resets angle
  zumoAngle = 0; // Resets Zumo32U4 angle (the important one)
}

void imu::calibrateTurn(int iterations=1000) {
  imu::initMAG();
  int32_t total = 0;
  for (int i = 0; i < iterations; i++) {
      while(!Zumo32U4IMU::gyroDataReady()) {}
      Zumo32U4IMU::readGyro();
      total += Zumo32U4IMU::g.z;
  }
  imu::reset();
  offset = total / iterations; // The average of all reading under calibration
}

void imu::dAngle() { 
  Zumo32U4IMU::readGyro(); // Only gyrometer is needed for Zumo32U4 angle readings
  int16_t turnRate = Zumo32U4IMU::g.z - offset;
  uint16_t m = micros(); // "New time"
  uint16_t dt = m - lastUpdate;
  lastUpdate = m; // "New time" becomes "old time"
  int32_t d = (int32_t)turnRate * dt; // Angle = Angular_velocity * time
  turnAngle += (int64_t)d * 14680064 / 17578125; // Fancy maths
  zumoAngle = (((int32_t)turnAngle >> 16) * 360) >> 16; // Even more fancy maths
}

int imu::gyroAdjust() { 
  imu::dAngle();
  return (180-zumoAngle)/9; 
}

void motors::forward(int speed=100) { Zumo32U4Motors::setSpeeds(speed, speed); }

void motors::stop() { Zumo32U4Motors::setSpeeds(0, 0); }

void motors::left() { Zumo32U4Motors::setSpeeds(-100,0); }

void motors::right() { Zumo32U4Motors::setSpeeds(0,-100); }

void motors::turn(int speed) {
  //first turns on the spot and then moves slightly forward while turning
  Zumo32U4Motors::setSpeeds(speed,-speed);
  delay(20);
  Zumo32U4Motors::setSpeeds(speed,-speed);
  delay(75);
}

void encoders::read() {
  value[0] = Zumo32U4Encoders::getCountsLeft();
  value[1] = Zumo32U4Encoders::getCountsRight();
}

void encoders::reset() {
  value[0] = Zumo32U4Encoders::getCountsAndResetLeft();
  value[1] = Zumo32U4Encoders::getCountsAndResetRight();
}

float encoders::getDistance() {
  float wheelCirc = 13;
  int countsL = Zumo32U4Encoders::getCountsLeft();
  int countsR = Zumo32U4Encoders::getCountsRight();

  float distanceL = countsL/900.0 * wheelCirc;
  float distanceR = countsR/900.0 * wheelCirc;

  return (distanceL + distanceR)/2;
}