#include <Tpp32U4.h>

Tpp32U4BumpSensors bumpSensors;
Tpp32U4Buzzer buzzer;
Tpp32U4LCD lcd;

void setup()
{
  bumpSensors.calibrate();
  lcd.clear();
  lcd.gotoXY(0, 1);
  lcd.print("Bump me!");
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
      lcd.gotoXY(0, 0);
      lcd.print('L');
    }
    else
    {
      // Left bump sensor was just released.
      buzzer.play("b32");
      lcd.gotoXY(0, 0);
      lcd.print(' ');
    }
  }

  if (bumpSensors.rightChanged())
  {
    ledRed(bumpSensors.rightIsPressed());
    if (bumpSensors.rightIsPressed())
    {
      // Right bump sensor was just pressed.
      buzzer.play("e32");
      lcd.gotoXY(7, 0);
      lcd.print('R');
    }
    else
    {
      // Right bump sensor was just released.
      buzzer.play("f32");
      lcd.gotoXY(7, 0);
      lcd.print(' ');
    }
  }
}
