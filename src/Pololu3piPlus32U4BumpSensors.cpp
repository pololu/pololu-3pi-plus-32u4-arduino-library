// Copyright (C) Pololu Corporation.  See www.pololu.com for details.

#include <Pololu3piPlus32U4BumpSensors.h>
#include <FastGPIO.h>

namespace Pololu3piPlus32U4
{

void BumpSensors::readRaw()
{
  FastGPIO::Pin<emitterPin>::setOutputLow();  // Turn on the emitters.

  FastGPIO::Pin<bumpLeftPin>::setOutputHigh();
  FastGPIO::Pin<bumpRightPin>::setOutputHigh();
  _delay_us(10);

  sensorValues[0] = timeout;
  sensorValues[1] = timeout;

  noInterrupts();
  uint16_t startTime = micros();
  FastGPIO::Pin<bumpLeftPin>::setInput();
  FastGPIO::Pin<bumpRightPin>::setInput();
  interrupts();

  while (true)
  {
    noInterrupts();
    uint16_t time = micros() - startTime;
    if (time >= timeout)
    {
      interrupts();
      break;
    }
    if (!FastGPIO::Pin<bumpLeftPin>::isInputHigh() && time < sensorValues[0]) { sensorValues[0] = time; }
    if (!FastGPIO::Pin<bumpRightPin>::isInputHigh() && time < sensorValues[1]) { sensorValues[1] = time; }
    interrupts();
    __builtin_avr_delay_cycles(4);  // allow interrupts to run
  }

  FastGPIO::Pin<emitterPin>::setInput();  // turn off the emitters
}

void BumpSensors::calibrate(uint8_t count)
{
  uint32_t sum[2] = {0, 0};

  for (uint8_t i = 0; i < count; i++)
  {
    readRaw();
    sum[BumpLeft]  += sensorValues[BumpLeft];
    sum[BumpRight] += sensorValues[BumpRight];
  }

  for (uint8_t s = BumpLeft; s <= BumpRight; s++)
  {
    baseline[s] = (sum[s] + count / 2) / count;

    // Calculate threshold to compare readings to by adding margin to baseline,
    // but make sure it is no larger than the QTR sensor timeout (i.e. if the
    // reading timed out, consider the bump sensor pressed).
    threshold[s] = baseline[s] + baseline[s] * (uint32_t)marginPercentage / 100;
    if (threshold[s] > timeout) { threshold[s] = timeout; }
  }
}

uint8_t BumpSensors::read()
{
  readRaw();

  uint8_t bitField = 0;
  for (uint8_t s = BumpLeft; s <= BumpRight; s++)
  {
    last[s] = pressed[s];
    pressed[s] = (sensorValues[s] >= threshold[s]);
    bitField |= pressed[s] << s;
  }
  return bitField;
}

}
