/** \file Tpp32U4LineSensors.h **/

#pragma once

#include <QTRSensors.h>
#include <Arduino.h>

/** \brief Gets readings from the five reflectance sensors on the bottom of
 * the 3pi+ 32U4.
 *
 * The readLineBlack() and readLineWhite() methods will always return values
 * that increase from left to right, with 0 corresponding to the leftmost sensor
 * and 4000 corresponding to the rightmost sensor.
 *
 * See the [Usage Notes in the QTRSensors
 * documentation](https://pololu.github.io/qtr-sensors-arduino/md_usage.html)
 * for an overview of how the methods inherited from the QTRSensors library can
 * be used and some example code.
 */
class Tpp32U4LineSensors : public QTRSensors
{
  public:

    Tpp32U4LineSensors()
    {
      setTypeRC();
      setNonDimmable();
      setSensorPins((const uint8_t[]){ 12, A0, A2, A3, A4 }, 5);
      setEmitterPin(11);
    }

  private:

    void initEmitterPin(uint8_t pin) override
    {
      pinMode(pin, INPUT);
    }

    void activateEmitterPin(uint8_t pin) override
    {
      // Call digitalWrite() first to pull the pin high before making it an output
      // driving high. (Reversing these would cause the pin to briefly drive low
      // before driving high.)
      digitalWrite(pin, HIGH);
      pinMode(pin, OUTPUT);
    }

    void deactivateEmitterPin(uint8_t pin) override
    {
      pinMode(pin, INPUT);
    }

    // By always returning false from both of these functions, we never allow a
    // wait delay to be skipped based on the current state of the emitter pin.
    // (When the pin is floating, its reading could be undefined.)

    bool emitterPinIsActive(uint8_t pin) override
    {
      return false;
    }

    bool emitterPinIsInactive(uint8_t pin) override
    {
      return false;
    }

};

