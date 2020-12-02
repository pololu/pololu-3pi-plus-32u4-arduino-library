// Copyright (C) Pololu Corporation.  See www.pololu.com for details.

#include <Pololu3piPlus32U4LineSensors.h>

namespace Pololu3piPlus32U4
{

void LineSensors::setTimeout(uint16_t timeout)
{
  if (timeout > 32767) { timeout = 32767; }
  _timeout = timeout;
  _maxValue = timeout;
}

void LineSensors::resetCalibration()
{
  for (uint8_t i = 0; i < _sensorCount; i++)
  {
    if (calibrationOn.maximum)   { calibrationOn.maximum[i] = 0; }
    if (calibrationOff.maximum)  { calibrationOff.maximum[i] = 0; }
    if (calibrationOn.minimum)   { calibrationOn.minimum[i] = _maxValue; }
    if (calibrationOff.minimum)  { calibrationOff.minimum[i] = _maxValue; }
  }
}

void LineSensors::calibrate(LineSensorsReadMode mode)
{
  // manual emitter control is not supported
  if (mode == LineSensorsReadMode::Manual) { return; }

  if (mode == LineSensorsReadMode::On)
  {
    calibrateOnOrOff(calibrationOn, LineSensorsReadMode::On);
  }

  if (mode == LineSensorsReadMode::Off)
  {
    calibrateOnOrOff(calibrationOff, LineSensorsReadMode::Off);
  }
}

void LineSensors::calibrateOnOrOff(CalibrationData & calibration, LineSensorsReadMode mode)
{
  uint16_t sensorValues[_sensorCount];
  uint16_t maxSensorValues[_sensorCount];
  uint16_t minSensorValues[_sensorCount];

  // (Re)allocate and initialize the arrays if necessary.
  if (!calibration.initialized)
  {
    uint16_t * oldMaximum = calibration.maximum;
    calibration.maximum = (uint16_t *)realloc(calibration.maximum,
                                              sizeof(uint16_t) * _sensorCount);
    if (calibration.maximum == nullptr)
    {
      // Memory allocation failed; don't continue.
      free(oldMaximum); // deallocate any memory used by old array
      return;
    }

    uint16_t * oldMinimum = calibration.minimum;
    calibration.minimum = (uint16_t *)realloc(calibration.minimum,
                                              sizeof(uint16_t) * _sensorCount);
    if (calibration.minimum == nullptr)
    {
      // Memory allocation failed; don't continue.
      free(oldMinimum); // deallocate any memory used by old array
      return;
    }

    // Initialize the max and min calibrated values to values that
    // will cause the first reading to update them.
    for (uint8_t i = 0; i < _sensorCount; i++)
    {
      calibration.maximum[i] = 0;
      calibration.minimum[i] = _maxValue;
    }

    calibration.initialized = true;
  }

  for (uint8_t j = 0; j < 10; j++)
  {
    read(sensorValues, mode);

    for (uint8_t i = 0; i < _sensorCount; i++)
    {
      // set the max we found THIS time
      if ((j == 0) || (sensorValues[i] > maxSensorValues[i]))
      {
        maxSensorValues[i] = sensorValues[i];
      }

      // set the min we found THIS time
      if ((j == 0) || (sensorValues[i] < minSensorValues[i]))
      {
        minSensorValues[i] = sensorValues[i];
      }
    }
  }

  // record the min and max calibration values
  for (uint8_t i = 0; i < _sensorCount; i++)
  {
    // Update maximum only if the min of 10 readings was still higher than it
    // (we got 10 readings in a row higher than the existing maximum).
    if (minSensorValues[i] > calibration.maximum[i])
    {
      calibration.maximum[i] = minSensorValues[i];
    }

    // Update minimum only if the max of 10 readings was still lower than it
    // (we got 10 readings in a row lower than the existing minimum).
    if (maxSensorValues[i] < calibration.minimum[i])
    {
      calibration.minimum[i] = maxSensorValues[i];
    }
  }
}

void LineSensors::read(uint16_t * sensorValues, LineSensorsReadMode mode)
{
  switch (mode)
  {
    case LineSensorsReadMode::Off:
      emittersOff();
      readPrivate(sensorValues);
      return;

    case LineSensorsReadMode::Manual:
      readPrivate(sensorValues);
      return;

    case LineSensorsReadMode::On:
      emittersOn();
      readPrivate(sensorValues);
      emittersOff();
      return;

    default: // invalid - do nothing
      return;
  }
}

void LineSensors::readCalibrated(uint16_t * sensorValues, LineSensorsReadMode mode)
{
  // manual emitter control is not supported
  if (mode == LineSensorsReadMode::Manual) { return; }

  // if not calibrated, do nothing

  if (mode == LineSensorsReadMode::On)
  {
    if (!calibrationOn.initialized)
    {
      return;
    }
  }

  if (mode == LineSensorsReadMode::Off)
  {
    if (!calibrationOff.initialized)
    {
      return;
    }
  }

  // read the needed values
  read(sensorValues, mode);

  for (uint8_t i = 0; i < _sensorCount; i++)
  {
    uint16_t calmin, calmax;

    // find the correct calibration
    if (mode == LineSensorsReadMode::On)
    {
      calmax = calibrationOn.maximum[i];
      calmin = calibrationOn.minimum[i];
    }
    else if (mode == LineSensorsReadMode::Off)
    {
      calmax = calibrationOff.maximum[i];
      calmin = calibrationOff.minimum[i];
    }

    uint16_t denominator = calmax - calmin;
    int16_t value = 0;

    if (denominator != 0)
    {
      value = (((int32_t)sensorValues[i]) - calmin) * 1000 / denominator;
    }

    if (value < 0) { value = 0; }
    else if (value > 1000) { value = 1000; }

    sensorValues[i] = value;
  }
}

uint16_t LineSensors::readLinePrivate(uint16_t * sensorValues, LineSensorsReadMode mode,
                         bool invertReadings)
{
  bool onLine = false;
  uint32_t avg = 0; // this is for the weighted total
  uint16_t sum = 0; // this is for the denominator, which is <= 64000

  // manual emitter control is not supported
  if (mode == LineSensorsReadMode::Manual) { return 0; }

  readCalibrated(sensorValues, mode);

  for (uint8_t i = 0; i < _sensorCount; i++)
  {
    uint16_t value = sensorValues[i];
    if (invertReadings) { value = 1000 - value; }

    // keep track of whether we see the line at all
    if (value > 200) { onLine = true; }

    // only average in values that are above a noise threshold
    if (value > 50)
    {
      avg += (uint32_t)value * (i * 1000);
      sum += value;
    }
  }

  if (!onLine)
  {
    // If it last read to the left of center, return 0.
    if (_lastPosition < (_sensorCount - 1) * 1000 / 2)
    {
      return 0;
    }
    // If it last read to the right of center, return the max.
    else
    {
      return (_sensorCount - 1) * 1000;
    }
  }

  _lastPosition = avg / sum;
  return _lastPosition;
}

// the destructor frees up allocated memory
LineSensors::~LineSensors()
{
  if (calibrationOn.maximum)  { free(calibrationOn.maximum); }
  if (calibrationOff.maximum) { free(calibrationOff.maximum); }
  if (calibrationOn.minimum)  { free(calibrationOn.minimum); }
  if (calibrationOff.minimum) { free(calibrationOff.minimum); }
}

void LineSensors::readPrivate(uint16_t * sensorValues)
{
  FastGPIO::Pin<line0Pin>::setOutputHigh();
  FastGPIO::Pin<line1Pin>::setOutputHigh();
  FastGPIO::Pin<line2Pin>::setOutputHigh();
  FastGPIO::Pin<line3Pin>::setOutputHigh();
  FastGPIO::Pin<line4Pin>::setOutputHigh();
  _delay_us(10);

  sensorValues[0] = _timeout;
  sensorValues[1] = _timeout;
  sensorValues[2] = _timeout;
  sensorValues[3] = _timeout;
  sensorValues[4] = _timeout;

  noInterrupts();
  uint16_t startTime = micros();
  FastGPIO::Pin<line0Pin>::setInput();
  FastGPIO::Pin<line1Pin>::setInput();
  FastGPIO::Pin<line2Pin>::setInput();
  FastGPIO::Pin<line3Pin>::setInput();
  FastGPIO::Pin<line4Pin>::setInput();
  interrupts();

  uint16_t time = 0;
  while (true)
  {
    noInterrupts();
    time = micros() - startTime;
    if (time >= _timeout)
    {
      interrupts();
      break;
    }
    if (!FastGPIO::Pin<line0Pin>::isInputHigh() && time < sensorValues[0]) { sensorValues[0] = time; }
    if (!FastGPIO::Pin<line1Pin>::isInputHigh() && time < sensorValues[1]) { sensorValues[1] = time; }
    if (!FastGPIO::Pin<line2Pin>::isInputHigh() && time < sensorValues[2]) { sensorValues[2] = time; }
    if (!FastGPIO::Pin<line3Pin>::isInputHigh() && time < sensorValues[3]) { sensorValues[3] = time; }
    if (!FastGPIO::Pin<line4Pin>::isInputHigh() && time < sensorValues[4]) { sensorValues[4] = time; }
    interrupts();
    __builtin_avr_delay_cycles(4);  // allow interrupts to run
  }
}

}
