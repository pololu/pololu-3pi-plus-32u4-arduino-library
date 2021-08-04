/*
This is a demo program for the older 3pi+ 32U4 with a black and green
LCD display.

It uses the buttons, display, and buzzer to provide a user
interface.  It presents a menu to the user that lets the user
select from several different demos.

To use this demo program, you will need to have the LCD connected to
the 3pi+.  If you cannot see any text on the LCD, try rotating the
contrast potentiometer.

If you have an newer 3pi+ with a blue and black OLED display, use the
other demo, DemoForOLEDVersion.
*/

#include <Wire.h>
#include <Pololu3piPlus32U4.h>

/* The IMU is not fully enabled by default since it depends on the
Wire library, which uses about 1400 bytes of additional code space
and defines an interrupt service routine (ISR) that might be
incompatible with some applications (such as our TWISlave example).

Include Pololu3piPlus32U4IMU.h in one of your cpp/ino files to
enable IMU functionality.
*/
#include <Pololu3piPlus32U4IMU.h>

#include <PololuMenu.h>

using namespace Pololu3piPlus32U4;

LCD display;

Buzzer buzzer;
ButtonA buttonA;
ButtonB buttonB;
ButtonC buttonC;
LineSensors lineSensors;
BumpSensors bumpSensors;
IMU imu;
Motors motors;
Encoders encoders;

PololuMenu<typeof(display)> mainMenu;

bool launchSelfTest = false;

// A couple of simple tunes, stored in program space.
const char beepBrownout[] PROGMEM = "<c8";
const char beepWelcome[] PROGMEM = ">g32>>c32";
const char beepThankYou[] PROGMEM = ">>c32>g32";
const char beepFail[] PROGMEM = "<g-8r8<g-8r8<g-8";
const char beepPass[] PROGMEM = ">l32c>e>g>>c8";
const char beepReadySetGo[] PROGMEM = ">c16r2>c16r2>>c4";

// Custom characters for the LCD:

// This character is a back arrow.
const char backArrow[] PROGMEM = {
  0b00000,
  0b00010,
  0b00001,
  0b00101,
  0b01001,
  0b11110,
  0b01000,
  0b00100,
};

// This character is two chevrons pointing up.
const char forwardArrows[] PROGMEM = {
  0b00000,
  0b00100,
  0b01010,
  0b10001,
  0b00100,
  0b01010,
  0b10001,
  0b00000,
};

// This character is two chevrons pointing down.
const char reverseArrows[] PROGMEM = {
  0b00000,
  0b10001,
  0b01010,
  0b00100,
  0b10001,
  0b01010,
  0b00100,
  0b00000,
};

// This character is two solid arrows pointing up.
const char forwardArrowsSolid[] PROGMEM = {
  0b00000,
  0b00100,
  0b01110,
  0b11111,
  0b00100,
  0b01110,
  0b11111,
  0b00000,
};

// This character is two solid arrows pointing down.
const char reverseArrowsSolid[] PROGMEM = {
  0b00000,
  0b11111,
  0b01110,
  0b00100,
  0b11111,
  0b01110,
  0b00100,
  0b00000,
};

void loadCustomCharacters()
{
  // The LCD supports up to 8 custom characters.  Each character
  // has a number between 0 and 7.  We assign #7 to be the back
  // arrow; other characters are loaded by individual demos as
  // needed.

  display.loadCustomCharacter(backArrow, 7);
}

// Assigns #0-6 to be bar graph characters.
void loadCustomCharactersBarGraph()
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

// Assigns #0-4 to be arrow symbols.
void loadCustomCharactersMotorDirs()
{
  display.loadCustomCharacter(forwardArrows, 0);
  display.loadCustomCharacter(reverseArrows, 1);
  display.loadCustomCharacter(forwardArrowsSolid, 2);
  display.loadCustomCharacter(reverseArrowsSolid, 3);
}

// Clears the LCD and puts [back_arrow]B on the second line
// to indicate to the user that the B button goes back.
void displayBackArrow()
{
  display.clear();
  display.gotoXY(0,1);
  display.print(F("\7B"));
  display.gotoXY(0,0);
}

