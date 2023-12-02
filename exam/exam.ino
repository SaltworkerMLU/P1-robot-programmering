#include <Wire.h>
#include <Zumo32U4.h>

Zumo32U4OLED OLED;
Zumo32U4LineSensors lineSensors;
Zumo32U4ButtonA buttonA;
Zumo32U4ButtonB buttonB;
Zumo32U4ButtonC buttonC;
Zumo32U4Buzzer buzzer;
Zumo32U4Motors motors;

#define NUM_SENSORS 3
uint16_t lineSensorValues[NUM_SENSORS];

int taskValue = 0;
int speed = 100;
int threshold = 600;

void setup() {
  Serial.begin(9600);
  lineSensors.initThreeSensors();
}


void loop() {
  OLED.clear();
  OLED.print("A=+; B=-");
  OLED.gotoXY(0, 1);
  OLED.print("C=verify");
  
  challengeZero();

  switch(taskValue){
    case 1: 
      challengeOne();
      break;
    default: break;
  }
  
  taskValue = 0;
}

void challengeOne(){
  unsigned int lengthFromBackwall = 0;
  int maxLength = 41;

  //  1)  Find length from backwall
  OLED.clear();

  bool run = true;

  while (run){
    OLED.clear();
    OLED.print(lengthFromBackwall);

    bool runCounter = true;

    while (runCounter){
      if (buttonA.isPressed()){
        buttonA.waitForRelease();

        lengthFromBackwall++;
        runCounter = false;

        buzz();
      } else if (buttonB.isPressed()){
        buttonB.waitForRelease();

        lengthFromBackwall--;
        runCounter = false;

        buzz();
      } else if (buttonC.isPressed()){
        buttonC.waitForRelease();

        runCounter = false;
        run = false;

        buzz();
      }
    }
  }

  //  2)  Wait five seconds
  delay(5000);

  //  3)  Drive to line
  forward( speed );

  //  4)  Use alignZumo()


  //  5)  Drive until
}


void alignZumo(){
  bool detectLine = true;

  if (lineSensorValues[0] >= threshold){
    
  }

}


//  Functions to complete challenge 0 is placed at the button since we don't have to explain them.
void challengeZero(){
  bool checkAndClear = true;

  while ( checkAndClear ){

    //  Placed here to be true in each itteration.
    bool check = true;

    while ( check ){
      if (buttonA.isPressed()){

        buttonA.waitForRelease();
        incrTaskValue();

        check = false;

        buzz();
        
      } else if (buttonB.isPressed()){

        buttonB.waitForRelease();
        decrTaskValue();

        check = false;

        buzz();

      } else if (buttonC.isPressed()){

        buttonC.waitForRelease();

        checkAndClear = false;
        check = false;

        buzz();
      }
    }
    
    OLED.clear();
    OLED.print(taskValue);
  }
}
void incrTaskValue(){
  taskValue++;
  if (taskValue > 7){
    taskValue = 1;
  }
}
void decrTaskValue(){
  taskValue--;
  if (taskValue < 1 ){
    taskValue = 7;
  }
}
void buzz(){
  buzzer.playNote(300, 100, 10);
}
void forward( int spd ){
  motors.setSpeeds(spd, spd);
}
void stop(){
  motors.setSpeeds(0, 0);
}

