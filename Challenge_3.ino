#include <Wire.h>
#include <Zumo32U4.h>
#include <Zumo32U4IRPulses.h>
#include <Zumo32U4ProximitySensors.h>

Zumo32U4OLED display;
Zumo32U4LineSensors lineSensors;
Zumo32U4ProximitySensors proxSensors;
Zumo32U4Motors motors;

uint16_t lineSensorValues[3];
int threshold = 800;

void setup() {
 Serial.begin(9600);
  uint8_t levelCount = 20;
  int brightnessLevels[20];
  for (int i=1; i<levelCount; i++){
    brightnessLevels[i] = i;
  }
  proxSensors.setBrightnessLevels(brightnessLevels, levelCount);
  proxSensors.initThreeSensors();
  proxSensors.setPulseOffTimeUs(400);
  proxSensors.setPulseOnTimeUs(300);
  lineSensors.initThreeSensors();
  delay(10);
}

void loop() {
  readSensors();
  while (lineSensorValues[0]<threshold && lineSensorValues[1]<threshold && lineSensorValues[2]<threshold){ 
    readSensors();
    motors.setSpeeds(150-30*getError()-15*getErrorFront(),150+30*getError()+15*getErrorFront());
    delay(5);
  }
  motors.setSpeeds(0,0);
}

void readSensors(){
  proxSensors.read();
  lineSensors.read(lineSensorValues, QTR_EMITTERS_ON);
}

float getError(){
  return proxSensors.countsRightWithRightLeds()-(proxSensors.countsLeftWithLeftLeds());
}

float getErrorFront(){
  return proxSensors.countsFrontWithRightLeds()-proxSensors.countsFrontWithLeftLeds();
}