// Blinks all three LEDs in sequence.
void ledDemo()
{
  displayBackArrow();

  uint8_t state = 3;
  static uint16_t lastUpdateTime = millis() - 2000;
  while (mainMenu.buttonMonitor() != 'B')
  {
    if ((uint16_t)(millis() - lastUpdateTime) >= 500)
    {
      lastUpdateTime = millis();
      state = state + 1;
      if (state >= 4) { state = 0; }

      switch (state)
      {
      case 0:
        buzzer.play("c32");
        display.gotoXY(0, 0);
        display.print(F("Red   "));
        ledRed(1);
        ledGreen(0);
        ledYellow(0);
        break;

      case 1:
        buzzer.play("e32");
        display.gotoXY(0, 0);
        display.print(F("Green"));
        ledRed(0);
        ledGreen(1);
        ledYellow(0);
        break;

      case 2:
        buzzer.play("g32");
        display.gotoXY(0, 0);
        display.print(F("Yellow"));
        ledRed(0);
        ledGreen(0);
        ledYellow(1);
        break;
      }
    }
  }

  ledRed(0);
  ledYellow(0);
  ledGreen(0);
}

void printBar(uint8_t height)
{
  if (height > 8) { height = 8; }
  static const char barChars[] = {' ', 0, 1, 2, 3, 4, 5, 6, (char)255};
  display.print(barChars[height]);
}

void selfTestWaitShowingVBat()
{
  ledYellow(0);
  ledGreen(0);
  ledRed(0);
  while(!mainMenu.buttonMonitor())
  {
    display.gotoXY(0,0);
    display.print(' ');
    display.print(readBatteryMillivolts());
    display.print(F(" mV"));
    delay(100);
  }
}

void selfTestFail()
{
  display.gotoXY(0, 1);
  display.print(F("FAIL"));
  buzzer.playFromProgramSpace(beepFail);
  while(!mainMenu.buttonMonitor());
}

void selfTest()
{
  display.clear();
  display.print(F("3\xf7+ 32U4"));
  display.gotoXY(0, 1);
  display.print(F("SelfTest"));
  delay(1000);

  bumpSensors.calibrate();

  display.clear();
  display.print(F("Press"));
  display.gotoXY(0, 1);
  display.print(F("bumpers"));
  do
  {
     bumpSensors.read();
  }
  while(!bumpSensors.leftIsPressed() || !bumpSensors.rightIsPressed());

  buzzer.play("!c32");
  display.gotoXY(0, 1);
  display.print(F("        "));

  // test some voltages and IMU presence
  display.gotoXY(0, 0);
  display.print(F("USB "));
  if(usbPowerPresent())
  {
    display.print(F("on"));
    selfTestFail();
    return;
  }
  else
  {
    display.print(F("off"));
  }
  ledYellow(1);
  delay(500);

  display.gotoXY(0, 0);
  display.print(F("VBAT     "));
  int v = readBatteryMillivolts();
  display.gotoXY(4, 0);
  display.print(v);
  if(v < 4000 || v > 7000)
  {
    selfTestFail();
    return;
  }
  ledGreen(1);
  delay(500);

  display.gotoXY(0, 0);
  display.print(F("IMU     "));
  display.gotoXY(4, 0);
  if(!imu.init())
  {
    selfTestFail();
    return;
  }
  display.print(F("OK"));
  ledRed(1);
  delay(500);

  // test motor speed, direction, and encoders
  display.gotoXY(0, 0);
  display.print(F("Motors  "));
  ledYellow(1);
  ledGreen(1);
  ledRed(1);
  imu.configureForTurnSensing();

  encoders.getCountsAndResetLeft();
  encoders.getCountsAndResetRight();
  motors.setSpeeds(90, -90);
  delay(250);

  // check rotation speed
  imu.read();
  int16_t gyroReading = imu.g.z;

  motors.setSpeeds(0, 0);
  delay(100);
  int left = encoders.getCountsAndResetLeft();
  int right = encoders.getCountsAndResetRight();
  display.clear();
  if(gyroReading > -7000 && gyroReading < -5000 &&
    left > 212 && left < 288 && right > -288 && right < -212)
  {
    display.print(F("Standrd?"));
  }
  else if(gyroReading > -1800 && gyroReading < -1200 &&
    left > 140 && left < 200 && right > -200 && right < -140)
  {
    display.print(F("Turtle?"));
  }
  else if(gyroReading > 9500 && gyroReading < 17000 &&
    left > 130 && left < 370 && right > -370 && right < -130)
  {
    display.print(F("Hyper?"));
  }
  else
  {
    display.clear();
    display.print(left);
    display.gotoXY(4, 0);
    display.print(right);

    display.gotoXY(4, 1);
    display.print(gyroReading/100);
    selfTestFail();
    return;
  }

  display.gotoXY(0,1);
  display.print(F("A=?  B=Y"));
  while(true)
  {
    char button = mainMenu.buttonMonitor();
    if(button == 'A')
    {
      display.clear();
      display.print(left);
      display.gotoXY(4, 0);
      display.print(right);

      display.gotoXY(0, 1);
      display.print(gyroReading);
    }
    if(button == 'B')
    {
      break;
    }
    if(button == 'C')
    {
      selfTestFail();
      return;
    }
  }

  // Passed all tests!
  display.gotoXY(0, 1);
  display.print(F("PASS    "));
  delay(250); // finish the button beep
  buzzer.playFromProgramSpace(beepPass);
  selfTestWaitShowingVBat();
}

