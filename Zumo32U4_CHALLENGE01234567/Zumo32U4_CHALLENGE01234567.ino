#include "Zumo32U4_CHALLENGE01234567.h"

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  switch(buttons.protocol) {
    case 0: challenge.zero();
      break;
    case 1: challenge.one();
      break;
    case 2: challenge.two();
      break;
    case 3: challenge.three();
      break;
    case 4: challenge.four();
      break;
    case 5: challenge.five();
      break;
    case 6: challenge.six();
      break;
    case 7: challenge.seven(); 
      break;
    default: // case 255:
      screen.show("Done.");
      buzzer.intermission();
      delay(1000);
      buttons.protocol = 0;
  }
}