// Copyright (C) Pololu Corporation.  See www.pololu.com for details.

/// \file Pololu3piPlus32U4Encoders.h

#pragma once

#include <stdint.h>

namespace Pololu3piPlus32U4
{

/// \brief Reads counts from the encoders on the 3pi+ 32U4.
///
/// This class allows you to read counts from the encoders on the 3pi+ 32U4,
/// which lets you tell how much each motor has turned and in what direction.
///
/// The encoders are monitored in the background using interrupts, so your code
/// can perform other tasks without missing encoder counts.
///
/// To read the left encoder, this class uses an interrupt service routine (ISR)
/// for PCINT0_vect, so there will be a compile-time conflict with any other
/// code that defines a pin-change ISR.
///
/// To read the right encoder, this class calls
/// [attachInterrupt()](http://arduino.cc/en/Reference/attachInterrupt), so
/// there will be a compile-time conflict with any other code that defines an
/// ISR for an external interrupt directly instead of using attachInterrupt().
class Encoders
{

public:

    /// \brief Flips the direction of the encoders.
    ///
    /// This is useful if you have to flip the direction of the motors
    /// due to a non-standard gearbox.
    ///
    /// \param flip If true, the direction of counting will be
    /// reversed relative to the standard 3pi+ 32U4.
    static void flipEncoders(bool flip);

    /// \brief Initializes the encoders (called automatically).
    ///
    /// This function initializes the encoders if they have not been initialized
    /// already and starts listening for counts.  This function is called
    /// automatically whenever you call any other function in this class, so you
    /// should not normally need to call it in your code.
    static void init()
    {
        static bool initialized = 0;
        if (!initialized)
        {
            initialized = true;
            init2();
        }
    }

    /// \brief Returns the number of counts that have been detected from the
    /// left-side encoder.
    ///
    /// The count starts at 0.  Positive counts correspond to forward movement
    /// of the left side of the 3pi+, while negative counts correspond to
    /// backwards movement.
    ///
    /// The count is returned as a signed 16-bit integer.  When the count goes
    /// over 32767, it will overflow down to -32768.  When the count goes below
    /// -32768, it will overflow up to 32767.
    static int16_t getCountsLeft();

    /// \brief Returns the number of counts that have been detected from the
    /// right-side encoder.
    ///
    /// \sa getCountsLeft()
    static int16_t getCountsRight();

    /// \brief Returns the number of counts that have been detected from the
    /// left-side encoder and clears the counts.
    ///
    /// This function is just like getCountsLeft() except it also clears the
    /// counts before returning.  If you call this frequently enough, you will
    /// not have to worry about the count overflowing.
    static int16_t getCountsAndResetLeft();

    /// \brief Returns the number of counts that have been detected from the
    /// left-side encoder and clears the counts.
    ///
    /// \sa getCountsAndResetLeft()
    static int16_t getCountsAndResetRight();

    /// \brief Returns true if an error was detected on the left-side encoder.
    ///
    /// This function resets the error flag automatically, so it will only
    /// return true if an error was detected since the last time
    /// checkErrorLeft() was called.
    ///
    /// If an error happens, it means that both of the encoder outputs changed
    /// at the same time from the perspective of the ISR, so the ISR was unable
    /// to tell what direction the motor was moving, and the encoder count could
    /// be inaccurate.  The most likely cause for an error is that the interrupt
    /// service routine for the encoders could not be started soon enough.  If
    /// you get encoder errors, make sure you are not disabling interrupts for
    /// extended periods of time in your code.
    static bool checkErrorLeft();

    /// \brief Returns true if an error was detected on the right-side encoder.
    ///
    /// \sa checkErrorLeft()
    static bool checkErrorRight();

private:

    static void init2();
    static bool flip;
};

}