// Display line sensor readings. Holding button C turns off
// the IR emitters.
void lineSensorDemo()
{
  loadCustomCharactersBarGraph();
  displayBackArrow();
  display.gotoXY(6, 1);
  display.print('C');

  uint16_t lineSensorValues[5];

  while (mainMenu.buttonMonitor() != 'B')
  {
    bool emittersOff = buttonC.isPressed();

    lineSensors.read(lineSensorValues, emittersOff ? LineSensorsReadMode::Off :  LineSensorsReadMode::On);

    display.gotoXY(1, 0);
    for (uint8_t i = 0; i < 5; i++)
    {
      uint8_t barHeight = map(lineSensorValues[i], 0, 2000, 0, 8);
      printBar(barHeight);
    }

    // Display an indicator of whether emitters are on or
    // off.
    display.gotoXY(7, 1);
    if (emittersOff)
    {
      display.print('\xa5');  // centered dot
    }
    else
    {
      display.print('*');
    }
  }
}

void bumpSensorDemo()
{
  bumpSensors.calibrate();
  displayBackArrow();

  while (mainMenu.buttonMonitor() != 'B')
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
}

// Starts I2C and initializes the inertial sensors.
void initInertialSensors()
{
  Wire.begin();
  imu.init();
  imu.enableDefault();
}

// Given 3 readings for axes x, y, and z, prints the sign
// and axis of the largest reading unless it is below the
// given threshold.
void printLargestAxis(int16_t x, int16_t y, int16_t z, uint16_t threshold)
{
  int16_t largest = x;
  char axis = 'X';

  if (abs(y) > abs(largest))
  {
    largest = y;
    axis = 'Y';
  }
  if (abs(z) > abs(largest))
  {
    largest = z;
    axis = 'Z';
  }

  if (abs(largest) < threshold)
  {
    display.print("  ");
  }
  else
  {
    bool positive = (largest > 0);
    display.print(positive ? '+' : '-');
    display.print(axis);
  }
}

// Print the direction of the largest rotation rate measured
// by the gyro and the up direction based on the
// accelerometer's measurement of gravitational acceleration
// (assuming gravity is the dominant force acting on the 3pi+).
void inertialDemo()
{
  displayBackArrow();

  display.gotoXY(3, 0);
  display.print(F("Rot"));
  display.gotoXY(4, 1);
  display.print(F("Up"));

  while (mainMenu.buttonMonitor() != 'B')
  {
    imu.read();

    display.gotoXY(6, 0);
    printLargestAxis(imu.g.x, imu.g.y, imu.g.z, 2000);
    display.gotoXY(6, 1);
    printLargestAxis(imu.a.x, imu.a.y, imu.a.z, 200);
  }
}

