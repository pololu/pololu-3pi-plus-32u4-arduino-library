// Copyright (C) Pololu Corporation.  See www.pololu.com for details.

/// \file Pololu3piPlus32U4BumpSensors.h

#pragma once

#include <Arduino.h>

namespace Pololu3piPlus32U4
{

/// Bump sensor sides.
enum BumpSide {
  /// Left bump sensor
  BumpLeft  = 0,

  /// Right bump sensor
  BumpRight = 1
};

/// \brief Gets readings from the two bump sensors on the front of the 3pi+
/// 32U4.
class BumpSensors
{
  public:
    /// Default timeout for RC sensors (in microseconds).
    static const uint16_t defaultTimeout = 4000;

    static const uint8_t emitterPin = 11;
    static const uint8_t bumpLeftPin = 4;
    static const uint8_t bumpRightPin = 5;

    /// \brief Calibrates the bump sensors.
    ///
    /// \param count The number of times to read the sensors during calibration.
    /// The default is 50.
    ///
    /// This method reads the bump sensors a number of times for calibration.
    /// You should call it while the bump sensors are not pressed and before
    /// using them in your program.
    ///
    /// Calling this function obtains a set of baseline readings that should
    /// should represent the raw sensor readings while the bump sensors are not
    /// pressed. The library uses these baseline values and an additional margin
    /// (#marginPercentage, defined as a percentage of the baselines) to derive
    /// thresholds that the sensors must exceed to register as pressed:
    ///
    /// \f[
    /// {\text{threshold} = \text{baseline} \times \frac{100 + \text{marginPercentage}}{100}}
    /// \f]
    void calibrate(uint8_t count = 50);

    /// \brief Reads both bump sensors.
    ///
    /// \return A bit field indicating whether each bump sensor is pressed. The
    /// bits representing each sensor are defined by the ::BumpSide enum.
    ///
    /// For example, a return value of 2 (0b10 in binary) indicates:
    /// * The right bump sensor is pressed, since bit 1 (BumpRight) is set.
    /// * The left bump sensor is not pressed, since bit 0 (BumpLeft) is
    ///   cleared.
    ///
    /// Instead of checking the return value of this method, you can instead
    /// call read() and then use the functions leftChanged(), rightChanged(),
    /// leftIsPressed(), and rightIsPressed() to get information about the bump
    /// sensors.
    uint8_t read();

    /// \brief Indicates whether the left bump sensor's state has changed.
    ///
    /// \return True if the left bump sensor's state has changed between the
    /// second-to-last and the last call to read(); false otherwise.
    bool leftChanged()    { return (pressed[BumpLeft] ^ last[BumpLeft]) != 0; }


    /// \brief Indicates whether the right bump sensor's state has changed.
    ///
    /// \return True if the right bump sensor's state has changed between the
    /// second-to-last and the last call to read(); false otherwise.
    bool rightChanged()   { return (pressed[BumpRight] ^ last[BumpRight]) != 0; }

    /// \brief Indicates whether the left bump sensor is pressed.
    ///
    /// \return True if the left bump sensor was pressed during the most recent
    /// call to read(); false otherwise.
    bool leftIsPressed()  { return pressed[BumpLeft]; }

    /// \brief Indicates whether the right bump sensor is pressed.
    ///
    /// \return True if the right bump sensor was pressed during the most recent
    /// call to read(); false otherwise.
    bool rightIsPressed() { return pressed[BumpRight]; }

    /// \brief The amount, as a percentage, that will be added to the measured
    /// baseline to get the threshold.
    ///
    /// You must calibrate after changing this.
    ///
    /// \sa calibrate()
    uint16_t marginPercentage = 50;

    /// Baseline readings obtained from calibration.
    uint16_t baseline[2];

    /// Thresholds for bump sensor press detection.
    uint16_t threshold[2];

    /// Raw reflectance sensor readings.
    uint16_t sensorValues[2];

    /// Timeout for bump sensor readings (in microseconds).
    uint16_t timeout = defaultTimeout;

  private:
    void readRaw();
    uint8_t pressed[2];
    uint8_t last[2];
};

}
