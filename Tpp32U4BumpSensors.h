/** \file Tpp32U4BumpSensors.h **/

#pragma once

#include <QTRSensors.h>
#include <Arduino.h>

enum BumpSide {
  BumpLeft  = 0,
  BumpRight = 1
};

class Tpp32U4BumpSensors
{
  public:

    void calibrate(uint8_t count = 50);

    uint8_t read();

    bool leftChanged()    { return pressed[BumpLeft] ^ last[BumpLeft] != 0; }
    bool leftIsPressed()  { return pressed[BumpLeft]; }
    bool rightChanged()   { return pressed[BumpRight] ^ last[BumpRight] != 0; }
    bool rightIsPressed() { return pressed[BumpRight]; }

    /*! \brief The amount, as a percentage, that will be added to the measured
    baseline to get the threshold. You must calibrate after changing this. */
    uint16_t marginPercentage = 50;

    uint16_t baseline[2];
    uint16_t threshold[2];
    uint16_t sensorValues[2];

  private:

    uint8_t pressed[2];
    uint8_t last[2];

    class Tpp32U4BumpQTR : public QTRSensors
    {
      public:

        Tpp32U4BumpQTR()
        {
          setTypeRC();
          setNonDimmable();
          setSensorPins((const uint8_t[]){ 4, 5 }, 2);
          setEmitterPin(11);
          setTimeout(2500);
        }

      private:

        void initEmitterPin(uint8_t pin) override
        {
          pinMode(pin, INPUT);
        }

        void activateEmitterPin(uint8_t pin) override
        {
          pinMode(pin, OUTPUT);
          digitalWrite(pin, LOW);
          delay(1);
        }

        void deactivateEmitterPin(uint8_t pin) override
        {
          pinMode(pin, INPUT);
        }

        // By always returning false from both of these functions, we never
        // allow a wait delay to be skipped based on the current state of the
        // emitter pin. (When the pin is floating, its reading could be
        // undefined.)

        bool emitterPinIsActive(uint8_t pin) override
        {
          return false;
        }

        bool emitterPinIsInactive(uint8_t pin) override
        {
          return false;
        }

    };

    Tpp32U4BumpQTR qtr;
};
