// Copyright (C) Pololu Corporation.  See www.pololu.com for details.

/// \file Pololu3piPlus32U4.h
///
/// \brief Main header file for the Pololu 3pi+ 32U4 library.
///
/// This file includes all the other headers files provided by the library.

#pragma once

#ifndef __AVR_ATmega32U4__
#error "This library only supports the ATmega32U4.  Try selecting A-Star 32U4 in the Boards menu."
#endif

#include <FastGPIO.h>
#include <Pololu3piPlus32U4BumpSensors.h>
#include <Pololu3piPlus32U4Buttons.h>
#include <Pololu3piPlus32U4Buzzer.h>
#include <Pololu3piPlus32U4Encoders.h>
#include <Pololu3piPlus32U4IMU_declaration.h>
#include <Pololu3piPlus32U4LCD.h>
#include <Pololu3piPlus32U4LineSensors.h>
#include <Pololu3piPlus32U4Motors.h>
#include <Pololu3piPlus32U4OLED.h>

/// Top-level namespace for the Pololu3piPlus32U4 library.
namespace Pololu3piPlus32U4
{

// TODO: servo support

/// \brief Turns the red user LED (RX) on or off.
///
/// \param on A value of 1 turns on the LED; 0 turns it off.
///
/// The red user LED is on pin 17, which is also known as PB0, SS, and RXLED.
/// The Arduino core code uses this LED to indicate when it receives data over
/// USB, so it might be hard to control this LED when USB is connected.
inline void ledRed(bool on)
{
    FastGPIO::Pin<17>::setOutput(!on);
}

/// \brief Turns the yellow user LED on pin 13 on or off.
///
/// \param on A value of 1 turns on the LED; 0 turns it off.
inline void ledYellow(bool on)
{
    FastGPIO::Pin<13>::setOutput(on);
}

/// \brief Turns the green user LED (TX) on or off.
///
/// \param on A value of 1 turns on the LED; 0 turns it off.
///
/// The green user LED is pin PD5, which is also known as TXLED.  The Arduino
/// core code uses this LED to indicate when it receives data over USB, so it
/// might be hard to control this LED when USB is connected.
inline void ledGreen(bool on)
{
    FastGPIO::Pin<IO_D5>::setOutput(!on);
}

/// \brief Returns true if USB power is detected.
///
/// This function returns true if power is detected on the board's USB port and
/// returns false otherwise.  It uses the ATmega32U4's VBUS line, which is
/// directly connected to the power pin of the USB connector.
///
/// \sa A method for detecting whether the board's virtual COM port is open:
/// http://arduino.cc/en/Serial/IfSerial
inline bool usbPowerPresent()
{
    return USBSTA >> VBUS & 1;
}

/// Reads the battery voltage and returns it in millivolts.
inline uint16_t readBatteryMillivolts()
{
    const uint8_t sampleCount = 8;
    uint16_t sum = 0;
    for (uint8_t i = 0; i < sampleCount; i++)
    {
        sum += analogRead(A1);
    }

    // VBAT = 3 * millivolt reading = 3 * raw * 5000/1024
    //      = raw * 1875 / 128
    // The correction term below makes it so that we round to the
    // nearest whole number instead of always rounding down.
    const uint32_t correction = 64 * sampleCount - 1;
    return ((uint32_t)sum * 1875 + correction) / (128 * sampleCount);
}

}
