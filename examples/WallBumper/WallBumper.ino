#include <Tpp32U4.h>

Tpp32U4BumpSensors bumpSensors;
Tpp32U4Buzzer buzzer;
Tpp32U4LCD lcd;
Tpp32U4Motors motors;

void setup()
{
  bumpSensors.calibrate();
  lcd.clear();
  delay(1000);
}

void loop()
{
  motors.setSpeeds(100, 100);
  bumpSensors.read();

  if (bumpSensors.leftIsPressed())
  {
    // Left bump sensor is pressed.
    ledYellow(true);
    motors.setSpeeds(0, 0);
    buzzer.play("a32");
    lcd.gotoXY(0, 0);
    lcd.print('L');

    motors.setSpeeds(100, -100);
    delay(250);

    motors.setSpeeds(0, 0);
    buzzer.play("b32");
    ledYellow(false);
    lcd.gotoXY(0, 0);
    lcd.print(' ');
  }
  else if (bumpSensors.rightIsPressed())
  {
    // Right bump sensor is pressed.
    ledRed(true);
    motors.setSpeeds(0, 0);
    buzzer.play("e32");
    lcd.gotoXY(7, 0);
    lcd.print('R');

    motors.setSpeeds(-100, 100);
    delay(250);

    motors.setSpeeds(0, 0);
    buzzer.play("f32");
    ledRed(false);
    lcd.gotoXY(7, 0);
    lcd.print(' ');
  }
}
