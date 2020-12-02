// Copyright (C) Pololu Corporation.  See www.pololu.com for details.

/// \file Pololu3piPlus32U4IMU.h
///
/// \brief Include this file in one of your cpp/ino files for IMU
/// functionality.
///
/// The IMU is not fully enabled by default since it depends on the
/// Wire library, which uses about 1400 bytes of additional code space
/// and defines an interrupt service routine (ISR) that might be
/// incompatible with some applications (such as our TWISlave example).

#pragma once
#include <Wire.h>
#include <Pololu3piPlus32U4IMU_declaration.h>

namespace Pololu3piPlus32U4
{

void IMU::writeReg(uint8_t addr, uint8_t reg, uint8_t value)
{
  Wire.beginTransmission(addr);
  Wire.write(reg);
  Wire.write(value);
  lastError = Wire.endTransmission();
}

uint8_t IMU::readReg(uint8_t addr, uint8_t reg)
{
  Wire.beginTransmission(addr);
  Wire.write(reg);
  lastError = Wire.endTransmission();
  if (lastError) { return 0; }

  uint8_t byteCount = Wire.requestFrom(addr, (uint8_t)1);
  if (byteCount != 1)
  {
    lastError = 50;
    return 0;
  }
  return Wire.read();
}

int16_t IMU::testReg(uint8_t addr, uint8_t reg)
{
  Wire.beginTransmission(addr);
  Wire.write(reg);
  if (Wire.endTransmission() != 0)
  {
    return -1;
  }

  uint8_t byteCount = Wire.requestFrom(addr, (uint8_t)1);
  if (byteCount != 1)
  {
    return -1;
  }
  return Wire.read();
}

void IMU::readAxes16Bit(uint8_t addr, uint8_t firstReg, vector<int16_t> & v)
{
  Wire.beginTransmission(addr);
  Wire.write(firstReg);
  lastError = Wire.endTransmission();
  if (lastError) { return; }

  uint8_t byteCount = (Wire.requestFrom(addr, (uint8_t)6));
  if (byteCount != 6)
  {
    lastError = 50;
    return;
  }
  uint8_t xl = Wire.read();
  uint8_t xh = Wire.read();
  uint8_t yl = Wire.read();
  uint8_t yh = Wire.read();
  uint8_t zl = Wire.read();
  uint8_t zh = Wire.read();

  // combine high and low bytes
  v.x = (int16_t)(xh << 8 | xl);
  v.y = (int16_t)(yh << 8 | yl);
  v.z = (int16_t)(zh << 8 | zl);
}

}
