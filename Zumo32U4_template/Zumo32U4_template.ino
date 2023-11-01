#include "Zumo32U4_structs.h"

buttons buttons;
encoders encoders;
proximity proximity;
line line;
imu imu;
oled oled;
lcd lcd;

void setup() {
  // put your setup code here, to run once:
  proximitySensors.initThreeSensors();
  lineSensors.initFiveSensors();
  imu.initMAG();
  OLED.setLayout21x8();
}

void loop() {
  // put your main code here, to run repeatedly:
  buttons.getRelease();
  encoders.getDistance();
  proximity.getSensorValue();
  line.getSensorValue();
  IMU.read();
  OLED.clear();
  oled.displayLine = 0;
  oled.displayPrint((String)"ENCODER: " + encoders.distance[0] + "\t" + encoders.distance[1]); // displayLine 0
  oled.displayPrint((String)"LINE024: " + line.sensorValue[0] + "\t" + line.sensorValue[2] + "\t" + line.sensorValue[4]); // displayLine 1
  oled.displayPrint((String)"LINE13:  " + line.sensorValue[1] + "\t" + line.sensorValue[3]); // displayLine 2
  oled.displayPrint((String)"PROX: " + proximity.sensorValue[0] + "\t" + proximity.sensorValue[1] + "\t" + 
                            proximity.sensorValue[2] + "\t" + proximity.sensorValue[3] + "\t" + 
                            proximity.sensorValue[4] + "\t" + proximity.sensorValue[5]); // displayLine 3
  oled.displayPrint((String)"MAG: " + *imu.mag[0] + "\t" + *imu.mag[1] + "\t" + *imu.mag[2]); // displayLine 4
  oled.displayPrint((String)"ACC: " + *imu.acc[0] + "\t" + *imu.acc[1] + "\t" + *imu.acc[2]); // displayLine 5
  oled.displayPrint((String)"GYR: " + *imu.gyro[0] + "\t" + *imu.gyro[1] + "\t" + *imu.gyro[2]); // displayLine 6
  oled.displayPrint((String)"BUTTONS: " + (String)buttons.checkPress() + "\t" + buttons.release); // displayLine 7
  delay(100);
}