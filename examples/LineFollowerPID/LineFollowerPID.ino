/* This example uses the line sensors on the 3pi+ 32U4 to follow
a black line on a white background, using a PID-based algorithm.
It works well on courses with smooth, 6" radius curves and can
even work with tighter turns, including sharp 90 degree corners.
This example has been tested with robots using 30:1 MP motors.
Modifications might be required for it to work well on different
courses or with different motors. */

#include <Tpp32U4.h>

// This is the maximum speed the motors will be allowed to turn.
// A maxSpeed of 400 would let the motors go at top speed, but
// the value of 200 here imposes a speed limit of 50%.

// Note that making the 3pi+ go faster on a line following course
// might involve more than just increasing this number; you will
// often have to adjust the PID constants too for it to work well.
const uint16_t maxSpeed = 200;

Tpp32U4Buzzer buzzer;
Tpp32U4LineSensors lineSensors;
Tpp32U4Motors motors;
Tpp32U4ButtonB buttonB;
Tpp32U4LCD lcd;

int16_t lastError = 0;

#define NUM_SENSORS 5
unsigned int lineSensorValues[NUM_SENSORS];

// Sets up special characters in the LCD so that we can display
// bar graphs.
void loadCustomCharacters()
{
  static const char levels[] PROGMEM = {
    0, 0, 0, 0, 0, 0, 0, 63, 63, 63, 63, 63, 63, 63
  };
  lcd.loadCustomCharacter(levels + 0, 0);  // 1 bar
  lcd.loadCustomCharacter(levels + 1, 1);  // 2 bars
  lcd.loadCustomCharacter(levels + 2, 2);  // 3 bars
  lcd.loadCustomCharacter(levels + 3, 3);  // 4 bars
  lcd.loadCustomCharacter(levels + 4, 4);  // 5 bars
  lcd.loadCustomCharacter(levels + 5, 5);  // 6 bars
  lcd.loadCustomCharacter(levels + 6, 6);  // 7 bars
}

void printBar(uint8_t height)
{
  if (height > 8) { height = 8; }
  const char barChars[] = {' ', 0, 1, 2, 3, 4, 5, 6, 255};
  lcd.print(barChars[height]);
}

void calibrateSensors()
{
  lcd.clear();

  // Wait 1 second and then begin automatic sensor calibration
  // by rotating in place to sweep the sensors over the line
  delay(1000);
  for(uint16_t i = 0; i < 80; i++)
  {
    if (i > 20 && i <= 60)
    {
      motors.setSpeeds(-60, 60);
    }
    else
    {
      motors.setSpeeds(60, -60);
    }

    lineSensors.calibrate();
  }
  motors.setSpeeds(0, 0);
}

// Displays the estimated line position and a bar graph of sensor
// readings on the LCD. Returns after the user presses B.
void showReadings()
{
  lcd.clear();

  while(!buttonB.getSingleDebouncedPress())
  {
    uint16_t position = lineSensors.readLineBlack(lineSensorValues);

    lcd.clear();
    lcd.gotoXY(0, 0);
    lcd.print(position);
    lcd.gotoXY(0, 1);
    for (uint8_t i = 0; i < NUM_SENSORS; i++)
    {
      uint8_t barHeight = map(lineSensorValues[i], 0, 1000, 0, 8);
      printBar(barHeight);
    }

    delay(50);
  }
}

void setup()
{
  // Uncomment if necessary to correct motor directions:
  //motors.flipLeftMotor(true);
  //motors.flipRightMotor(true);

  loadCustomCharacters();

  // Play a little welcome song
  buzzer.play(">g32>>c32");

  // Wait for button B to be pressed and released.
  lcd.clear();
  lcd.print(F("Press B"));
  lcd.gotoXY(0, 1);
  lcd.print(F("to calib"));
  buttonB.waitForButton();

  calibrateSensors();

  showReadings();

  // Play music and wait for it to finish before we start driving.
  lcd.clear();
  lcd.print(F("Go!"));
  buzzer.play("L16 cdegreg4");
  while(buzzer.isPlaying());
}

void loop()
{
  // Get the position of the line.  Note that we *must* provide
  // the "lineSensorValues" argument to readLineBlack() here, even
  // though we are not interested in the individual sensor
  // readings.
  int16_t position = lineSensors.readLineBlack(lineSensorValues);

  // Our "error" is how far we are away from the center of the
  // line, which corresponds to position 2000.
  int16_t error = position - 2000;

  // Get motor speed difference using proportional and derivative
  // PID terms (the integral term is generally not very useful
  // for line following).  Here we are using a proportional
  // constant of 1/4 and a derivative constant of 7/2 (3.5), which
  // should work well for a 3pi+ with 30:1 MP motors.  You
  // probably want to use trial and error to tune these constants
  // for your particular 3pi+ and line course.
  int16_t speedDifference = error / 4 + (error - lastError) * 7/2;

  lastError = error;

  // Get individual motor speeds.  The sign of speedDifference
  // determines if the robot turns left or right.
  int16_t leftSpeed = (int16_t)maxSpeed + speedDifference;
  int16_t rightSpeed = (int16_t)maxSpeed - speedDifference;

  // Constrain our motor speeds to be between 0 and maxSpeed.
  // One motor will always be turning at maxSpeed, and the other
  // will be at maxSpeed-|speedDifference| if that is positive,
  // else it will be stationary.  For some applications, you
  // might want to allow the motor speed to go negative so that
  // it can spin in reverse.
  leftSpeed = constrain(leftSpeed, 0, (int16_t)maxSpeed);
  rightSpeed = constrain(rightSpeed, 0, (int16_t)maxSpeed);

  motors.setSpeeds(leftSpeed, rightSpeed);
}
