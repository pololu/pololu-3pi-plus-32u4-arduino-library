// This program shows how to read the encoders on the 3pi+ 32U4.
// The encoders can tell you how far, and in which direction each
// motor has turned.
//
// You can press button A on the 3pi+ to drive both motors
// forward at full speed.  You can press button C to drive both
// motors in reverse at full speed.
//
// Encoder counts are printed to the LCD and to the serial
// monitor.
//
// On the LCD, the top line shows the counts from the left
// encoder, and the bottom line shows the counts from the right
// encoder.  Encoder errors should not happen, but if one does
// happen then the buzzer will beep and an exclamation mark will
// appear temporarily on the LCD.
//
// In the serial monitor, the first and second numbers represent
// counts from the left and right encoders, respectively.  The
// third and fourth numbers represent errors from the left and
// right encoders, respectively.

#include <Pololu3piPlus32U4.h>

using namespace Pololu3piPlus32U4;

Encoders encoders;
LCD lcd;
Buzzer buzzer;
Motors motors;
ButtonA buttonA;
ButtonC buttonC;

const char encoderErrorLeft[] PROGMEM = "!<c2";
const char encoderErrorRight[] PROGMEM = "!<e2";

char report[80];

void setup()
{
  // Uncomment the following lines for the Hyper edition; its wheels
  // spin in the opposite direction relative to the encoders.
  // encoders.flipEncoders(true);
  // motors.flipLeftMotor(true);
  // motors.flipRightMotor(true);
}

void loop()
{
  static uint8_t lastDisplayTime;
  static uint8_t displayErrorLeftCountdown = 0;
  static uint8_t displayErrorRightCountdown = 0;

  if ((uint8_t)(millis() - lastDisplayTime) >= 100)
  {
    lastDisplayTime = millis();

    int16_t countsLeft = encoders.getCountsLeft();
    int16_t countsRight = encoders.getCountsRight();

    bool errorLeft = encoders.checkErrorLeft();
    bool errorRight = encoders.checkErrorRight();

    if(encoders.checkErrorLeft())
    {
      // An error occurred on the left encoder channel.
      // Display it on the LCD for the next 10 iterations and
      // also beep.
      displayErrorLeftCountdown = 10;
      buzzer.playFromProgramSpace(encoderErrorLeft);
    }

    if(encoders.checkErrorRight())
    {
      // An error occurred on the left encoder channel.
      // Display it on the LCD for the next 10 iterations and
      // also beep.
      displayErrorRightCountdown = 10;
      buzzer.playFromProgramSpace(encoderErrorRight);
    }

    // Update the LCD with encoder counts and error info.
    lcd.clear();
    lcd.print(countsLeft);
    lcd.gotoXY(0, 1);
    lcd.print(countsRight);

    if (displayErrorLeftCountdown)
    {
      // Show an exclamation point on the first line to
      // indicate an error from the left encoder.
      lcd.gotoXY(7, 0);
      lcd.print('!');
      displayErrorLeftCountdown--;
    }

    if (displayErrorRightCountdown)
    {
      // Show an exclamation point on the second line to
      // indicate an error from the left encoder.
      lcd.gotoXY(7, 1);
      lcd.print('!');
      displayErrorRightCountdown--;
    }

    // Send the information to the serial monitor also.
    snprintf_P(report, sizeof(report),
        PSTR("%6d %6d %1d %1d"),
        countsLeft, countsRight, errorLeft, errorRight);
    Serial.println(report);
  }

  if (buttonA.isPressed())
  {
    motors.setSpeeds(400, 400);
  }
  else if (buttonC.isPressed())
  {
    motors.setSpeeds(-400, -400);
  }
  else
  {
    motors.setSpeeds(0, 0);
  }
}
