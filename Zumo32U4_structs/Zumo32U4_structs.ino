#include "Zumo32U4_structs.h"

buttons buttons;
encoders encoders;
proximity proximity;
line line;
imu imu;
OLED OLED;
LCD LCD;

void setup() {
  // put your setup code here, to run once:
  proximitySensors.initThreeSensors();
  lineSensors.initFiveSensors();
  imu.initMAG();
  oled.setLayout21x8();
}

void loop() {
  // put your main code here, to run repeatedly:
  buttons.getRelease();
  encoders.getDistance();
  proximity.getProximitySensorValue();
  line.getLineSensorValue();
  IMU.read();
  oled.clear();
  OLED.displayLine = 0;
  OLED.displayPrint((String)"ENCODER: " + encoders.distance[0] + "\t" + encoders.distance[1]); // displayLine 0
  OLED.displayPrint((String)"LINE024: " + line.sensorValue[0] + "\t" + line.sensorValue[2] + "\t" + line.sensorValue[4]); // displayLine 1
  OLED.displayPrint((String)"LINE13:  " + line.sensorValue[1] + "\t" + line.sensorValue[3]); // displayLine 2
  OLED.displayPrint((String)"PROX: " + proximity.sensorValue[0] + "\t" + proximity.sensorValue[1] + "\t" + 
                            proximity.sensorValue[2] + "\t" + proximity.sensorValue[3] + "\t" + 
                            proximity.sensorValue[4] + "\t" + proximity.sensorValue[5]); // displayLine 3
  OLED.displayPrint((String)"MAG: " + *imu.mag[0] + "\t" + *imu.mag[1] + "\t" + *imu.mag[2]); // displayLine 4
  OLED.displayPrint((String)"ACC: " + *imu.acc[0] + "\t" + *imu.acc[1] + "\t" + *imu.acc[2]); // displayLine 5
  OLED.displayPrint((String)"GYR: " + *imu.gyro[0] + "\t" + *imu.gyro[1] + "\t" + *imu.gyro[2]); // displayLine 6
  OLED.displayPrint((String)"BUTTONS: " + (String)buttons.checkPress() + "\t" + buttons.release); // displayLine 7
  /*Serial.print((String)line.sensorValue[0] + "\t" + line.sensorValue[1] + "\t" + line.sensorValue[2] + "\t" + line.sensorValue[3] + "\t" + line.sensorValue[4]);
  Serial.println((String)proximity.sensorValue[0] + "\t" + proximity.sensorValue[1] + "\t" + 
                         proximity.sensorValue[2] + "\t" + proximity.sensorValue[3] + "\t" + 
                         proximity.sensorValue[4] + "\t" + proximity.sensorValue[5]);*/
  delay(100);
}