/* This demo shows how the 3pi+ can use its gyroscope to detect
when it is being rotated, and use the motors to resist that
rotation.

Be careful to not move the robot for a few seconds after starting
it while the gyro is being calibrated.  During the gyro
calibration, the yellow LED is on and the words "Gyro cal" are
displayed on the display.

After the gyro calibration is done, press button A to start the
demo.  If you try to turn the 3pi+, or put it on a surface that
is turning, it will drive its motors to counteract the turning.

This demo only uses the Z axis of the gyro, so it is possible to
pick up the 3pi+, rotate it about its X and Y axes, and then put
it down facing in a new position. */

#include <Wire.h>
#include <Pololu3piPlus32U4.h>
#include <PololuMenu.h>

/* The IMU is not fully enabled by default since it depends on the
Wire library, which uses about 1400 bytes of additional code space
and defines an interrupt service routine (ISR) that might be
incompatible with some applications (such as our TWISlave example).

Include Pololu3piPlus32U4IMU.h in one of your cpp/ino files to
enable IMU functionality.
*/
#include <Pololu3piPlus32U4IMU.h>

using namespace Pololu3piPlus32U4;

// Change next line to this if you are using the older 3pi+
// with a black and green LCD display:
// LCD display;
OLED display;

Buzzer buzzer;
ButtonA buttonA;
ButtonB buttonB;
ButtonC buttonC;
Motors motors;
IMU imu;

#include "TurnSensor.h"

/* Configuration for specific 3pi+ editions: the Standard, Turtle, and
Hyper versions of 3pi+ have different motor configurations, requiring
the demo to be configured with different parameters for proper
operation.  The following functions set up these parameters using a
menu that runs at the beginning of the program.  To bypass the menu,
you can replace the call to selectEdition() in setup() with one of the
specific functions.
*/

// This is the maximum speed the motors will be allowed to turn.
// A maxSpeed of 400 lets the motors go at top speed.  Decrease
// this value to impose a speed limit.
int16_t maxSpeed;

void selectHyper()
{
  motors.flipLeftMotor(true);
  motors.flipRightMotor(true);
  // Encoders are not used in this example.
  // encoders.flipEncoders(true);
  maxSpeed = 100;
}

void selectStandard()
{
  maxSpeed = 200;
}

void selectTurtle()
{
  maxSpeed = 400;
}

PololuMenu<typeof(display)> menu;

void selectEdition()
{
  display.clear();
  display.print(F("Select"));
  display.gotoXY(0,1);
  display.print(F("edition"));
  delay(1000);

  static const PololuMenuItem items[] = {
    { F("Standard"), selectStandard },
    { F("Turtle"), selectTurtle },
    { F("Hyper"), selectHyper },
  };

  menu.setItems(items, 3);
  menu.setDisplay(display);
  menu.setBuzzer(buzzer);
  menu.setButtons(buttonA, buttonB, buttonC);

  while(!menu.select());

  display.gotoXY(0,1);
  display.print("OK!  ...");
}

void setup()
{
  // To bypass the menu, replace this function with
  // selectHyper(), selectStandard(), or selectTurtle().
  selectEdition();

  // Delay before calibrating the gyro.
  delay(1000);

  turnSensorSetup();
  turnSensorReset();

  display.clear();
  display.print(F("Try to"));
  display.gotoXY(0, 1);
  display.print(F("turn me!"));
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
