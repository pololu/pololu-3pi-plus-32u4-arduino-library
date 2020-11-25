/* This demo uses the Tpp 32U4's accelerometer to detect
whether it is on a slanted surface.  If it is on a slanted
surface, then it uses the motors to face uphill.

It also uses the encoders to avoid rolling down the surface.

Please note that this example is not very robust and it might be
hard to modify it to behave differently.  The accelerometer
readings are affected by the movement of the Tpp, so if you
change the code for controlling the motors, you might also affect
the accelerometer readings.

This code was tested on a Tpp with 4 NiMH batteries and two 75:1
HP micro metal gearmotors.  If you have different batteries or
motors, you might need to adjust the formula for computing
turnSpeed.

Also, if the robot is pointing directly downhill, it might not
move, because the y component of the acceleration would be close
to 0. */

#include <Wire.h>
#include <Tpp32U4.h>

/* The IMU is not fully enabled by default since it depends on the
Wire library, which uses about 1400 bytes of additional code space
and defines an interrupt service routine (ISR) that might be
incompatible with some applications (such as our TWISlave example).

Include Tpp32U4IMU.h in one of your cpp/ino files to enable IMU
functionality.
*/
#include <Tpp32U4IMU.h>

const int16_t maxSpeed = 100;

Tpp32U4IMU imu;
Tpp32U4Motors motors;
Tpp32U4LCD lcd;
Tpp32U4ButtonA buttonA;
Tpp32U4Encoders encoders;

void setup()
{
  // Start I2C and initialize the IMU sensors.
  Wire.begin();
  imu.init();
  imu.enableDefault();
  imu.configureForFaceUphill();

  lcd.clear();
  lcd.print(F("Press A"));
  buttonA.waitForPress();
  lcd.clear();
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
    lcd.gotoXY(0, 0);
    lcd.print(x);
    lcd.print(F("       "));
    lcd.gotoXY(0, 1);
    lcd.print(y);
    lcd.print(F("       "));
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