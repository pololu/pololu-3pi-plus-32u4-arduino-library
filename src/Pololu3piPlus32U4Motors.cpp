// Copyright (C) Pololu Corporation.  See www.pololu.com for details.

#include <Pololu3piPlus32U4Motors.h>
#include <FastGPIO.h>
#include <avr/io.h>

namespace Pololu3piPlus32U4
{

#define PWM_L 10
#define PWM_R 9
#define DIR_L 16
#define DIR_R 15

bool Motors::flipLeft = false;
bool Motors::flipRight = false;

// initialize timer1 to generate the proper PWM outputs to the motor drivers
void Motors::init2()
{
    FastGPIO::Pin<PWM_L>::setOutputLow();
    FastGPIO::Pin<PWM_R>::setOutputLow();
    FastGPIO::Pin<DIR_L>::setOutputLow();
    FastGPIO::Pin<DIR_R>::setOutputLow();

    // Timer 1 configuration
    // prescaler: clockI/O / 1
    // outputs enabled
    // phase-correct PWM
    // top of 400
    //
    // PWM frequency calculation
    // 16MHz / 1 (prescaler) / 2 (phase-correct) / 400 (top) = 20kHz
    TCCR1A = 0b10100000;
    TCCR1B = 0b00010001;
    ICR1 = 400;
    OCR1A = 0;
    OCR1B = 0;
}

void Motors::flipLeftMotor(bool flip)
{
    flipLeft = flip;
}

void Motors::flipRightMotor(bool flip)
{
    flipRight = flip;
}

void Motors::setLeftSpeed(int16_t speed)
{
    init();

    bool reverse = 0;

    if (speed < 0)
    {
        speed = -speed; // Make speed a positive quantity.
        reverse = 1;    // Preserve the direction.
    }
    if (speed > 400)
    {
        speed = 400;
    }

    OCR1B = speed;

    FastGPIO::Pin<DIR_L>::setOutput(reverse ^ flipLeft);
}

void Motors::setRightSpeed(int16_t speed)
{
    init();

    bool reverse = 0;

    if (speed < 0)
    {
        speed = -speed;  // Make speed a positive quantity.
        reverse = 1;     // Preserve the direction.
    }
    if (speed > 400)
    {
        speed = 400;
    }

    OCR1A = speed;

    FastGPIO::Pin<DIR_R>::setOutput(reverse ^ flipRight);
}

void Motors::setSpeeds(int16_t leftSpeed, int16_t rightSpeed)
{
    setLeftSpeed(leftSpeed);
    setRightSpeed(rightSpeed);
}

}
