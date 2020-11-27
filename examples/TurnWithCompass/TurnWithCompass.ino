/* This example uses the 3pi+ 32U4 robot's onboard magnetometer to
 * help the make precise 90-degree turns and drive in squares.
 *
 * This program first calibrates the compass to account for offsets in
 * its output. Calibration is accomplished in setup().
 *
 * In loop(), The driving angle then changes its offset by 90 degrees
 * from the heading every second. Essentially, this navigates the
 * 3pi+ to drive in square patterns.
 *
 * It is important to note that stray magnetic fields from electric
 * current (including from the robot's own motors) and the environment
 * (for example, steel rebar in a concrete floor) might adversely
 * affect readings from the compass and make them less reliable.
 *
 * Note that the Hyper Edition does not drive very straight at the low
 * speeds used in this example, so it might not make a very accurate
 * square.
 */

#include <Pololu3piPlus32U4.h>
#include <PololuMenu.h>

/* The IMU is not fully enabled by default since it depends on the
Wire library, which uses about 1400 bytes of additional code space
and defines an interrupt service routine (ISR) that might be
incompatible with some applications (such as our TWISlave example).

Include Pololu3piPlus32U4IMU.h in one of your cpp/ino files to enable IMU
functionality.
*/
#include <Pololu3piPlus32U4IMU.h>

#define CALIBRATION_SAMPLES 70  // Number of compass readings to take when calibrating

// Allowed deviation (in degrees) relative to target angle that must be achieved before driving straight
#define DEVIATION_THRESHOLD 5

using namespace Pololu3piPlus32U4;

Motors motors;
Buzzer buzzer;
ButtonA buttonA;
ButtonB buttonB;
ButtonC buttonC;
LCD lcd;
IMU imu;

IMU::vector<int16_t> m_max; // maximum magnetometer values, used for calibration
IMU::vector<int16_t> m_min; // minimum magnetometer values, used for calibration

/* Configuration for specific 3pi+ editions: the Standard, Turtle, and
Hyper versions of 3pi+ have different motor configurations, requiring
the demo to be configured with different parameters for proper
operation.  The following functions set up these parameters using a
menu that runs at the beginning of the program.  To bypass the menu,
you can replace the call to selectEdition() in setup() with one of the
specific functions.
*/

uint16_t speedStraight; // Maximum motor speed when going straight; variable speed when turning
uint16_t turnBaseSpeed; // Base speed when turning (added to variable speed)

void selectHyper()
{
  motors.flipLeftMotor(true);
  motors.flipRightMotor(true);
  // Encoders are not used in this example.
  // encoders.flipEncoders(true);
  speedStraight = 70;
  turnBaseSpeed = 35;
}

void selectStandard()
{
  speedStraight = 100;
  turnBaseSpeed = 50;
}

void selectTurtle()
{
  speedStraight = 200;
  turnBaseSpeed = 100;
}

PololuMenu menu;

void selectEdition()
{
  lcd.clear();
  lcd.print(F("Select"));
  lcd.gotoXY(0,1);
  lcd.print(F("edition"));
  delay(1000);

  static const PololuMenu::Item items[] = {
    { F("Hyper"), selectHyper },
    { F("Standard"), selectStandard },
    { F("Turtle"), selectTurtle },
  };

  menu.setItems(items, 3);
  menu.setLcd(lcd);
  menu.setBuzzer(buzzer);
  menu.setButtons(buttonA, buttonB, buttonC);

  while(!menu.select());

  lcd.gotoXY(0,1);
  lcd.print("OK!  ...");
}

