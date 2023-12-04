#include <Wire.h>
#include <Zumo32U4.h>

Zumo32U4OLED OLED;
Zumo32U4LineSensors lineSensors;
Zumo32U4ButtonA buttonA;
Zumo32U4ButtonB buttonB;
Zumo32U4ButtonC buttonC;
Zumo32U4Buzzer buzzer;
Zumo32U4Motors motors;
Zumo32U4IMU imu;

//  For linesensors.
#define NUM_SENSORS 3
uint16_t lineSensorValues[NUM_SENSORS];

//  Variables to find the angle with gyro.
uint16_t gyroLastUpdate = 0;
int16_t gyroOffset;
int16_t turnRate;
int32_t turnAngle = 0;
int degreesToTurn = 0;

bool run = true;
const int speed = 100;
const int threshold = 600;
void setup() {
lineSensors.initThreeSensors();
}

void loop() {
if (run){
  alignZumo();
  delay(1000);
  turnSensorSetup();
  degreesToTurn = chsValue();
  run = false;
  turnByDegree(degreesToTurn);
  forward(speed);
  delay(800);
  run = true;
  while (run){
    readLineSensors();
    if ( lineSensorValues[0] >= threshold || lineSensorValues[1] >= threshold || lineSensorValues[2] >= threshold){
      stop();
      run = false;
    }
  }
}
}


void alignZumo(){ //find the line and align on the other side of it
  bool lOver = false, rOver = false;

  forward(speed);
  
  while (run){ // Stop the zumo with both sensors touching the start of the line
    readLineSensors();
    if ( lineSensorValues[0] >= threshold && lineSensorValues[2] >= threshold ){
      stop();
      run = false;
    }
    else if ( lineSensorValues[0] >= threshold ){
      motors.setLeftSpeed(0);
    }
    else if ( lineSensorValues[2] >= threshold ){
      motors.setRightSpeed(0);
    }
  }
  run = true;
  forward( speed * 2 );
  while (run){ // cross the line and align on the end of the line
    readLineSensors();
    if (lineSensorValues[0] < threshold){
      motors.setLeftSpeed(0);
      lOver = true;
    }
    if ( lineSensorValues[2] < threshold ){
      motors.setRightSpeed(0);
      rOver = true;
    }
    if (lOver && rOver){
      run = false;
    }
  }
}

void forward(int spd){
  motors.setSpeeds(spd,spd);
}
void stop(){
  motors.setSpeeds(0,0);
}
void turn(int spd){
  motors.setSpeeds(-spd, spd);
}

void readLineSensors(){
  lineSensors.read(lineSensorValues, QTR_EMITTERS_ON);
}
//choose angle:
int chsValue(){
  int value = 0;
  bool chsValue = true;

while (chsValue){
  if (buttonA.isPressed()){
  value+=5;
    if ( value > 180 ){
         value = 180;
      }
    buttonA.waitForRelease();
    printValue( value );
    buzz();
     }
  else if (buttonB.isPressed()){
    value-=5;
        if ( value < -180 ){
          value = -180;
        }
      buttonB.waitForRelease();
      printValue( value );
      buzz();
      }
  else if (buttonC.isPressed()){
    chsValue = false;
    buttonC.waitForRelease();
    buzz();
    }
  }
  return value;
}

void turnByDegree(int degree){
  bool run = true;
  turnSensorReset();
  float angle = getTurnAngleInDegrees();
  if (degree < 0){
    while (degree < angle){
      turn(-speed);
      angle = getTurnAngleInDegrees();
      OLED.clear();
      OLED.print(angle);
    }
  }
  if (degree > 0){
    while (degree > angle){
      turn(speed);
      angle = getTurnAngleInDegrees();
      OLED.clear();
      OLED.print(angle);
    }
  }
  stop();
}

//gyro stuff:
void turnSensorSetup(){
  Wire.begin();
  imu.init();
  imu.enableDefault();
  imu.configureForTurnSensing();
  OLED.clear();
  OLED.print(F("Gyro cal"));
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
  OLED.clear();
  turnSensorReset();
}

void turnSensorReset() {
  gyroLastUpdate = micros();
  turnAngle = 0;
}

float getTurnAngleInDegrees(){
  turnSensorUpdate();
  return (((int32_t)turnAngle >> 16) * 360) >> 16;
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

void buzz(){
  buzzer.playNote(300, 100, 10);
}
void printValue( int value ){
  OLED.clear();
  OLED.print(value);
}
