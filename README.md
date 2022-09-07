# Pololu3piPlus32U4 library

[www.pololu.com](https://www.pololu.com/)

## Summary

<img align="right" src="https://a.pololu-files.com/picture/0J11323.240w.jpg?bf2f67dbe8c5a1035409af8b78b78f97">

This is a C++ library for the Arduino IDE that helps access the on-board hardware of the [Pololu 3pi+ 32U4 Robot](https://www.pololu.com/category/280/3pi-plus-32u4-oled-robot) (both the [newer OLED version](https://www.pololu.com/category/280/3pi-plus-32u4-oled-robot) and the [original LCD version](https://www.pololu.com/category/285/original-3pi-plus-32u4-robot)).

The 3pi+ 32U4 robot is a complete, high-performance mobile platform based on the ATmega32U4 microcontroller.  It has integrated motor drivers, encoders, a display screen (graphical OLED or LCD), a buzzer, buttons, line sensors, front bump sensors, an LSM6DS33 accelerometer and gyro, and an LIS3MDL compass. See the [3pi+ 32U4 user's guide](https://www.pololu.com/docs/0J83) for more information.

## Installing the library

Use the Library Manager in version 1.8.10 or later of the Arduino software (IDE) to install this library:

1. In the Arduino IDE, open the "Tools" menu and select "Manage Libraries...".
2. Search for "3pi+".
3. Click the Pololu3piPlus32U4 entry in the list.
4. Click "Install".
5. If you see a prompt asking to install missing dependencies, click "Install all".

## Usage

To access most of features of this library, you just need a single include statement.  For convenience, we recommend using the Pololu3piPlus32U4 namespace and declaring all of the objects you want to use as global variables, as shown below:

```cpp
#include <Pololu3piPlus32U4.h>

using namespace Pololu3piPlus32U4;

OLED display;
Buzzer buzzer;
ButtonA buttonA;
ButtonB buttonB;
ButtonC buttonC;
LineSensors lineSensors;
BumpSensors bumpSensors;
Motors motors;
Encoders encoders;
```

The IMU is not fully enabled by default since it depends on the Wire library, which uses about 1400 bytes of additional code space and defines an interrupt service routine (ISR) that might be incompatible with some applications.

Include Pololu3piPlus32U4IMU.h in one of your cpp/ino files to enable IMU functionality:

```cpp
#include <Pololu3piPlus32U4IMU.h>

IMU imu;
```

## Examples

Several example sketches are available that show how to use the library.  You can access them from the Arduino IDE by opening the "File" menu, selecting "Examples", and then selecting "Pololu3piPlus32U4".  If you cannot find these examples, the library was probably installed incorrectly and you should retry the installation instructions above.

## Classes and functions

The contents of the library are contained in the Pololu3piPlus32U4 namespace. The main classes and functions provided by the library are listed below:

* Pololu3piPlus32U4::ButtonA
* Pololu3piPlus32U4::ButtonB
* Pololu3piPlus32U4::ButtonC
* Pololu3piPlus32U4::Buzzer
* Pololu3piPlus32U4::Encoders
* Pololu3piPlus32U4::OLED
* Pololu3piPlus32U4::LCD
* Pololu3piPlus32U4::Motors
* Pololu3piPlus32U4::LineSensors
* Pololu3piPlus32U4::BumpSensors
* Pololu3piPlus32U4::IMU
* Pololu3piPlus32U4::ledRed()
* Pololu3piPlus32U4::ledGreen()
* Pololu3piPlus32U4::ledYellow()
* Pololu3piPlus32U4::usbPowerPresent()
* Pololu3piPlus32U4::readBatteryMillivolts()

## Dependencies

This library also references several other Arduino libraries which are used to help implement the classes and functions above.

* [FastGPIO](https://github.com/pololu/fastgpio-arduino)
* [PololuBuzzer](https://github.com/pololu/pololu-buzzer-arduino)
* [PololuHD44780](https://github.com/pololu/pololu-hd44780-arduino)
* [PololuMenu](https://github.com/pololu/pololu-menu-arduino)
* [PololuOLED](https://github.com/pololu/pololu-oled-arduino)
* [Pushbutton](https://github.com/pololu/pushbutton-arduino)
* [USBPause](https://github.com/pololu/usb-pause-arduino)

## Version history

* 1.1.3 (2022-09-06): Fixed a bug in the Encoders demo that could prevent encoder errors from being shown properly on the display.
* 1.1.2 (2021-10-15): Fixed a bug that prevented the OLED display from working if the MOS pin was previously changed to be an input.
* 1.1.1 (2021-08-11): Fixed a typo in the RotationResist demo.
* 1.1.0 (2021-08-10): Added support for the 3pi+ 32U4 OLED.
* 1.0.1 (2020-12-03): Moved code into `src/` folder; continuous integration; fixed some warnings; added missing library dependence on PololuMenu.
* 1.0.0 (2020-12-01): Customized example sketches for different 3pi+ editions. Reimplemented menus using the PololuMenu class. Added flipEncoders() method to the Encoders class to work better with the Hyper edition robot.
* 0.2.0 (2020-11-25): Initial public release; motor demos only support Standard Edition.
* 0.1.0 (2020-11-25): Initial release for production.
