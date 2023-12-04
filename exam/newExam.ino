#include <Wire.h>
#include <Zumo32U4.h>

Zumo32U4OLED OLED;
Zumo32U4LineSensors lineSensors;
Zumo32U4ButtonA buttonA;
Zumo32U4ButtonB buttonB;
Zumo32U4ButtonC buttonC;
Zumo32U4Buzzer buzzer;
Zumo32U4Motors motors;
Zumo32U4Encoders encoders;
Zumo32U4ProximitySensors proxSensors;
Zumo32U4IMU imu;

//  For linesensors.
#define NUM_SENSORS 3
uint16_t lineSensorValues[NUM_SENSORS];


const int speed = 100;
const int threshold = 600;
const float wheelCirc = 11.94;

int count = 0;

uint8_t value[6];

bool middlePoint = false;
bool eightFig = false;

//  Variables to find the angle with gyro.
uint16_t gyroLastUpdate = 0;
int16_t gyroOffset;
int16_t turnRate;
int32_t turnAngle = 0;

void setup() {
  Serial.begin(9600);
  lineSensors.initThreeSensors();
  proxSensors.initThreeSensors();
}

void loop() {
  switch(challengeZero()){
    case 1: 
      challengeOne();
      break;
    case 2:
      challengeTwo();
      break;
    case 3:
      challengeThree();
      break;
    case 4:
      challengeFour();
      break;
    case 5:
      challengeFive();
      break;
    case 6:
      challengeSix();
      break;
    case 7:
      challengeSeven();
      break;
    default: break;
  }
}

//  Prints first the action of A, B and C buttons.
//  Then starts a loop for input of button press.
int challengeZero(){
  int taskValue = 0;
  bool chooseTask = true;

  OLED.clear();
  OLED.print("A=+; B=-");
  OLED.gotoXY(0, 1);
  OLED.print("C=verify");

  taskValue = chsValue( 0 );
  
  //  Returns the choosen challenge to be started.
  return taskValue;
}

void challengeOne(){
  int maxLength = 41;
  int length;
  bool runLastPart = true;

  //  Gets a value returned from the function.
  length = chsValue(1);

  countdown();
  alignZumo();

  delay(200);

  resetEncoders();
  forward(speed);

  while (runLastPart){
    if (getDistance() >= maxLength - length){
      stop();
      runLastPart = false;
    }
  }
}

void challengeTwo(){
  int baseSpeed = 200;
  bool run = true;
  countdown();

  while( run ){
    readLineSensors();

    if ( lineSensorValues[0] >= threshold){
      turnOnLine( -baseSpeed );
    }
    else if ( lineSensorValues[2] >= threshold ){
      turnOnLine( baseSpeed );
    }
    else {
      forward( baseSpeed );
    }
  }
}

void challengeThree(){
  countdown();

  uint8_t levelCount = 20;
  int brightnessLevels[levelCount];
  for (int i = 1; i < levelCount; i++){
    brightnessLevels[i] = i;
  }

  proxSensors.setBrightnessLevels(brightnessLevels, levelCount);
  proxSensors.setPulseOffTimeUs(400);
  proxSensors.setPulseOnTimeUs(300);
  delay(10);

  readLineSensors();
  while (lineSensorValues[0] < threshold && lineSensorValues[1] < threshold && lineSensorValues[2] < threshold){ 
    readSensors();
    motors.setSpeeds(150 - 30 * getError() - 15 * getErrorFront(), 150 + 30 * getError() + 15 * getErrorFront());
    delay(5);
  }
  motors.setSpeeds(0,0);
}

void challengeFour(){
  int length;
  bool runChallengeFour = true;

  length = chsValue(4);

  countdown();

  alignZumo();

  resetEncoders();

  forward( speed * 2 );

  while (runChallengeFour){
    if (getDistance() >= length){
      stop();
      runChallengeFour = false;
    }
  }
}

void challengeFive(){
  countdown();

  int lastCount = 0;
  const int levels = 13;
  bool run = true;
  int brightnessLevels[levels];

  for (int i = 0; i < levels; i++) {
    brightnessLevels[i] = 2 * i;
  }

  proxSensors.setBrightnessLevels(brightnessLevels, levels);

  while (run){
    proxSensors.read();
    readLineSensors();

    if (lineSensorValues[0] > threshold || lineSensorValues[0] > threshold || lineSensorValues[2] > threshold){
      count++;
    }

    OLED.clear();
    OLED.print("Count:");
    OLED.gotoXY(7, 0);
    OLED.print(count);
    lastCount = count;
    
    getProximity();
    eightFigure();
  }
}

void challengeSix(){
  int degreesToTurn = chsValue(6);

  countdown();
  bool run = true;

  alignZumo();
  delay(1000);

  turnSensorSetup();
  run = false;
  turnByDegree( degreesToTurn );
  forward( speed );
  delay(800);
  run = true;
  while ( run ){
    readLineSensors();

    if ( lineSensorValues[0] >= threshold || lineSensorValues[1] >= threshold || lineSensorValues[2] >= threshold){
      stop();
      run = false;
    }
  }
}

