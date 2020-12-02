// Copyright (C) Pololu Corporation.  See www.pololu.com for details.

/// \file Pololu3piPlus32U4Buttons.h

#pragma once

#include <Pushbutton.h>
#include <FastGPIO.h>
#include <USBPause.h>
#include <util/delay.h>

namespace Pololu3piPlus32U4
{

/// \brief Interfaces with button A on the 3pi+ 32U4.
class ButtonA : public Pushbutton
{
public:
    /// The pin number for the pin connected to button A on the 3pi+ 32U4.
    static const uint8_t buttonAPin = 14;

    ButtonA() : Pushbutton(buttonAPin)
    {
    }
};

/// \brief Interfaces with button B on the 3pi+ 32U4.
///
/// The pin used for button B is also used for the TX LED.
///
/// This class temporarily disables USB interrupts because the Arduino core code
/// has USB interrupts enabled that sometimes write to the pin this button is
/// on.
///
/// This class temporarily sets the pin to be an input without a pull-up
/// resistor.  The pull-up resistor is not needed because of the resistors on
/// the board.
class ButtonB : public PushbuttonBase
{
public:
    /// The pin number for the pin connected to button B on the 3pi+ 32U4.
    static const uint8_t buttonBPin = IO_D5;

    virtual bool isPressed()
    {
        USBPause usbPause;
        FastGPIO::PinLoan<buttonBPin> loan;
        FastGPIO::Pin<buttonBPin>::setInputPulledUp();
        _delay_us(3);
        return !FastGPIO::Pin<buttonBPin>::isInputHigh();
    }
};

/// \brief Interfaces with button C on the 3pi+ 32U4.
///
/// The pin used for button C is also used for the RX LED.
///
/// This class temporarily disables USB interrupts because the Arduino core code
/// has USB interrupts enabled that sometimes write to the pin this button is
/// on.
///
/// This class temporarily sets the pin to be an input without a pull-up
/// resistor.  The pull-up resistor is not needed because of the resistors on
/// the board.
class ButtonC : public PushbuttonBase
{
public:
    /// The pin number for the pin conencted to button C on the 3pi+ 32U4.
    static const uint8_t buttonCPin = 17;

    virtual bool isPressed()
    {
        USBPause usbPause;
        FastGPIO::PinLoan<buttonCPin> loan;
        FastGPIO::Pin<buttonCPin>::setInputPulledUp();
        _delay_us(3);
        return !FastGPIO::Pin<buttonCPin>::isInputHigh();
    }
};

}
