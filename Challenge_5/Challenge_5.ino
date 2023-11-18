#include <Wire.h>
#include <Zumo32U4.h>

Zumo32U4ProximitySensors proximity;
Zumo32U4Motors motors;
Zumo32U4LineSensors lineSensors;
Zumo32U4OLED oled;

const int levels = 13;

uint8_t value[6];
int brightnessLevels[levels];
int16_t lineSensorValues[3];

void getProximity() {
  value[0] = proximity.countsLeftWithLeftLeds();
  value[1] = proximity.countsLeftWithRightLeds();

  value[2] = proximity.countsFrontWithLeftLeds();
  value[3] = proximity.countsFrontWithRightLeds();

  value[4] = proximity.countsRightWithLeftLeds();
  value[5] = proximity.countsRightWithRightLeds();
}

bool eightFig = false;
bool middlePoint = false;
int count = 0;

void eightFigure() {
  if (value[2] + value[3] > 20) { middlePoint = true; }
  if (value[2] + value[3] <= 14 && middlePoint == true) {
    if (eightFig == false) {
      motors.setSpeeds(100, 240);
      delay(600);
    }
    else {
      motors.setSpeeds(100, -100);
      delay(300);
      motors.setSpeeds(100, 100);
      delay(300);
    }
    eightFig = !eightFig;
    middlePoint = false;
  }
  if (eightFig == false) {
    motors.setSpeeds(100 + 15 * (value[5] - value[0]), 100 - 15 * (value[5] - value[0]));
  }
  else {
    motors.setSpeeds(100 - 20 * (value[5] - 13), 100 + 20 * (value[5] - 13));
  }
  if (lineSensorValues[2] > 400) { 
    count++;
    while (lineSensorValues[2] > 400) { lineSensors.read(lineSensorValues, QTR_EMITTERS_ON); }
  }
  oled.clear();
  oled.print((String)value[2] + "\t" + value[3]);
  oled.gotoXY(0, 1);
  oled.print((String)count);
}

void setup() {
  // put your setup code here, to run once:
  for (int i = 0; i < levels; i++) {
    brightnessLevels[i] = 2 * i;
  }
  proximity.initThreeSensors();
  proximity.setBrightnessLevels(brightnessLevels, levels);
  lineSensors.initThreeSensors();
}

void loop() {
  // put your main code here, to run repeatedly:
  proximity.read();
  lineSensors.read(lineSensorValues, QTR_EMITTERS_ON);
  getProximity();
  eightFigure();
  /*Serial.println((String)value[0] + " " + value[0] + "\t" + 
                         value[2] + " " + value[3] + "\t" + 
                         value[4] + " " + value[5]);*/
  Serial.println((String)lineSensorValues[0] + "\t" +  lineSensorValues[1] + "\t" +  lineSensorValues[2]);
}