// Demonstrate the built-in magnetometer with a simple compass
// application.  This demo constantly calibrates by keeping
// track of min and max values in all axes, and it displays an
// estimated heading ("S", "NE", etc.) by comparing the current
// values to these values.
void compassDemo()
{
  IMU::vector<int16_t> magMax;
  IMU::vector<int16_t> magMin;
  IMU::vector<int16_t> m;
  bool showReadings = false;
  bool firstReading = true;

  while (true)
  {
    display.noAutoDisplay();
    display.clear();
    displayBackArrow();
    display.gotoXY(7,1);
    display.print('C');
    imu.readMag();

    if (firstReading)
    {
      magMax = imu.m;
      magMin = imu.m;
      firstReading = false;
    }

    // Update min/max calibration values.
    magMax.x = max(magMax.x, imu.m.x);
    magMax.y = max(magMax.y, imu.m.y);
    magMax.z = max(magMax.z, imu.m.z);
    magMin.x = min(magMin.x, imu.m.x);
    magMin.y = min(magMin.y, imu.m.y);
    magMin.z = min(magMin.z, imu.m.z);

    // Subtract the average of min and max to get a vector
    // centered at an approximation of true zero.
    m.x = imu.m.x - (magMax.x+magMin.x)/2;
    m.y = imu.m.y - (magMax.y+magMin.y)/2;
    m.z = imu.m.z - (magMax.z+magMin.z)/2;

    if (showReadings)
    {
      display.gotoXY(0, 0);
      display.print(m.x/100);
      display.gotoXY(4, 0);
      display.print(m.y/100);

      display.gotoXY(3, 1);
      display.print(m.z/100);
    }
    else if (magMax.x - 1000 < magMin.x || magMax.y - 1000 < magMin.y)
    {
      display.gotoXY(0, 0);
      display.print(F("Turn me!"));
    }
    else {
      // Estimate the direction by checking which vector is the closest
      // match.
      int n = m.x;
      int s = -n;
      int e = m.y;
      int w = -e;
      int ne = (n+e)*10/14;
      int nw = (n+w)*10/14;
      int se = (s+e)*10/14;
      int sw = (s+w)*10/14;

      const __FlashStringHelper* dir = F("??");
      int max_value=0;
      if (n > max_value) { max_value = n; dir = F("N"); }
      if (s > max_value) { max_value = s; dir = F("S"); }
      if (e > max_value) { max_value = e; dir = F("E"); }
      if (w > max_value) { max_value = w; dir = F("W"); }
      if (ne > max_value) { max_value = ne; dir = F("NE"); }
      if (se > max_value) { max_value = se; dir = F("SE"); }
      if (nw > max_value) { max_value = nw; dir = F("NW"); }
      if (sw > max_value) { max_value = sw; dir = F("SW"); }

      display.gotoXY(3,0);
      display.print(dir);
    }
    display.display();

    switch (mainMenu.buttonMonitor())
    {
    case 'B':
      return;
    case 'C':
      showReadings = !showReadings;
      break;
    }

    delay(50);
  }

}

