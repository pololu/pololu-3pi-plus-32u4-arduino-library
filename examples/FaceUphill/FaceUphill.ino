/* This demo uses the 3pi+ 32U4's accelerometer to detect
whether it is on a slanted surface.  If it is on a slanted
surface, then it uses the motors to face uphill.

It also uses the encoders to avoid rolling down the surface.

Please note that this example is not very robust and it might be
hard to modify it to behave differently.  The accelerometer
readings are affected by the movement of the 3pi+, so if you
change the code for controlling the motors, you might also affect
the accelerometer readings.

Also, if the robot is pointing directly downhill, it might not
move, because the y component of the acceleration would be close
to 0. */

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

IMU imu;
Motors motors;
Buzzer buzzer;
ButtonA buttonA;
ButtonB buttonB;
ButtonC buttonC;
Encoders encoders;

/* Configuration for specific 3pi+ editions: the Standard, Turtle, and
Hyper versions of 3pi+ have different motor configurations, requiring
the demo to be configured with different parameters for proper
operation.  The following functions set up these parameters using a
menu that runs at the beginning of the program.  To bypass the menu,
you can replace the call to selectEdition() in setup() with one of the
specific functions.
*/
int16_t maxSpeed;

void selectHyper()
{
  motors.flipLeftMotor(true);
  motors.flipRightMotor(true);
  encoders.flipEncoders(true);
  maxSpeed = 75;
}

void selectStandard()
{
  maxSpeed = 100;
}

void selectTurtle()
{
  maxSpeed = 200;
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
  // Start I2C and initialize the IMU sensors.
  Wire.begin();
  imu.init();
  imu.enableDefault();
  imu.configureForFaceUphill();

  // To bypass the menu, replace this function with
  // selectHyper(), selectStandard(), or selectTurtle().
  selectEdition();

  // Delay before running motors
  delay(1000);
}

void loop()
{
  // Read the accelerometer.
  // A value of 16384 corresponds to approximately 1 g.
  imu.readAcc();
  int16_t x = imu.a.x;
  int16_t y = imu.a.y;
  int32_t magnitudeSquared = (int32_t)x * x + (int32_t)y * y;

  // Display the X and Y acceleration values on the LCD
  // every 150 ms.
  static uint8_t lastDisplayTime;
  if ((uint8_t)(millis() - lastDisplayTime) > 150)
  {
    lastDisplayTime = millis();
    display.gotoXY(0, 0);
    display.print(x);
    display.print(F("       "));
    display.gotoXY(0, 1);
    display.print(y);
    display.print(F("       "));
  }

  // Use the encoders to see how much we should drive forward.
  // If the robot rolls downhill, the encoder counts will become
  // negative, resulting in a positive forwardSpeed to counteract
  // the rolling.
  int16_t forwardSpeed = -(encoders.getCountsLeft() + encoders.getCountsRight());
  forwardSpeed = constrain(forwardSpeed, -maxSpeed, maxSpeed);

  // See if we are actually on an incline.
  // 16384 * sin(5 deg) = 1427
  int16_t turnSpeed;
  if (magnitudeSquared > (int32_t)1427 * 1427)
  {
    // We are on an incline of more than 5 degrees, so
    // try to face uphill using a feedback algorithm.

    if (x < 0)
    {
      if (y < 0)
      {
        turnSpeed = -maxSpeed;
      }
      else
      {
        turnSpeed = maxSpeed;
      }
    }
    else
    {
      turnSpeed = y / 40;
    }

    ledYellow(1);
  }
  else
  {
    // We not on a noticeable incline, so don't turn.
    turnSpeed = 0;
    ledYellow(0);
  }

  // To face uphill, we need to turn so that the X acceleration
  // is negative and the Y acceleration is 0.  Therefore, when
  // the Y acceleration is positive, we want to turn to the
  // left (counter-clockwise).
  int16_t leftSpeed = forwardSpeed - turnSpeed;
  int16_t rightSpeed = forwardSpeed + turnSpeed;

  // Constrain the speeds to be between -maxSpeed and maxSpeed.
  leftSpeed = constrain(leftSpeed, -maxSpeed, maxSpeed);
  rightSpeed = constrain(rightSpeed, -maxSpeed, maxSpeed);

  motors.setSpeeds(leftSpeed, rightSpeed);
}
