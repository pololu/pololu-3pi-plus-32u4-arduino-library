/* Load this example to make the 3pi+ 32U4 drive forward until it hits
a wall, detect the collision with its bumpers, then reverse, turn, and
keep driving.
*/

#include <Pololu3piPlus32U4.h>
#include <PololuMenu.h>

using namespace Pololu3piPlus32U4;

// Change next line to this if you are using the older 3pi+
// with a black and green LCD display:
// LCD display;
OLED display;

BumpSensors bumpSensors;
Buzzer buzzer;
Motors motors;
ButtonA buttonA;
ButtonB buttonB;
ButtonC buttonC;

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

int16_t turnTime;

void selectHyper()
{
  motors.flipLeftMotor(true);
  motors.flipRightMotor(true);
  // Encoders are not used in this example.
  // encoders.flipEncoders(true);
  maxSpeed = 75;
  turnTime = 150;
}

void selectStandard()
{
  maxSpeed = 100;
  turnTime = 250;
}

void selectTurtle()
{
  maxSpeed = 200;
  turnTime = 500;
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

  bumpSensors.calibrate();
  delay(1000);
  display.clear();
}

void loop()
{
  motors.setSpeeds(maxSpeed, maxSpeed);
  bumpSensors.read();

  if (bumpSensors.leftIsPressed())
  {
    // Left bump sensor is pressed.
    ledYellow(true);
    motors.setSpeeds(0, 0);
    buzzer.play("a32");
    display.gotoXY(0, 0);
    display.print('L');

    motors.setSpeeds(maxSpeed, -maxSpeed);
    delay(turnTime);

    motors.setSpeeds(0, 0);
    buzzer.play("b32");
    ledYellow(false);
    display.gotoXY(0, 0);
    display.print(' ');
  }
  else if (bumpSensors.rightIsPressed())
  {
    // Right bump sensor is pressed.
    ledRed(true);
    motors.setSpeeds(0, 0);
    buzzer.play("e32");
    display.gotoXY(7, 0);
    display.print('R');

    motors.setSpeeds(-maxSpeed, maxSpeed);
    delay(turnTime);

    motors.setSpeeds(0, 0);
    buzzer.play("f32");
    ledRed(false);
    display.gotoXY(7, 0);
    display.print(' ');
  }
}