// Provides an interface to test the motors. Holding button A or C
// causes the left or right motor to accelerate; releasing the
// button causes the motor to decelerate. Tapping the button while
// the motor is not running reverses the direction it runs.
//
// If the showEncoders argument is true, encoder counts are
// displayed on the first line of the LCD; otherwise, an
// instructional message is shown.
void motorDemoHelper(bool showEncoders)
{
  loadCustomCharactersMotorDirs();
  display.clear();
  display.gotoXY(1, 1);
  display.print(F("A \7B C"));

  int16_t leftSpeed = 0, rightSpeed = 0;
  int8_t leftDir = 1, rightDir = 1;
  uint16_t lastUpdateTime = millis() - 100;
  uint8_t btnCountA = 0, btnCountC = 0, instructCount = 0;

  int16_t encCountsLeft = 0, encCountsRight = 0;
  char buf[4];

  while (mainMenu.buttonMonitor() != 'B')
  {
    encCountsLeft += encoders.getCountsAndResetLeft();
    if (encCountsLeft < 0) { encCountsLeft += 1000; }
    if (encCountsLeft > 999) { encCountsLeft -= 1000; }

    encCountsRight += encoders.getCountsAndResetRight();
    if (encCountsRight < 0) { encCountsRight += 1000; }
    if (encCountsRight > 999) { encCountsRight -= 1000; }

    // Update the LCD and motors every 50 ms.
    if ((uint16_t)(millis() - lastUpdateTime) > 50)
    {
      lastUpdateTime = millis();

      display.gotoXY(0, 0);
      if (showEncoders)
      {
        sprintf(buf, "%03d", encCountsLeft);
        display.print(buf);
        display.gotoXY(5, 0);
        sprintf(buf, "%03d", encCountsRight);
        display.print(buf);
      }
      else
      {
        // Cycle the instructions every 2 seconds.
        if (instructCount == 0)
        {
          display.print("Hold=run");
        }
        else if (instructCount == 40)
        {
          display.print("Tap=flip");
        }
        if (++instructCount == 80) { instructCount = 0; }
      }

      if (buttonA.isPressed())
      {
        if (btnCountA < 4)
        {
          btnCountA++;
        }
        else
        {
          // Button has been held for more than 200 ms, so
          // start running the motor.
          leftSpeed += 15;
        }
      }
      else
      {
        if (leftSpeed == 0 && btnCountA > 0 && btnCountA < 4)
        {
          // Motor isn't running and button was pressed for
          // 200 ms or less, so flip the motor direction.
          leftDir = -leftDir;
        }
        btnCountA = 0;
        leftSpeed -= 30;
      }

      if (buttonC.isPressed())
      {
        if (btnCountC < 4)
        {
          btnCountC++;
        }
        else
        {
          // Button has been held for more than 200 ms, so
          // start running the motor.
          rightSpeed += 15;
        }
      }
      else
      {
        if (rightSpeed == 0 && btnCountC > 0 && btnCountC < 4)
        {
          // Motor isn't running and button was pressed for
          // 200 ms or less, so flip the motor direction.
          rightDir = -rightDir;
        }
        btnCountC = 0;
        rightSpeed -= 30;
      }

      leftSpeed = constrain(leftSpeed, 0, 400);
      rightSpeed = constrain(rightSpeed, 0, 400);

      motors.setSpeeds(leftSpeed * leftDir, rightSpeed * rightDir);

      // Display arrows pointing the appropriate direction
      // (solid if the motor is running, chevrons if not).
      display.gotoXY(0, 1);
      if (leftSpeed == 0)
      {
        display.print((leftDir > 0) ? '\0' : '\1');
      }
      else
      {
        display.print((leftDir > 0) ? '\2' : '\3');
      }
      display.gotoXY(7, 1);
      if (rightSpeed == 0)
      {
        display.print((rightDir > 0) ? '\0' : '\1');
      }
      else
      {
        display.print((rightDir > 0) ? '\2' : '\3');
      }
    }
  }
  motors.setSpeeds(0, 0);
}


// Motor demo with instructions.
void motorDemo()
{
  motorDemoHelper(false);
}

// Motor demo with encoder counts.
void encoderDemo()
{
  motorDemoHelper(true);
}

// Spin in place
void spinDemo()
{
  display.clear();
  displayBackArrow();
  display.print(F("Spinning"));
  display.gotoXY(5,1);
  display.print(F("..."));
  delay(200);
  buzzer.playFromProgramSpace(beepReadySetGo);
  while(buzzer.isPlaying())
  {
    if(mainMenu.buttonMonitor() == 'B')
      return;
  }
  spinDemoInternal();
  motors.setSpeeds(0, 0);
}

void spinDemoInternal()
{
  // Spin right
  for(int i = 0; i < 40; i++)
  {
    motors.setSpeeds(i * 10, -i * 10);
    delay(50);
    if(mainMenu.buttonMonitor() == 'B')
      return;
  }
  for(int i = 40; i >= 0; i--)
  {
    motors.setSpeeds(i * 10, -i * 10);
    delay(50);
    if(mainMenu.buttonMonitor() == 'B')
      return;
  }

  // Spin left
  for(int i = 0; i < 40; i++)
  {
    motors.setSpeeds(-i * 10, i * 10);
    delay(50);
    if(mainMenu.buttonMonitor() == 'B')
      return;
  }
  for(int i = 40; i >= 0; i--)
  {
    motors.setSpeeds(-i * 10, i * 10);
    delay(50);
    if(mainMenu.buttonMonitor() == 'B')
      return;
  }
}

