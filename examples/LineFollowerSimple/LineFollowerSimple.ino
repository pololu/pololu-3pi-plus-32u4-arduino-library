/* This example uses the line sensors on the 3pi+ 32U4 to follow
a black line on a white background, using a simple conditional-
based algorithm. */

#include <Pololu3piPlus32U4.h>
#include <PololuMenu.h>

using namespace Pololu3piPlus32U4;

// Change next line to this if you are using the older 3pi+
// with a black and green LCD display:
// LCD display;
OLED display;

Buzzer buzzer;
LineSensors lineSensors;
Motors motors;
ButtonA buttonA;
ButtonB buttonB;
ButtonC buttonC;

#define NUM_SENSORS 5
unsigned int lineSensorValues[NUM_SENSORS];

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
uint16_t maxSpeed;

uint16_t calibrationSpeed;

void selectHyper()
{
  motors.flipLeftMotor(true);
  motors.flipRightMotor(true);
  // Encoders are not used in this example.
  // encoders.flipEncoders(true);
  maxSpeed = 75;
  calibrationSpeed = 50;
}

void selectStandard()
{
  maxSpeed = 100;
  calibrationSpeed = 60;
}

void selectTurtle()
{
  maxSpeed = 200;
  calibrationSpeed = 120;
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

// Sets up special characters in the LCD so that we can display
// bar graphs.
void loadCustomCharacters()
{
  static const char levels[] PROGMEM = {
    0, 0, 0, 0, 0, 0, 0, 63, 63, 63, 63, 63, 63, 63
  };
  display.loadCustomCharacter(levels + 0, 0);  // 1 bar
  display.loadCustomCharacter(levels + 1, 1);  // 2 bars
  display.loadCustomCharacter(levels + 2, 2);  // 3 bars
  display.loadCustomCharacter(levels + 3, 3);  // 4 bars
  display.loadCustomCharacter(levels + 4, 4);  // 5 bars
  display.loadCustomCharacter(levels + 5, 5);  // 6 bars
  display.loadCustomCharacter(levels + 6, 6);  // 7 bars
}

void printBar(uint8_t height)
{
  if (height > 8) { height = 8; }
  const char barChars[] = {' ', 0, 1, 2, 3, 4, 5, 6, (char)255};
  display.print(barChars[height]);
}

void calibrateSensors()
{
  display.clear();

  // Wait 1 second and then begin automatic sensor calibration
  // by rotating in place to sweep the sensors over the line
  delay(1000);
  for(uint16_t i = 0; i < 80; i++)
  {
    if (i > 20 && i <= 60)
    {
      motors.setSpeeds(-(int16_t)calibrationSpeed, calibrationSpeed);
    }
    else
    {
      motors.setSpeeds(calibrationSpeed, -(int16_t)calibrationSpeed);
    }

    lineSensors.calibrate();
  }
  motors.setSpeeds(0, 0);
}

// Displays the estimated line position and a bar graph of sensor readings on
// the LCD. Returns after the user presses B.
void showReadings()
{
  display.clear();

  while(!buttonB.getSingleDebouncedPress())
  {
    uint16_t position = lineSensors.readLineBlack(lineSensorValues);

    display.gotoXY(0, 0);
    display.print(position);
    display.print("    ");
    display.gotoXY(0, 1);
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

  // To bypass the menu, replace this function with
  // selectHyper(), selectStandard(), or selectTurtle().
  selectEdition();

  // Wait for button B to be pressed and released.
  display.clear();
  display.print(F("Press B"));
  display.gotoXY(0, 1);
  display.print(F("to calib"));
  while(!buttonB.getSingleDebouncedPress());

  calibrateSensors();

  showReadings();

  // Play music and wait for it to finish before we start driving.
  display.clear();
  display.print(F("Go!"));
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
    motors.setSpeeds(0, maxSpeed);

    // Just for fun, indicate the direction we are turning on
    // the LEDs.
    ledYellow(1);
    ledRed(0);
  }
  else if (position < 3000)
  {
    // We are somewhat close to being centered on the line:
    // drive straight.
    motors.setSpeeds(maxSpeed, maxSpeed);
    ledYellow(1);
    ledRed(1);
  }
  else
  {
    // We are far to the left of the line: turn right.
    motors.setSpeeds(maxSpeed, 0);
    ledYellow(0);
    ledRed(1);
  }
}
