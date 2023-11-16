#include "Zumo32U4_components.h"

struct zumo : protected lcd, protected buttons, protected encoders, protected imu, 
              protected lineSensors, protected proxSensors, protected motors {
  int speed = 150;
  int threshold = 600;
  bool alignment = false;
  int parameter = 45; //angle in degrees
  bool leftReachedTape = false, leftOverTape = false, leftDone = false; //booleans necessary for the align function
  bool rightReachedTape = false, rightOverTape = false, rightDone = false; //booleans necessary for the align function

  bool aboveThreshold();

  void turnByDegree(int degree);

  /*! Align function.
   *  As it is heavily used, it will be a reserved function for this struct. */
   void align();

  void checkOver();
    
};

/*! This struct contains all 8 challenges from 0 to 7. */
struct challenge : private zumo {
  /*! CHALLENGE ZERO.
   *  You shall program the Zumo to have a programmable interface using 
   *  - the encoders.
   *  - the LCD/OLED screen.
   *  - the buttons.
   *  so that you can launch set solutions with set parameters. */
  int zero();

  void alignZumo(String side);

  /*! CHALLENGE ONE.
   *  The Zumo shall be able to.
   *  1. Drive to the white line.
   *  2. Align itself.
   *  3. Then drive.
   *  4. untill it is at a specific distance...
   *     ...to the back wall at the end of the hallway. */
  void one();

  /*! CHALLENGE TWO.
   *  The Zumo starts on one end of the whiteline. 
   *  The Zumo shall be able to follow the line all the way to the other end...
   *  ...without skipping major sections. */
  void two();

  /*! CHALLENGE THREE.
   *  The Zumo shall.
   *  1. Drive in between the walls with equal distance to the walls at each side.
   *  2. Untill detection of the white line at the far end of the challenge. */
  void three();

  /*! CHALLENGE FOUR.
   *  The Zumo shall. 
   *  1. Be able to drive to the whiteline. 
   *  2. Align it self.
   *  3. Drive a set distance straight forward. */
  void four();

  /*! CHALLGEN FIVE.
   *  The Zumo shall 
   *  1. Drive slalom between the cylinders.
   *  2. Whilst using the white lines to count successfull passes from line to line on the display. 
   *  You are challenged to get as many successfull passes as possible in a set amountof time. */
  void five();

  /*! CHALLENGE SIX.
   *  The Zumo shall 
   *  1. Be able to drive to the whiteline.
   *  2. Align it self.
   *  3. Turn the a set amount of degrees
   *  4. Before driving forward untill detecting the other white line. */
  void six();

 /*! CHALLENGE SEVEN.
  *  The Zumo shall.
  *  1. Be able to drive to the whiteline.
  *  2. Align it self 
  *  3. Drive straight. 
  *  4. Until hitting the whiteline at the other end of the board. */
  void seven();

  /*! Indicates challenge start and finish. */
  void intermission(uint8_t attention=10, int windup=800);
};