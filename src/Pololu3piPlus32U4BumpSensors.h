/** \file Pololu3piPlus32U4BumpSensors.h **/

#pragma once

#include <Arduino.h>

namespace Pololu3piPlus32U4
{

enum BumpSide {
  BumpLeft  = 0,
  BumpRight = 1
};

class BumpSensors
{
  public:
    /// Default timeout for RC sensors (in microseconds).
    static const uint16_t defaultTimeout = 4000;

    static const uint8_t emitterPin = 11;
    static const uint8_t bumpLeftPin = 4;
    static const uint8_t bumpRightPin = 5;

    void calibrate(uint8_t count = 50);

    uint8_t read();

    bool leftChanged()    { return (pressed[BumpLeft] ^ last[BumpLeft]) != 0; }
    bool leftIsPressed()  { return pressed[BumpLeft]; }
    bool rightChanged()   { return (pressed[BumpRight] ^ last[BumpRight]) != 0; }
    bool rightIsPressed() { return pressed[BumpRight]; }

    /*! \brief The amount, as a percentage, that will be added to the measured
    baseline to get the threshold. You must calibrate after changing this. */
    uint16_t marginPercentage = 50;

    uint16_t baseline[2];
    uint16_t threshold[2];
    uint16_t sensorValues[2];
    uint16_t timeout = defaultTimeout;

  private:
    void readRaw();
    uint8_t pressed[2];
    uint8_t last[2];
};

}
