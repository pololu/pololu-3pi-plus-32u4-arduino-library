# Pololu3piPlus32U4 library

Version: 1.0.0<br/>
Release date: 2020-12-01<br/>
[www.pololu.com](https://www.pololu.com/)

## Summary

<img align="right" src="https://a.pololu-files.com/picture/0J11029.240w.jpg?6587c7507cb885bf3d4cdab208e39ddf">

This is a C++ library for the Arduino IDE that helps access the on-board hardware of the [Pololu 3pi+ 32U4 Robot](https://www.pololu.com/category/280/3pi-32u4-robot).

The 3pi+ 32U4 robot is a complete, high-performance mobile platform based on the ATmega32U4 microcontroller.  It has integrated motor drivers, encoders, buzzer, buttons, line sensors, front bump sensors, an LSM6DS33 accelerometer and gyro, and an LIS3MDL compass. See the [3pi+ 32U4 user's guide](https://www.pololu.com/docs/0J83) for more information.

## Installing the library

Follow the installation instructions in the latest release at https://github.com/pololu/pololu-3pi-plus-32u4-arduino-library/releases/.

## Examples

Several example sketches are available that show how to use the library.  You can access them from the Arduino IDE by opening the "File" menu, selecting "Examples", and then selecting "Pololu3piPlus32U4".  If you cannot find these examples, the library was probably installed incorrectly and you should retry the installation instructions above.

## Classes and functions

The contents of the library are contained in the Pololu3piPlus32U4 namespace. The main classes and functions provided by the library are listed below:

* ButtonA
* ButtonB
* ButtonC
* Buzzer
* Encoders
* LCD
* Motors
* LineSensors
* BumpSensors
* IMU
* ledRed()
* ledGreen()
* ledYellow()
* usbPowerPresent()
* readBatteryMillivolts()

## Dependencies

This library also references several other Arduino libraries which are used to help implement the classes and functions above.

* [FastGPIO](https://github.com/pololu/fastgpio-arduino)
* [PololuBuzzer](https://github.com/pololu/pololu-buzzer-arduino)
* [PololuHD44780](https://github.com/pololu/pololu-hd44780-arduino)
* [PololuMenu](https://github.com/pololu/pololu-menu-arduino)
* [Pushbutton](https://github.com/pololu/pushbutton-arduino)
* [USBPause](https://github.com/pololu/usb-pause-arduino)


You can use these libraries in your sketch automatically without any extra installation steps and without needing to add any extra `#include` lines to your sketch. The only `#include` line needed to access all features of this library are:

~~~{.cpp}
#include <Pololu3piPlus32U4.h>
~~~

## Version history

* 1.0.0 (2020-12-01): Customized example sketches for different 3pi+ editions. Reimplemented menus using the PololuMenu class. Added flipEncoders() method to the Encoders class to work better with the Hyper edition robot.
* 0.2.0 (2020-11-25): Initial public release; motor demos only support Standard Edition.
* 0.1.0 (2020-11-25): Initial release for production.
