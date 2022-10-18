/*
   File: measure.cpp
   Project: PixelRadio, an RBDS/RDS FM Transmitter (QN8027 Digital FM IC)
   Version: 1.1.2
   Creation: Dec-16-2021
   Revised:  Oct-18-2022
   Revision History: See PixelRadio.cpp
   Project Leader: T. Black (thomastech)
   Contributors: thomastech

   (c) copyright T. Black 2021-2022, Licensed under GNU GPL 3.0 and later, under this license absolutely no warranty is given.
   This Code was formatted with the uncrustify extension.

 */

// *********************************************************************************************

#include <Arduino.h>
#include <ArduinoLog.h>
#include <driver/adc.h>
#include <esp_adc_cal.h>
#include "config.h"
#include "PixelRadio.h"
#include "globals.h"

const uint16_t DEFAULT_VREF        = 1100;
const uint8_t  E_AVG_SIZE          = 16;                                 // Size of voltage averaging buffers.
const float    VBAT_SCALE          = ((100000.0 + 100000.0) / 100000.0); // Onboard ESP32 Resistor Attenuator on "VBAT" ADC Port.
const float    VDC_SCALE           = ((100000.0 + 33000.0) / 33000.0);   // Resistor Attenuator on "VDC" ADC Port.
const adc1_channel_t VBAT_ADC_PORT = ADC1_CHANNEL_7;                     // GPIO-35, Onboard ESP32 "VBAT" Voltage.
const adc1_channel_t VDC_ADC_PORT  = ADC1_CHANNEL_0;                     // GPIO-36, "VDC" Voltage Monitor (9V RF PA rail).

// Local Scope Vars
static int vbatAvgBuff[E_AVG_SIZE + 1];                                  // VDC data averaging buffer.
static int vdcAvgBuff[E_AVG_SIZE + 1];                                   // VDC data averaging buffer.
static esp_adc_cal_characteristics_t *adc_chars;


// *********************************************************************************************
// initVdcAdc(): Initialize the two ADC used in PixelRadio.
// (1) ESP32's VBAT ADC. We use ADC1_CHANNEL_7, which is GPIO 35.
// (2) ESP32's VDC ADC. We use ADC1_CHANNEL_0, which is GPIO 36.
// This MUST be called in setup() before first use of measureVbatVoltage().
void initVdcAdc(void)
{
    // Configure ADC
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(VBAT_ADC_PORT, ADC_ATTEN_DB_11);
    adc1_config_channel_atten(VDC_ADC_PORT,  ADC_ATTEN_DB_11);

    // Characterize ADC
    adc_chars = (esp_adc_cal_characteristics_t *)calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, DEFAULT_VREF, adc_chars);

    if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
        Log.infoln("ADC eFuse provided Factory Stored Vref Calibration.");      // Best Accuracy.
    }
    else if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP) {
        Log.infoln("ADC eFuse provided Factory Stored Two Point Calibration."); // Good Accuracy.
    }
    else {
        Log.infoln("ADC eFuse not supported, using Default VRef (1100mV).");    // Low Quality Accuracy.
    }
}

// *********************************************************************************************
// measureVbatVoltage(): Measure ESP32's Onboard "VBAT" Voltage using data averaging.
// Be sure to call initVdcAdc() in setup();
void measureVbatVoltage(void)
{
    static int avgIndex      = 0; // Index of the current Amps reading
    static uint32_t totalVdc = 0; // Current totalizer for Amps averaging
    uint32_t voltage;
    uint32_t reading;

    reading = adc1_get_raw(VBAT_ADC_PORT);
    voltage = esp_adc_cal_raw_to_voltage(reading, adc_chars); // Convert to unscaled mV.

    totalVdc              = totalVdc - vbatAvgBuff[avgIndex];
    vbatAvgBuff[avgIndex] = voltage;

    totalVdc  = totalVdc + vbatAvgBuff[avgIndex];
    avgIndex += 1;
    avgIndex  = avgIndex >= E_AVG_SIZE ? 0 : avgIndex;

    voltage   = totalVdc / E_AVG_SIZE;
    vbatVolts = (voltage * VBAT_SCALE) / 1000.0f; // Apply Attenuator Scaling, covert from mV to VDC.
    vbatVolts = constrain(vbatVolts, 0.0f, 99.0f);
}

// *********************************************************************************************
// measureVdcVoltage(): Measure RF Power Amp's Voltage using data averaging.
// Be sure to call initVdcAdc() in setup();
void measureVdcVoltage(void)
{
    static int avgIndex      = 0; // Index of the current Amps reading
    static uint32_t totalVdc = 0; // Current totalizer for Amps averaging
    uint32_t voltage;
    uint32_t reading;

    reading = adc1_get_raw(VDC_ADC_PORT);
    voltage = esp_adc_cal_raw_to_voltage(reading, adc_chars); // Convert to unscaled mV.

    totalVdc             = totalVdc - vdcAvgBuff[avgIndex];
    vdcAvgBuff[avgIndex] = voltage;

    totalVdc  = totalVdc + vdcAvgBuff[avgIndex];
    avgIndex += 1;
    avgIndex  = avgIndex >= E_AVG_SIZE ? 0 : avgIndex;

    voltage = totalVdc / E_AVG_SIZE;
    paVolts = (voltage * VDC_SCALE) / 1000.0f; // Apply Attenuator Scaling, covert from mV to VDC.
    paVolts = constrain(paVolts, 0.0f, 99.0f);
}

// *********************************************************************************************
// initVdcBuffer(): Initialize the Supply Volts Averaging Buffer (fill with current voltage).
void initVdcBuffer(void)
{
    for (uint8_t i = 0; i < E_AVG_SIZE; i++) {
        measureVbatVoltage(); // Prime the Volts averaging buffer.
        measureVdcVoltage();  // Prime the Volts averaging buffer.
        delay(25);
    }
}

// *********************************************************************************************
// processMeasurements(): Periodically perform voltage measurements. Must be called in main loop.
void processMeasurements(void)
{
    unsigned long currentMillis             = millis(); // Snapshot of System Timer.
    static unsigned long previousMeasMillis = millis(); // Timer for Voltage Measurement.

    currentMillis = millis();

    // System Tick Timers Tasks
    if (currentMillis - previousMeasMillis >= MEAS_TIME) {
        previousMeasMillis = currentMillis;
        measureVbatVoltage();
        measureVdcVoltage();
    }
}

// *********************************************************************************************
// EOF