void challengeSeven(){
  bool run = true;

  countdown();
  alignZumo();

  turnSensorSetup();
  forward( speed );
  delay( 200 );
  
  while (run){
    readLineSensors();
    if (lineSensorValues[0] > threshold || lineSensorValues[2] > threshold || lineSensorValues[4] > threshold){
      stop();
    }
    else {
      printValue(getTurnAngleInDegrees());
      motors.setSpeeds(200 + gyroAdjust(), 200 - gyroAdjust());
    }
  }
}


//  Functions to complete challenges.

//  Choosing value functions lets the user pick a value.
//  This value is either used to pick a challenge or choose a length or pick an angle.
void alignZumo(){
  bool run = true;
  bool lOver = false, rOver = false;

  forward( speed );
  
  while ( run ){
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

  while ( run ){
    readLineSensors();

    if ( lineSensorValues[0] < threshold ){
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

//  *** Challenge 2 functions ***
void turnOnLine( int spd ){
  motors.setSpeeds(spd, -spd);
  delay(20);
  motors.setSpeeds(spd, (-0.75 * spd));
  delay(75);
}


//  *** Challenge 3 functions ***
float getError(){
  return proxSensors.countsRightWithRightLeds() - (proxSensors.countsLeftWithLeftLeds());
}

float getErrorFront(){
  return proxSensors.countsFrontWithRightLeds() - proxSensors.countsFrontWithLeftLeds();
}


//  *** Challenge 5 functions ***
void getProximity() {
  value[0] = proxSensors.countsLeftWithLeftLeds();
  value[1] = proxSensors.countsLeftWithRightLeds();

  value[2] = proxSensors.countsFrontWithLeftLeds();
  value[3] = proxSensors.countsFrontWithRightLeds();

  value[4] = proxSensors.countsRightWithLeftLeds();
  value[5] = proxSensors.countsRightWithRightLeds();
}

void eightFigure() {
  if (value[2] + value[3] > 20) {
    middlePoint = true;
  }

  if (value[2] + value[3] <= 14 && middlePoint) {
    if (eightFig == false) {
      motors.setSpeeds(100, 240);
      delay(600);
    }
    else {
      motors.setSpeeds(100, -100);
      delay(200);
      motors.setSpeeds(100, 100);
      delay(200);
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
}


//  *** Challenge 7 functions ***
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

int gyroAdjust(){
  return (180 - getTurnAngleInDegrees()) / 9;
}





//  Functions to find and turn an angle.
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

void turn(int spd){
  motors.setSpeeds(-spd, spd);
}

//  Other functions.

int chsValue( int challenge ){
  int value = 0;
  bool chsValue = true;

  if ( challenge != 0 ){
    printValue( value );
  }

  while (chsValue){
    if (buttonA.isPressed()){
      value++;

      if ( value > 7 && challenge == 0){
        value = 1;
      }
      else if ( value > 41 && challenge == 1){
        value = 41;
      }
      else if ( challenge == 6 ){
        value--;
        value += 5;

        if ( value > 180 ){
          value = 180;
        }
      }

      buttonA.waitForRelease();
      printValue( value );

      buzz();
    }
    else if (buttonB.isPressed()){
      value--;
      
      if ( value < 1 && challenge == 0 ){
        value = 7;
      }
      else if ( challenge != 6 && value < 1 ){
        value = 0;
      }
      else if ( challenge == 6 ){
        value++;
        value -= 5;

        if ( value < -180 ){
          value = -180;
        }
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

void printValue( int value ){
  OLED.clear();
  OLED.print(value);
}

void buzz(){
  buzzer.playNote(300, 100, 10);
}

void countdown(){
  for (int time = 5; time > 0; time--){
    buzz();
    OLED.clear();
    OLED.print(time);
    delay(1000);
  }
  buzzer.playNote(330, 200, 10);
  OLED.clear();
}

void forward( int spd ){
  motors.setSpeeds( spd, spd );
  ledRed(false);
  ledGreen(true);
}

void stop(){
  motors.setSpeeds(0, 0);
  ledRed(true);
  ledGreen(false);
}

void readLineSensors(){
  lineSensors.read(lineSensorValues, QTR_EMITTERS_ON);
}

void readSensors(){
  proxSensors.read();
}

float getDistance(){
  int countsL = encoders.getCountsLeft();
  int countsR = encoders.getCountsRight();

  float distanceL = countsL/909.7 * wheelCirc;
  float distanceR = countsR/909.7 * wheelCirc;

  return (distanceL + distanceR)/2;
}

void resetEncoders(){
  encoders.getCountsAndResetLeft();
  encoders.getCountsAndResetRight();
}
