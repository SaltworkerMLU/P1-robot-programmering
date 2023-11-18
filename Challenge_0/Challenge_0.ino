#include "Challenge_01234567.h"

challenge challenge;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  switch(challenge.zero()) {
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
      challenge.intermission();
  }
}