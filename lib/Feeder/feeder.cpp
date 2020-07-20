#include "feeder.h"

#include <Arduino.h>
#include <SPI.h>

// https://github.com/teemuatlut/TMCStepper/blob/master/examples/Simple/Simple.ino
#define EN_PIN           D0 // Enable
#define DIR_PIN          D1 // Direction
#define STEP_PIN         D2 // Step

#define CS_PIN           D8 // Chip select
#define R_SENSE          0.11f

using namespace TMC2130_n;


Feeder::Feeder()
: _stepper(CS_PIN, R_SENSE)
{

}

void Feeder::Feed()
{
    ++_feedCount;

    digitalWrite(EN_PIN, LOW);
    for(unsigned long i = 0; i < _feedSteps; ++i)
    {
        digitalWrite(STEP_PIN, HIGH);
        delayMicroseconds(100);
        digitalWrite(STEP_PIN, LOW);
        delayMicroseconds(100);
        yield();
    }
    ValidateDriverStatus();
    digitalWrite(EN_PIN, HIGH);
}

void Feeder::InitDriver()
{
    pinMode(EN_PIN, OUTPUT);
    pinMode(STEP_PIN, OUTPUT);
    pinMode(DIR_PIN, OUTPUT);
    digitalWrite(EN_PIN, LOW);

    pinMode(MISO, INPUT_PULLUP);

    _stepper.begin();                 // SPI: Init CS pins
    _stepper.toff(5);                 // Enables driver in software
    _stepper.rms_current(600);        // Set motor RMS current
    _stepper.microsteps(16);          // Set microsteps to 1/16th

    _stepper.en_pwm_mode(true);
    _stepper.pwm_autoscale(true);

    digitalWrite(EN_PIN, HIGH);
    ValidateDriverStatus();
}

bool Feeder::ValidateDriverStatus()
{
    DRV_STATUS_t drv_status{0};
    drv_status.sr = _stepper.DRV_STATUS();

    Serial.println("DRIVER STATUS");
    Serial.print("Registers: "); Serial.println(drv_status.sr, HEX);
    Serial.printf("Temp: %x\n", drv_status.ot);
    Serial.printf("S2GA: %x\n", drv_status.s2ga);
    Serial.printf("S2GB: %x\n", drv_status.s2gb);
    Serial.printf("OLA:  %x\n", drv_status.ola);
    Serial.printf("OLB:  %x\n", drv_status.olb);
    Serial.printf("STST: %x\n", drv_status.stst);

    return true;
}