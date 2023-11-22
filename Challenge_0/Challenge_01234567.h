#include "Zumo32U4_components.h"

/*! This struct makes use of the display equipped on the Zumo.
 *  Switch between inheriting Zumo32U4LCD and Zumo32U4OLED depending on the display on the used Zumo. */
struct display : protected Zumo32U4OLED {};

/*! This struct makes use of the buzzer */
struct buzzer : protected Zumo32U4Buzzer {};

/*! This struct inherits each defined struct from "Zumo32U4_components.h" as well as the buzzer  */
struct zumo : protected display, protected buzzer, protected buttons, protected encoders, 
              protected imu, protected lineSensors, protected proxSensors, protected motors {
  /*!  */
  int speed = 100;

  /*! The great decider between whether a line is white/black */
  int threshold = 600;

  /*! Zumo turn angle in degrees. Defined in challenge::selectParameters() */
  float turnParameter;

  /*! Zumo drive forward in cm. Defined in challenge::selectParameters() */
  float moveParameter;

  /*! Checks if either of the line sensors of the Zumo surpass the desired threshold */
  bool aboveThreshold();

  /*! Makes the Zumo align to a line ahead of it. */
  void align();
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

  /*! Select parameters moveParameter and turnParameter */
  void selectParameters();
};