const char fugue[] PROGMEM =
  "! T120O5L16agafaea dac+adaea fa<aa<bac#a dac#adaea f"
  "O6dcd<b-d<ad<g d<f+d<gd<ad<b- d<dd<ed<f+d<g d<f+d<gd<ad"
  "L8MS<b-d<b-d MLe-<ge-<g MSc<ac<a MLd<fd<f O5MSb-gb-g"
  "ML>c#e>c#e MS afaf ML gc#gc# MS fdfd ML e<b-e<b-"
  "O6L16ragafaea dac#adaea fa<aa<bac#a dac#adaea faeadaca"
  "<b-acadg<b-g egdgcg<b-g <ag<b-gcf<af dfcf<b-f<af"
  "<gf<af<b-e<ge c#e<b-e<ae<ge <fe<ge<ad<fd"
  "O5e>ee>ef>df>d b->c#b->c#a>df>d e>ee>ef>df>d"
  "e>d>c#>db>d>c#b >c#agaegfe fO6dc#dfdc#<b c#4";

const char fugueTitle[] PROGMEM =
  "       Fugue in D Minor - by J.S. Bach       ";

// Play a song on the buzzer and display its title.
void musicDemo()
{
  displayBackArrow();

  size_t fugueTitlePos = 0;
  uint16_t lastShiftTime = millis() - 2000;

  while (mainMenu.buttonMonitor() != 'B')
  {
    // Shift the song title to the left every 250 ms.
    if ((uint16_t)(millis() - lastShiftTime) > 250)
    {
      lastShiftTime = millis();

      display.gotoXY(0, 0);
      for (uint8_t i = 0; i < 8; i++)
      {
        char c = pgm_read_byte(fugueTitle + fugueTitlePos + i);
        display.print(c);
      }
      fugueTitlePos++;

      if (fugueTitlePos + 8 >= strlen(fugueTitle))
      {
        fugueTitlePos = 0;
      }
    }

    if (!buzzer.isPlaying())
    {
      buzzer.playFromProgramSpace(fugue);
    }
  }
}

// Display the the battery (VIN) voltage and indicate whether USB
// power is detected.
void powerDemo()
{
  displayBackArrow();

  uint16_t lastDisplayTime = millis() - 2000;
  char buf[6];

  while (mainMenu.buttonMonitor() != 'B')
  {
    if ((uint16_t)(millis() - lastDisplayTime) > 250)
    {
      bool usbPower = usbPowerPresent();

      uint16_t batteryLevel = readBatteryMillivolts();

      lastDisplayTime = millis();
      display.gotoXY(0, 0);
      sprintf(buf, "%5d", batteryLevel);
      display.print(buf);
      display.print(F(" mV"));
      display.gotoXY(3, 1);
      display.print(F("USB="));
      display.print(usbPower ? 'Y' : 'N');
    }
  }
}

// This demo shows all characters that the LCD can display.  Press C to
// advance to the the next page of 8 characters, or A to go back a page.
//
// The demo starts on character 0x20 (space).
//
// Note that characters 0-8 are the custom characters.  They are
// initially set to some strange random-looking shapes, but if you run
// other demos that set custom characters then return here, you will
// see what they loaded.
void displayDemo() {
  displayBackArrow();
  display.gotoXY(7,1);
  display.print('C');

  // The first four pages are boring/weird, so start at 0x20, which
  // will show these characters, starting with a space:
  //  !"#$%&'
  uint8_t startCharacter = 4 * 8;

  while (true)
  {
    display.gotoXY(0,0);
    for(uint8_t i = 0; i < 8; i++)
    {
      display.print((char)(startCharacter + i));
    }
    display.gotoXY(3,1);

    char buf[4];
    sprintf(buf, "x%02x", startCharacter);
    display.print(buf);

    char b = mainMenu.buttonMonitor();
    if ('B' == b) break;
    if ('A' == b) startCharacter -= 8;
    if ('C' == b) startCharacter += 8;
  }
}

const char aboutText[] PROGMEM =
  "       Pololu 3pi+ 32U4 Robot - more info at pololu.com/3pi+       ";

void aboutDemo() {
  size_t textPos = 0;
  uint16_t lastShiftTime = millis() - 2000;
  displayBackArrow();

  while (mainMenu.buttonMonitor() != 'B')
  {
    // Shift the text to the left every 250 ms.
    if ((uint16_t)(millis() - lastShiftTime) > 250)
    {
      lastShiftTime = millis();

      display.gotoXY(0, 0);
      for (uint8_t i = 0; i < 8; i++)
      {
        char c = pgm_read_byte(aboutText + textPos + i);
        display.print(c);
      }
      textPos++;

      if (textPos + 8 >= strlen(aboutText))
      {
        textPos = 0;
      }
    }
  }
}

