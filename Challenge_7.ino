#include <Wire.h>
#include <Zumo32U4.h>

Zumo32U4Motors motors;
Zumo32U4LineSensors lineSensors;
Zumo32U4OLED display;
Zumo32U4Encoders encoders;
Zumo32U4IMU imu;

#define NUM_SENSORS 5
uint16_t lineSensorValues[NUM_SENSORS];
int threshold = 800;
int alignStage = 0;
int sideStage = 0;
uint32_t turnAngle = 0;
int16_t turnRate;
int16_t gyroOffset;
uint16_t gyroLastUpdate = 0;

void setup() {
  lineSensors.initFiveSensors(); 
  turnSensorSetup();
  Serial.begin(9600);
}

void print(int input){
  display.clear();
  display.print(input);
}

void loop() {
  switch (alignStage){
    case 0:
      readLineSensors();
      forward();
      if (lineSensorValues[0] > threshold || lineSensorValues[4] > threshold){
        if (lineSensorValues[0] > threshold)
          sideStage = 0;
        if (lineSensorValues[4] > threshold){
          sideStage = 1;
        }
        alignStage++;
      }
      break;
    case 1:
      switch (sideStage){
        case 0:
          resetEncoders();
          while (lineSensorValues[4] < threshold){
            readLineSensors();
            forward();
          }
          stop();
          while (encoders.getCountsLeft()+30 > 0){
            motors.setSpeeds(-100,0);
          }
          alignStage++;
          break;
        case 1:
          resetEncoders();
          while (lineSensorValues[0] < threshold){
            readLineSensors();
            forward();
          }
          stop();
          while (encoders.getCountsRight()+30 > 0){
            motors.setSpeeds(0,-100); 
          }
          alignStage++;
          break;
      }

      //Alt før dette er align
    case 2:
      stop();
      turnSensorReset();
      forward();
      delay(100);   
      alignStage++; 
    case 3:
      readLineSensors();
      if (lineSensorValues[0] > threshold || lineSensorValues[2] > threshold || lineSensorValues[4] > threshold){
        stop();
      }
      else{
        print(getTurnAngleInDegrees());
        motors.setSpeeds(200+gyroAdjust(),200-gyroAdjust());
      }
  } 
}

int gyroAdjust(){
  return (180-getTurnAngleInDegrees())/9;
}

void forward(){
  motors.setSpeeds(100,100);
}

void stop(){
  motors.setSpeeds(0,0);
}

void readLineSensors(){
  lineSensors.read(lineSensorValues, QTR_EMITTERS_ON);
  print(lineSensorValues[2]);
}

void print(String something){
  display.clear();
  display.print(something);
}

void resetEncoders(){
  encoders.getCountsAndResetLeft();
  encoders.getCountsAndResetRight();
}

void turnSensorSetup(){
  Wire.begin();
  imu.init();
  imu.enableDefault();
  imu.configureForTurnSensing();
  display.clear();
  display.print(F("Gyro cal"));
  ledYellow(1);
  delay(500);
  int32_t total = 0;
  for (uint16_t i = 0; i < 1024; i++)
  {
    while(!imu.gyroDataReady()) {}
    imu.readGyro();
    total += imu.g.z;
  }
  ledYellow(0);
  gyroOffset = total / 1024;
  display.clear();
  turnSensorReset();
}

int getTurnAngleInDegrees(){
  turnSensorUpdate();
  return (((uint32_t)turnAngle >> 16) * 360) >> 16;
}

void turnSensorUpdate(){
  imu.readGyro();
  turnRate = imu.g.z - gyroOffset;
  uint16_t m = micros();
  uint16_t dt = m - gyroLastUpdate;
  gyroLastUpdate = m;
  int32_t d = (int32_t)turnRate * dt;
  turnAngle += (int64_t)d * 14680064 / 17578125;
}

void turnSensorReset(){
  gyroLastUpdate = micros();
  turnAngle = 0;
}
