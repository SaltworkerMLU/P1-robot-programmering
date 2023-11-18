#include <Wire.h>
#include <Zumo32U4.h>
Zumo32U4Motors motors;
Zumo32U4IMU imu;
//line sensors:
Zumo32U4LineSensors lineSensors;
#define NUM_SENSORS 3
uint16_t lineSensorValues[NUM_SENSORS];
int s1, s2, s3;

//variables:
int speed = 150;
int32_t turnAngle = 0;
uint16_t gyroLastUpdate = 0;
int16_t gyroOffset;
int16_t turnRate;
int threshold = 600;
bool alignment = false;
int parameter = 45; //angle in degrees
bool leftReachedTape = false, leftOverTape = false, leftDone = false; //booleans necessary for the align function
bool rightReachedTape = false, rightOverTape = false, rightDone = false; //booleans necessary for the align function

//necessary for display only:
Zumo32U4ButtonA buttonA;
Zumo32U4LCD lcd;

void readLineSensors(){
  lineSensors.read(lineSensorValues, QTR_EMITTERS_ON);
}
void setup() {
  turnSensorSetup();
  delay(500);
  turnSensorReset();
  lcd.clear();
  lineSensors.initThreeSensors();

}
bool aboveThreshold(){
readLineSensors();
if (lineSensorValues[0]>threshold || lineSensorValues[1]>threshold || lineSensorValues[2]>threshold){
  return true;
}
else{
  return false;
}
}
void printReadingsToSerial() //til kalibrering og test
{
  char buffer[80];
  sprintf(buffer, "%4d %4d %4d\n",
    lineSensorValues[0],
    lineSensorValues[1],
    lineSensorValues[2]
  );
  Serial.print(buffer);
}

void loop() {
/*    int32_t turnDegrees = getTurnAngleInDegrees();
    Serial.println("Degree: " + (String)turnDegrees);
*/
  readLineSensors();
  if(!alignment && !aboveThreshold()){
    forward(speed);
  }
  if(!alignment && aboveThreshold()){
    lcd.print(F("STOP"));
    stop();
    align();
    alignment=true;
    delay(2000);
    turnByDegree(parameter);
    forward(speed);
    delay(2000);
  }
  if(alignment && aboveThreshold()){
    stop();
  }
}

int32_t getTurnAngleInDegrees(){ //turnSensors
  turnSensorUpdate();
  // do some math and pointer magic to turn angle in seconds to angle in degree
  return (((int32_t)turnAngle >> 16) * 360) >> 16;
}

void turnSensorSetup()//turnSensors
{
  Wire.begin();
  imu.init();
  imu.enableDefault();
  imu.configureForTurnSensing();

//display only:
/*  lcd.clear();
  lcd.print(F("Gyro cal"));


  delay(500);
*/
  // Calibrate the gyro.
  int32_t total = 0;
  for (uint16_t i = 0; i < 1024; i++)
  {
    // Wait for new data to be available, then read it.
    while(!imu.gyroDataReady()) {}
    imu.readGyro();

    // Add the Z axis reading to the total.
    total += imu.g.z;
  }
  gyroOffset = total / 1024;

  // Display the angle (in degrees from -180 to 180) until the
  // user presses A.
/*  lcd.clear();
  turnSensorReset();
  while (!buttonA.getSingleDebouncedRelease())
  {
    turnSensorUpdate();
    lcd.gotoXY(0, 0);
    lcd.print((((int32_t)turnAngle >> 16) * 360) >> 16);
    lcd.print(F("   "));
  }
  lcd.clear();
  */
}

void turnSensorReset()//turnSensors
{
  gyroLastUpdate = micros();
  turnAngle = 0;
}

void turnSensorUpdate()//turnSensors
{
  imu.readGyro();
  turnRate = imu.g.z - gyroOffset;

  turnAngle += (int64_t)d * 14680064 / 17578125;
}
void forward(int spd){//movement
  motors.setSpeeds(spd,spd);
}
void stop(){//movement
  motors.setSpeeds(0,0);
}
void turn(int spd){
  motors.setSpeeds(spd,-spd);
}
void align(){
if (lineSensorValues[0] >= threshold && !leftReachedTape){         //  Aligns from the left side.
  leftReachedTape = true;
  stop();
  checkOver();
  } 
else if (lineSensorValues[2] >= threshold && !rightReachedTape){        //  Aligns from the right side
  rightReachedTape = true;
  stop();
  checkOver();
}
}
void checkOver(){
  delay(500);
  forward(speed/2);

  bool run = true;

  while(run){
    readLineSensors();
    if(leftReachedTape && lineSensorValues[2] >= threshold){
      stop();
      rightReachedTape = true;
      run = false;
    } else if (rightReachedTape && lineSensorValues[0] >= threshold){
      stop();
      leftReachedTape = true;
      run = false;
    }
  }

  delay(1000);

  run = true;

  forward(speed);

  while (run){
    readLineSensors();

    if (!leftDone || !rightDone){   //  Runs if and only if one of the sides is not aligned.
      //  Checks left side
      if (leftReachedTape && lineSensorValues[0] < threshold){      //  Goes back if over tape.
        motors.setLeftSpeed(-speed);
        leftOverTape = true;
        leftDone = false;
      } else if(leftOverTape && lineSensorValues[0] >= threshold){  //  Only stops if it have been over the tape once.
 
        motors.setLeftSpeed(0);
        leftDone = true;
      }

      //  Checks right side
      if (rightReachedTape && lineSensorValues[2] < threshold){     //  Goes back if over tape.
        motors.setRightSpeed(-speed);
        rightOverTape = true;
        rightDone = false;
      } else if (rightOverTape && lineSensorValues[2] >= threshold){  //  Only stops if it have been over the tape once.
       
        motors.setRightSpeed(0);
        rightDone = true;
      }
    } else {    //  Stops aligning the Zumo when both sides have stopped.
        run = false;
    }
  }
}
void turnByDegree(int degree){
turnSensorReset();
float angle = getTurnAngleInDegrees();
if (degree<0){
  while (degree<angle){
    turn(speed);
    angle = getTurnAngleInDegrees();
  }
}
if (parameter>0){
  while (parameter>angle){
    turn(-speed);
    angle = getTurnAngleInDegrees();
  }}
stop();
}
