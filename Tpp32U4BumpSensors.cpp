#include <Tpp32U4BumpSensors.h>

void Tpp32U4BumpSensors::calibrate(uint8_t count)
{
  uint32_t sum[2] = {0, 0};

  for (uint8_t i = 0; i < count; i++)
  {
    qtr.read(sensorValues);
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
    if (threshold[s] > qtr.getTimeout()) { threshold[s] = qtr.getTimeout(); }
  }
}

uint8_t Tpp32U4BumpSensors::read()
{
  uint8_t bitField = 0;

  qtr.read(sensorValues);

  for (uint8_t s = BumpLeft; s <= BumpRight; s++)
  {
    last[s] = pressed[s];
    pressed[s] = (sensorValues[s] >= threshold[s]);
    bitField |= pressed[s] << s;
  }

  return bitField;
}
