#include <Pololu3piPlus32U4.h>

using namespace Pololu3piPlus32U4;
BumpSensors bumpSensors;
Buzzer buzzer;

// Change next line to this if you are using the older 3pi+
// with a black and green LCD display:
// LCD display;
OLED display;

void setup()
{
  bumpSensors.calibrate();
  display.clear();
  display.gotoXY(0, 1);
  display.print("Bump me!");
}

void loop()
{
  bumpSensors.read();

  if (bumpSensors.leftChanged())
  {
    ledYellow(bumpSensors.leftIsPressed());
    if (bumpSensors.leftIsPressed())
    {
      // Left bump sensor was just pressed.
      buzzer.play("a32");
      display.gotoXY(0, 0);
      display.print('L');
    }
    else
    {
      // Left bump sensor was just released.
      buzzer.play("b32");
      display.gotoXY(0, 0);
      display.print(' ');
    }
  }

  if (bumpSensors.rightChanged())
  {
    ledRed(bumpSensors.rightIsPressed());
    if (bumpSensors.rightIsPressed())
    {
      // Right bump sensor was just pressed.
      buzzer.play("e32");
      display.gotoXY(7, 0);
      display.print('R');
    }
    else
    {
      // Right bump sensor was just released.
      buzzer.play("f32");
      display.gotoXY(7, 0);
      display.print(' ');
    }
  }
}
