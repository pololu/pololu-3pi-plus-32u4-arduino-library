// Copyright (C) Pololu Corporation.  See www.pololu.com for details.

/// \file Pololu3piPlus32U4Motors.h

#pragma once

#include <stdint.h>

namespace Pololu3piPlus32U4
{

/// \brief Controls motor speed and direction on the 3pi+ 32U4.
///
/// This library uses Timer 1, so it will conflict with any other libraries
/// using that timer.
class Motors
{
  public:

    /// \brief Flips the direction of the left motor.
    ///
    /// You can call this function with an argument of \c true if the left motor
    /// of your 3pi+ was not wired in the standard way and you want a
    /// positive speed argument to correspond to forward movement.
    ///
    /// \param flip If true, then positive motor speeds will correspond to the
    /// direction pin being high.  If false, then positive motor speeds will
    /// correspond to the direction pin being low.
    ///
    static void flipLeftMotor(bool flip);

    /// \brief Flips the direction of the right motor.
    ///
    /// You can call this function with an argument of \c true if the right
    /// motor of your 3pi+ was not wired in the standard way and you want a
    /// positive speed argument to correspond to forward movement.
    ///
    /// \param flip If true, then positive motor speeds will correspond to the
    /// direction pin being high.  If false, then positive motor speeds will
    /// correspond to the direction pin being low.
    static void flipRightMotor(bool flip);

    /// \brief Sets the speed for the left motor.
    ///
    /// \param speed A number from -400 to 400 representing the speed and
    /// direction of the left motor.  Values of -400 or less result in full
    /// speed reverse, and values of 400 or more result in full speed forward.
    static void setLeftSpeed(int16_t speed);

    /// \brief Sets the speed for the right motor.
    ///
    /// \param speed A number from -400 to 400 representing the speed and
    /// direction of the right motor. Values of -400 or less result in full
    /// speed reverse, and values of 400 or more result in full speed forward.
    static void setRightSpeed(int16_t speed);

    /// \brief Sets the speeds for both motors.
    ///
    /// \param leftSpeed A number from -400 to 400 representing the speed and
    /// direction of the right motor. Values of -400 or less result in full
    /// speed reverse, and values of 400 or more result in full speed forward.
    /// \param rightSpeed A number from -400 to 400 representing the speed and
    /// direction of the right motor. Values of -400 or less result in full
    /// speed reverse, and values of 400 or more result in full speed forward.
    static void setSpeeds(int16_t leftSpeed, int16_t rightSpeed);

  private:

    static inline void init()
    {
        static bool initialized = false;

        if (!initialized)
        {
            initialized = true;
            init2();
        }
    }

    static void init2();

    static bool flipLeft;
    static bool flipRight;
};

}
