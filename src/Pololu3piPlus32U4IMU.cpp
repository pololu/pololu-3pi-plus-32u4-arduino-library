// Copyright (C) Pololu Corporation.  See www.pololu.com for details.

#include <Pololu3piPlus32U4IMU_declaration.h>

#define LSM6DS33_WHO_ID 0x69
#define LIS3MDL_WHO_ID  0x3D

namespace Pololu3piPlus32U4
{

bool IMU::init()
{
  if (testReg(LSM6DS33_ADDR, LSM6DS33_REG_WHO_AM_I) == LSM6DS33_WHO_ID &&
      testReg( LIS3MDL_ADDR,  LIS3MDL_REG_WHO_AM_I) ==  LIS3MDL_WHO_ID)
  {
    type = IMUType::LSM6DS33_LIS3MDL;
    return true;
  }
  else
  {
    return false;
  }
}

void IMU::enableDefault()
{
  switch (type)
  {
  case IMUType::LSM6DS33_LIS3MDL:

    // Accelerometer

    // 0x30 = 0b00110000
    // ODR = 0011 (52 Hz (high performance)); FS_XL = 00 (+/- 2 g full scale)
    writeReg(LSM6DS33_ADDR, LSM6DS33_REG_CTRL1_XL, 0x30);
    if (lastError) { return; }

    // Gyro

    // 0x50 = 0b01010000
    // ODR = 0101 (208 Hz (high performance)); FS_G = 00 (+/- 245 dps full scale)
    writeReg(LSM6DS33_ADDR, LSM6DS33_REG_CTRL2_G, 0x50);
    if (lastError) { return; }

    // Accelerometer + Gyro

    // 0x04 = 0b00000100
    // IF_INC = 1 (automatically increment register address)
    writeReg(LSM6DS33_ADDR, LSM6DS33_REG_CTRL3_C, 0x04);
    if (lastError) { return; }

    // Magnetometer

    // 0x70 = 0b01110000
    // OM = 11 (ultra-high-performance mode for X and Y); DO = 100 (10 Hz ODR)
    writeReg(LIS3MDL_ADDR, LIS3MDL_REG_CTRL_REG1, 0x70);
    if (lastError) { return; }

    // 0x00 = 0b00000000
    // FS = 00 (+/- 4 gauss full scale)
    writeReg(LIS3MDL_ADDR, LIS3MDL_REG_CTRL_REG2, 0x00);
    if (lastError) { return; }

    // 0x00 = 0b00000000
    // MD = 00 (continuous-conversion mode)
    writeReg(LIS3MDL_ADDR, LIS3MDL_REG_CTRL_REG3, 0x00);
    if (lastError) { return; }

    // 0x0C = 0b00001100
    // OMZ = 11 (ultra-high-performance mode for Z)
    writeReg(LIS3MDL_ADDR, LIS3MDL_REG_CTRL_REG4, 0x0C);
    return;
  default:
    return;
  }
}

void IMU::configureForTurnSensing()
{
  switch (type)
  {
  case IMUType::LSM6DS33_LIS3MDL:

    // Gyro

    // 0x7C = 0b01111100
    // ODR = 0111 (833 Hz (high performance)); FS_G = 11 (+/- 2000 dps full scale)
    writeReg(LSM6DS33_ADDR, LSM6DS33_REG_CTRL2_G, 0x7C);
    return;
  default:
    return;
  }
}

void IMU::configureForFaceUphill()
{
  switch (type)
  {
  case IMUType::LSM6DS33_LIS3MDL:

    // Accelerometer

    // 0x10 = 0b00010000
    // ODR = 0001 (13 Hz (high performance)); FS_XL = 00 (+/- 2 g full scale)
    writeReg(LSM6DS33_ADDR, LSM6DS33_REG_CTRL1_XL, 0x10);
    return;
  default:
    return;
  }
}

void IMU::configureForCompassHeading()
{
  switch (type)
  {
  case IMUType::LSM6DS33_LIS3MDL:

    // Magnetometer

    // 0x7C = 0b01111100
    // OM = 11 (ultra-high-performance mode for X and Y); DO = 111 (80 Hz ODR)
    writeReg(LIS3MDL_ADDR, LIS3MDL_REG_CTRL_REG1, 0x7C);
    return;
  default:
    return;
  }
}

// Reads the 3 accelerometer channels and stores them in vector a
void IMU::readAcc(void)
{
  switch (type)
  {
  case IMUType::LSM6DS33_LIS3MDL:
    // assumes register address auto-increment is enabled (IF_INC in CTRL3_C)
    readAxes16Bit(LSM6DS33_ADDR, LSM6DS33_REG_OUTX_L_XL, a);
    return;
  default:
    return;
  }
}

// Reads the 3 gyro channels and stores them in vector g
void IMU::readGyro()
{
  switch (type)
  {
  case IMUType::LSM6DS33_LIS3MDL:
    // assumes register address auto-increment is enabled (IF_INC in CTRL3_C)
    readAxes16Bit(LSM6DS33_ADDR, LSM6DS33_REG_OUTX_L_G, g);
    return;
  default:
    return;
  }
}

// Reads the 3 magnetometer channels and stores them in vector m
void IMU::readMag()
{
  switch (type)
  {
  case IMUType::LSM6DS33_LIS3MDL:
    // set MSB of register address for auto-increment
    readAxes16Bit(LIS3MDL_ADDR, LIS3MDL_REG_OUT_X_L | (1 << 7), m);
    return;
  default:
    return;
  }
}

// Reads all 9 accelerometer, gyro, and magnetometer channels and stores them
// in the respective vectors
void IMU::read()
{
  readAcc();
  if (lastError) { return; }
  readGyro();
  if (lastError) { return; }
  readMag();
}

bool IMU::accDataReady()
{
  switch (type)
  {
  case IMUType::LSM6DS33_LIS3MDL:
    return readReg(LSM6DS33_ADDR, LSM6DS33_REG_STATUS_REG) & 0x01;
  default:
    return false;
  }
}

bool IMU::gyroDataReady()
{
  switch (type)
  {
  case IMUType::LSM6DS33_LIS3MDL:
    return readReg(LSM6DS33_ADDR, LSM6DS33_REG_STATUS_REG) & 0x02;
  default:
    return false;
  }
}

bool IMU::magDataReady()
{
  switch (type)
  {
  case IMUType::LSM6DS33_LIS3MDL:
    return readReg(LIS3MDL_ADDR, LIS3MDL_REG_STATUS_REG) & 0x08;
  default:
    return false;
  }
}

}
