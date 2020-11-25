/* This demo shows how the 3pi+ can use its gyroscope to detect
when it is being rotated, and use the motors to resist that
rotation.

This code was tested on a 3pi+ with 4 NiMH batteries and two 75:1
HP micro metal gearmotors.  If you have different batteries or
motors, you might need to adjust the PID constants.

Be careful to not move the robot for a few seconds after starting
it while the gyro is being calibrated.  During the gyro
calibration, the yellow LED is on and the words "Gyro cal" are
displayed on the LCD.

After the gyro calibration is done, press button A to start the
demo.  If you try to turn the 3pi+, or put it on a surface that
is turning, it will drive its motors to counteract the turning.

This demo only uses the Z axis of the gyro, so it is possible to
pick up the 3pi+, rotate it about its X and Y axes, and then put
it down facing in a new position. */

#include <Wire.h>
#include <Pololu3piPlus32U4.h>

/* The IMU is not fully enabled by default since it depends on the
Wire library, which uses about 1400 bytes of additional code space
and defines an interrupt service routine (ISR) that might be
incompatible with some applications (such as our TWISlave example).

Include Pololu3piPlus32U4IMU.h in one of your cpp/ino files to
enable IMU functionality.
*/
#include <Pololu3piPlus32U4IMU.h>

#include "TurnSensor.h"

// This is the maximum speed the motors will be allowed to turn.
// A maxSpeed of 400 lets the motors go at top speed.  Decrease
// this value to impose a speed limit.
const int16_t maxSpeed = 400;

using namespace Pololu3piPlus32U4;

LCD lcd;
ButtonA buttonA;
Motors motors;
IMU imu;

void setup()
{
  turnSensorSetup();
  delay(500);
  turnSensorReset();

  lcd.clear();
  lcd.print(F("Try to"));
  lcd.gotoXY(0, 1);
  lcd.print(F("turn me!"));
}

void loop()
{
  // Read the gyro to update turnAngle, the estimation of how far
  // the robot has turned, and turnRate, the estimation of how
  // fast it is turning.
  turnSensorUpdate();

  // Calculate the motor turn speed using proportional and
  // derivative PID terms.  Here we are a using a proportional
  // constant of 28 and a derivative constant of 1/40.
  int32_t turnSpeed = -(int32_t)turnAngle / (turnAngle1 / 28)
    - turnRate / 40;

  // Constrain our motor speeds to be between
  // -maxSpeed and maxSpeed.
  turnSpeed = constrain(turnSpeed, -maxSpeed, maxSpeed);

  motors.setSpeeds(-turnSpeed, turnSpeed);
}
