// Copyright Pololu Corporation.  For more information, see http://www.pololu.com/

/** \file Tpp32U4Buttons.h **/

#pragma once

#include <Pushbutton.h>
#include <FastGPIO.h>
#include <USBPause.h>
#include <util/delay.h>

/*! The pin number for the pin connected to button A on the 3pi+ 32U4. */
#define TPP_32U4_BUTTON_A 14

/*! The pin number for the pin connected to button B on the 3pi+ 32U4. */
#define TPP_32U4_BUTTON_B IO_D5

/*! The pin number for the pin conencted to button C on the 3pi+ 32U4. */
#define TPP_32U4_BUTTON_C 17

/*! \brief Interfaces with button A on the 3pi+ 32U4. */
class Tpp32U4ButtonA : public Pushbutton
{
public:
    Tpp32U4ButtonA() : Pushbutton(TPP_32U4_BUTTON_A)
    {
    }
};

/*! \brief Interfaces with button B on the 3pi+ 32U4.
 *
 * The pin used for button B is also used for the TX LED.
 *
 * This class temporarily disables USB interrupts because the Arduino core code
 * has USB interrupts enabled that sometimes write to the pin this button is on.
 *
 * This class temporarily sets the pin to be an input without a pull-up
 * resistor.  The pull-up resistor is not needed because of the resistors on the
 * board. */
class Tpp32U4ButtonB : public PushbuttonBase
{
public:

    virtual bool isPressed()
    {
        USBPause usbPause;
        FastGPIO::PinLoan<TPP_32U4_BUTTON_B> loan;
        FastGPIO::Pin<TPP_32U4_BUTTON_B>::setInputPulledUp();
        _delay_us(3);
        return !FastGPIO::Pin<TPP_32U4_BUTTON_B>::isInputHigh();
    }
};

/*! \brief Interfaces with button C on the 3pi+ 32U4.
 *
 * The pin used for button C is also used for the RX LED.
 *
 * This class temporarily disables USB interrupts because the Arduino core code
 * has USB interrupts enabled that sometimes write to the pin this button is on.
 *
 * This class temporarily sets the pin to be an input without a pull-up
 * resistor.  The pull-up resistor is not needed because of the resistors on the
 * board. */
class Tpp32U4ButtonC : public PushbuttonBase
{
public:

    virtual bool isPressed()
    {
        USBPause usbPause;
        FastGPIO::PinLoan<TPP_32U4_BUTTON_C> loan;
        FastGPIO::Pin<TPP_32U4_BUTTON_C>::setInputPulledUp();
        _delay_us(3);
        return !FastGPIO::Pin<TPP_32U4_BUTTON_C>::isInputHigh();
    }
};