void setup()
{
  static const PololuMenuItem mainMenuItems[] = {
    { F("Power"), powerDemo },
    { F("LineSens"), lineSensorDemo },
    { F("BumpSens"), bumpSensorDemo },
    { F("Inertial"), inertialDemo },
    { F("Compass"), compassDemo },
    { F("Motors"), motorDemo },
    { F("Encoders"), encoderDemo },
    { F("Spin"), spinDemo },
    { F("LEDs"), ledDemo },
    { F("LCD"), displayDemo },
    { F("Music"), musicDemo },
    { F("About"), aboutDemo },
  };
  mainMenu.setItems(mainMenuItems, sizeof(mainMenuItems)/sizeof(mainMenuItems[0]));
  mainMenu.setDisplay(display);
  mainMenu.setBuzzer(buzzer);
  mainMenu.setButtons(buttonA, buttonB, buttonC);
  mainMenu.setSecondLine(F("\x7f" "A \xa5" "B C\x7e"));

  initInertialSensors();

  loadCustomCharacters();

  // The brownout threshold on the ATmega32U4 is set to 4.3
  // V.  If VCC drops below this, a brownout reset will
  // occur, preventing the AVR from operating out of spec.
  //
  // Note: Brownout resets usually do not happen on the 3pi+
  // 32U4 because the voltage regulator goes straight from 5
  // V to 0 V when VIN drops too low.
  //
  // The bootloader is designed so that you can detect
  // brownout resets from your sketch using the following
  // code:
  bool brownout = MCUSR >> BORF & 1;
  MCUSR = 0;

  if (brownout)
  {
    // The board was reset by a brownout reset
    // (VCC dropped below 4.3 V).
    // Play a special sound and display a note to the user.

    buzzer.playFromProgramSpace(beepBrownout);
    display.clear();
    display.print(F("Brownout"));
    display.gotoXY(0, 1);
    display.print(F(" reset! "));
    delay(1000);
  }
  else
  {
    buzzer.playFromProgramSpace(beepWelcome);
  }

  // allow skipping quickly to the menu by holding button C
  if (buttonC.isPressed())
  {
    selfTest();
    return;
  }

  display.clear();
  display.print(F("3\xf7+ 32U4"));
  display.gotoXY(2, 1);
  display.print(F("Demo"));

  uint16_t blinkStart = millis();
  while((uint16_t)(millis() - blinkStart) < 1000)
  {
    // keep setting the LEDs on for 1s
    // the Green/Red LEDs might turn off during USB communication
    ledYellow(1);
    ledGreen(1);
    ledRed(1);
  }

  display.clear();
  display.print(F("Use B to"));
  display.gotoXY(0, 1);
  display.print(F("select."));

  while((uint16_t)(millis() - blinkStart) < 2000)
  {
    // keep the LEDs off for 1s
    ledYellow(0);
    ledGreen(0);
    ledRed(0);
  }

  display.clear();
  display.print(F("Press B"));
  display.gotoXY(0, 1);
  display.print(F("-try it!"));

  // Keep blinking the yellow LED while waiting for the
  // user to press button B.
  blinkStart = millis();
  while (mainMenu.buttonMonitor() != 'B')
  {
    uint16_t blinkPhase = millis() - blinkStart;
    ledGreen(blinkPhase < 1000);
    if (blinkPhase >= 2000) { blinkStart += 2000; }
  }
  ledGreen(0);

  buzzer.playFromProgramSpace(beepThankYou);
  display.clear();
  display.print(F(" Thank"));
  display.gotoXY(0, 1);
  display.print(F("  you!"));
  delay(1000);
  mainMenuWelcome();
}

// Clear LEDs and show a message about the main menu.
void mainMenuWelcome()
{
  ledYellow(false);
  ledGreen(false);
  ledRed(false);
  display.clear();
  display.print(F("  Main"));
  display.gotoXY(0, 1);
  display.print(F("  Menu"));
  delay(1000);
}

void loop()
{
  if(mainMenu.select())
  {
    // a menu item ran; show "Main Menu" again and repeat
    mainMenuWelcome();
  }
}
