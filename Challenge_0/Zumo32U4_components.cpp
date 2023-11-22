#include "Zumo32U4_components.h"

bool buttons::pressAnyKeyToContinue() { 
  if (!Zumo32U4ButtonA::getSingleDebouncedRelease() && 
      !Zumo32U4ButtonB::getSingleDebouncedRelease() && 
      !Zumo32U4ButtonC::getSingleDebouncedRelease()) { return true; }
  return false; // If any of the above buttons is pressed
}

bool buttons::getProtocol() {
  if (Zumo32U4ButtonA::getSingleDebouncedRelease()) { protocol++; }
  if (Zumo32U4ButtonB::getSingleDebouncedRelease()) { protocol--; }
  if (protocol % 8 == 0) { protocol = -(protocol/8)*6 + 7; } // if (protocol == 0) { protocol = 7; } else if (protocol == 8) { protocol = 1; }
  if (Zumo32U4ButtonC::getSingleDebouncedRelease()) { return true; }
  else { return false; }
}

lineSensors::lineSensors() { Zumo32U4LineSensors::initThreeSensors(); }

void lineSensors::read() { Zumo32U4LineSensors::read(value, QTR_EMITTERS_ON); }

proxSensors::proxSensors() {
  Zumo32U4ProximitySensors::initThreeSensors();
  Zumo32U4ProximitySensors::setPulseOffTimeUs(400);
  Zumo32U4ProximitySensors::setPulseOnTimeUs(300);
  for (int i = 0; i < proxSensors::level; i++) { proxSensors::brightness[i] = i; }
  proxSensors::setBrightnessLevels(proxSensors::brightness, proxSensors::level);
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

bool imu::failsafe() {
  Zumo32U4IMU::readAcc();
  if (Zumo32U4IMU::a.z <= 0) { return false; }
  return true; // Else-statement not needed with return.
}

void motors::forward(int speed=100) { Zumo32U4Motors::setSpeeds(speed, speed); }

void motors::stop() { Zumo32U4Motors::setSpeeds(0, 0); }

void motors::left(int speed=100) { Zumo32U4Motors::setSpeeds(speed, -speed); }

void motors::right(int speed=100) { Zumo32U4Motors::setSpeeds(-speed, speed); }

void encoders::read() {
  value[0] = Zumo32U4Encoders::getCountsLeft();
  value[1] = Zumo32U4Encoders::getCountsRight();
}

void encoders::reset() {
  value[0] = Zumo32U4Encoders::getCountsAndResetLeft();
  value[1] = Zumo32U4Encoders::getCountsAndResetRight();
}

float encoders::readDistance() {
  encoders::value[0] = 10 * 8 * acos(0.0) * Zumo32U4Encoders::getCountsLeft() / 900;
  encoders::value[1] = 10 * 8 * acos(0.0) * Zumo32U4Encoders::getCountsRight() / 900;
  return (value[0] + value[1])/2; // Average of the two
}