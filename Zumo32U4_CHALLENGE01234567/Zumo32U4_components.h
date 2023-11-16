#include "Arduino.h" // Needed for any header file to run arduino code
#include <Wire.h>
#include <Zumo32U4.h>

struct oled : public Zumo32U4OLED {};

struct lcd : public Zumo32U4LCD {};

struct buzzer : public Zumo32U4Buzzer {};

/*! This struct makes use of all 3 buttons. */
struct buttons : public Zumo32U4ButtonA, public Zumo32U4ButtonB, public Zumo32U4ButtonC {
  uint8_t protocol = 0;

  /*! While neither button A, B, nor C has been pressed: Do nothing. */
  void pressAnyKeyToContinue();

  /*! Function to acquire variable protocol. Update protocol by pressing and releasing:
   *  * buttonA -> Add 1 to protocol.
   *  * buttonB -> Subtract 1 to protocol.
   *  * buttonC -> end function. 
   *  Whilst function is running, keep protocol in range 1-7 and show variable protocol on display. */
  bool getProtocol();
};

struct lineSensors : public Zumo32U4LineSensors {
  /*! Line sensor values are stored here after lineSensors.read() has been called. */
  uint16_t value[5];

  /*! Stored threshold regarding black/white. */
  int threshold = 800;

  /*! CONSTRUCTOR. It executes as the very first thing when struct object called. */
  lineSensors();

  /*! Reads line follower sensors and updates value[3]. */
  void read();
};

struct proxSensors : public Zumo32U4ProximitySensors {
  /*! Proximity sensor values are stored here after proxSensors.read() has been called. */
  uint8_t value[6];

  /*! Length of array brightness. Note how it is a "const".
   *  Because it is in a struct, "static" has to be put first. */
  static const uint8_t levelTwo = 20;

  /*! Array of brightness levels. */
  uint16_t brightnessTwo[levelTwo];

  /*! Length of array brightness. Note how it is a "const".
   *  Because it is in a struct, "static" has to be put first. */
  static const uint8_t levelFive = 13;

  /*! Array of brightness levels. */
  uint16_t brightnessFive[levelFive];

  /*! CONSTRUCTOR. It executes as the very first thing when struct object called. */
  proxSensors();

  /*! Reads proximity sensors and updates value[6]. */
  void read();

  /*! Returns error of side sensors. */
  float getError();

  /*! Returns error of front sensors. */
  float getErrorFront();
};

/*! This struct represents the Zumo32U4 IMU */
struct imu : public Zumo32U4IMU {
  //These values are explicitly used for the functions calibrateTurn() & dAngle()
  int16_t offset; // Average reading using calibrateTurn()... an offset
  uint16_t lastUpdate; // Stores old time in microseconds using micros()
  uint32_t turnAngle = 0; // Turn angle which needs further conversion
  uint32_t zumoAngle = 0; // Zumo32U4 turn angle (THE ONE)

  /*! Initializing the IMU takes multiple commands, thus this singular function has been made */
  void initMAG();
  
  void reset();

  /*! Takes measurements of gyrometer z-axis {iterations} amount of times. Then updates several struct variables */
  void calibrateTurn(int iterations=1000);

  /*! Used to update zumoAngle. 
   *  DO NOT DELAY CODE WHILST USING THIS USING e.g. delay(). Inconsistencies will occur. */
  void dAngle();

  int gyroAdjust();
};

struct motors : public Zumo32U4Motors {
  void forward(int speed=100);
  void stop();
  void left();
  void right();
  void turn(int spd);
};

struct encoders : public Zumo32U4Encoders {
  int16_t value[2];

  void read();

  void reset();

  float getDistance();
};