// Setup will calibrate our compass by finding maximum/minimum magnetic readings
void setup()
{
  // The highest possible magnetic value to read in any direction is 32767
  // The lowest possible magnetic value to read in any direction is -32767
  IMU::vector<int16_t> running_min = {32767, 32767, 32767}, running_max = {-32767, -32767, -32767};
  unsigned char index;

  Serial.begin(9600);

  // Initialize the Wire library and join the I2C bus as a master
  Wire.begin();

  // Initialize IMU
  imu.init();

  // Enables accelerometer and magnetometer
  imu.enableDefault();

  imu.configureForCompassHeading();

  // To bypass the menu, replace this function with
  // selectHyper(), selectStandard(), or selectTurtle().
  selectEdition();

  delay(1000);

  lcd.clear();
  lcd.print("starting");
  lcd.gotoXY(0,1);
  lcd.print("calib");
  Serial.println("starting calibration");

  // To calibrate the magnetometer, the 3pi+ spins to find the max/min
  // magnetic vectors. This information is used to correct for offsets
  // in the magnetometer data.
  motors.setLeftSpeed(speedStraight);
  motors.setRightSpeed(-speedStraight);

  for(index = 0; index < CALIBRATION_SAMPLES; index ++)
  {
    // Take a reading of the magnetic vector and store it in compass.m
    imu.readMag();

    running_min.x = min(running_min.x, imu.m.x);
    running_min.y = min(running_min.y, imu.m.y);

    running_max.x = max(running_max.x, imu.m.x);
    running_max.y = max(running_max.y, imu.m.y);

    Serial.println(index);

    delay(50);
  }

  motors.setLeftSpeed(0);
  motors.setRightSpeed(0);

  Serial.print("max.x   ");
  Serial.print(running_max.x);
  Serial.println();
  Serial.print("max.y   ");
  Serial.print(running_max.y);
  Serial.println();
  Serial.print("min.x   ");
  Serial.print(running_min.x);
  Serial.println();
  Serial.print("min.y   ");
  Serial.print(running_min.y);
  Serial.println();

  // Store calibrated values in m_max and m_min
  m_max.x = running_max.x;
  m_max.y = running_max.y;
  m_min.x = running_min.x;
  m_min.y = running_min.y;

  lcd.clear();
  lcd.print("Press A");
  buttonA.waitForButton();
}

void loop()
{
  float heading, relative_heading;
  int speed;
  static float target_heading = averageHeading();

  // Heading is given in degrees away from the magnetic vector, increasing clockwise
  heading = averageHeading();

  // This gives us the relative heading with respect to the target angle
  relative_heading = relativeHeading(heading, target_heading);

  Serial.print("Target heading: ");
  Serial.print(target_heading);
  Serial.print("    Actual heading: ");
  Serial.print(heading);
  Serial.print("    Difference: ");
  Serial.print(relative_heading);

  // If the 3pi+ has turned to the direction it wants to be pointing, go straight and then do another turn
  if(abs(relative_heading) < DEVIATION_THRESHOLD)
  {
    motors.setSpeeds(speedStraight, speedStraight);

    Serial.print("   Straight");

    delay(1000);

    // Turn off motors and wait a short time to reduce interference from motors
    motors.setSpeeds(0, 0);
    delay(100);

    // Turn 90 degrees relative to the direction we are pointing.
    // This will help account for variable magnetic field, as opposed
    // to using fixed increments of 90 degrees from the initial
    // heading (which might have been measured in a different magnetic
    // field than the one the 3pi+ is experiencing now).
    // Note: fmod() is floating point modulo
    target_heading = fmod(averageHeading() + 90, 360);
  }
  else
  {
    // To avoid overshooting, the closer the 3pi+ gets to the target
    // heading, the slower it should turn. Set the motor speeds to a
    // minimum base amount plus an additional variable amount based
    // on the heading difference.

    speed = speedStraight*relative_heading/180;

    if (speed < 0)
      speed -= turnBaseSpeed;
    else
      speed += turnBaseSpeed;

    motors.setSpeeds(speed, -speed);

    Serial.print("   Turn");
  }
  Serial.println();
}

// Converts x and y components of a vector to a heading in degrees.
// This calculation assumes that the 3pi+ is always level.
template <typename T> float heading(IMU::vector<T> v)
{
  float x_scaled =  2.0*(float)(v.x - m_min.x) / (m_max.x - m_min.x) - 1.0;
  float y_scaled =  2.0*(float)(v.y - m_min.y) / (m_max.y - m_min.y) - 1.0;

  float angle = atan2(y_scaled, x_scaled)*180 / M_PI;
  if (angle < 0)
    angle += 360;
  return angle;
}

// Yields the angle difference in degrees between two headings
float relativeHeading(float heading_from, float heading_to)
{
  float relative_heading = heading_to - heading_from;

  // constrain to -180 to 180 degree range
  if (relative_heading > 180)
    relative_heading -= 360;
  if (relative_heading < -180)
    relative_heading += 360;

  return relative_heading;
}

// Average 10 vectors to get a better measurement and help smooth out
// the motors' magnetic interference.
float averageHeading()
{
  IMU::vector<int32_t> avg = {0, 0, 0};

  for(int i = 0; i < 10; i ++)
  {
    imu.readMag();
    avg.x += imu.m.x;
    avg.y += imu.m.y;
  }
  avg.x /= 10.0;
  avg.y /= 10.0;

  // avg is the average measure of the magnetic vector.
  return heading(avg);
}
