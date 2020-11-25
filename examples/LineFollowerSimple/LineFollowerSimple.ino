/* This example uses the line sensors on the 3pi+ 32U4 to follow
a black line on a white background, using a simple conditional-
based algorithm. */

#include <Pololu3piPlus32U4.h>

// This is the maximum speed the motors will be allowed to turn.
// A maxSpeed of 400 lets the motors go at top speed.  Decrease
// this value to impose a speed limit.
const uint16_t maxSpeed = 200;

using namespace Pololu3piPlus32U4;

Buzzer buzzer;
LineSensors lineSensors;
Motors motors;
ButtonB buttonB;
LCD lcd;

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
  const char barChars[] = {' ', 0, 1, 2, 3, 4, 5, 6, (char)255};
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

// Displays the estimated line position and a bar graph of sensor readings on
// the LCD. Returns after the user presses B.
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

  if (position < 1000)
  {
    // We are far to the right of the line: turn left.

    // Set the right motor to 100 and the left motor to zero,
    // to do a sharp turn to the left.  Note that the maximum
    // value of either motor speed is 400, so we are driving
    // it at just about 25% of the max.
    motors.setSpeeds(0, 100);

    // Just for fun, indicate the direction we are turning on
    // the LEDs.
    ledYellow(1);
    ledRed(0);
  }
  else if (position < 3000)
  {
    // We are somewhat close to being centered on the line:
    // drive straight.
    motors.setSpeeds(100, 100);
    ledYellow(1);
    ledRed(1);
  }
  else
  {
    // We are far to the left of the line: turn right.
    motors.setSpeeds(100, 0);
    ledYellow(0);
    ledRed(1);
  }
}
