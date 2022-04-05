/*
   File: radio.cpp
   Project: PixelRadio, an RBDS/RDS FM Transmitter (QN8027 Digital FM IC)
   Version: 1.0
   Creation: Dec-16-2021
   Revised:  Mar-30-2022
   Public Release:
   Project Leader: T. Black (thomastech)
   Contributors: thomastech
   Revision History: See PixelRadio.cpp

   (c) copyright T. Black 2021-2022, Licensed under GNU GPL 3.0 and later, under this license absolutely no warranty is given.
   This Code was formatted with the uncrustify extension.
 */

// *********************************************************************************************
#include <Arduino.h>
#include <ArduinoLog.h>
#include <Wire.h>
#include "PixelRadio.h"
#include "globals.h"
#include "language.h"
#include "QN8027Radio.h"

// *********************************************************************************************

extern QN8027Radio radio;

// *********************************************************************************************
// calibrateAntenna(): Calibrate the QN8027 Antenna Interface. On exit, true if Calibration OK.
// This is an undocumented feature that was found during PixelRadio project development.
// This routine is called once during startup(). Takes about 2 Secs to execute.
bool calibrateAntenna(void)
{
    bool successFlg = true;
    char logBuff[70];
    uint8_t regVal1;
    uint8_t regVal2;

    radio.setFrequency(108.0F); // High end of FM tuning range.
    waitForIdle(50);
    radio.Switch(OFF);
    waitForIdle(15);
    radio.Switch(ON);
    waitForIdle(50);

    radio.reCalibrate();
    waitForIdle(120);

    regVal1 = radio.read1Byte(ANT_REG);

    radio.setFrequency(85.0F); // Low end of FM tuning range.
    waitForIdle(50);
    radio.Switch(OFF);
    waitForIdle(15);
    radio.Switch(ON);
    waitForIdle(50);

    radio.reCalibrate();
    waitForIdle(120);

    regVal2 = radio.read1Byte(ANT_REG);

    sprintf(logBuff, "-> QN8027 RF Port Test: Low RF Range= 0x%02X, High RF Range= 0x%02X", regVal1, regVal2);
    Log.verboseln(logBuff);

    if ((regVal1 <= 0x01) || (regVal1 >= 0x1f) || (regVal2 <= 0x01) || (regVal2 >= 0x1f)) {
        successFlg = false;
        Log.errorln("-> QN8027 RF Port Matching Poor, RF Tuning Range Impaired.");
    }
    else {
        successFlg = true;
        Log.infoln("-> QN8027 RF Port Matching OK");
    }

    /*
       radio.setFrequency(103.0F); // Recalibrate at middle FM range.
       waitForIdle(50);
       radio.Switch(OFF);
       waitForIdle(15);
       radio.Switch(ON);
       waitForIdle(50);
     */

    radio.reCalibrate();
    waitForIdle(120);

    setRfCarrier();
    waitForIdle(50);

    return successFlg;
}

// *********************************************************************************************
//
// checkRadioIsPresent(): Check to see if QN8027 FM Radio Chip is installed. Return true if Ok.
bool checkRadioIsPresent(void) {
    Wire.beginTransmission(QN8027_I2C_ADDR);

    if (Wire.endTransmission(true) == 0) { // Receive 0 = success (ACK response)
        return true;
    }

    return false;
}

/*
   // *********************************************************************************************
   // initRadioChip(); Initialize the QN8027 FM Radio Chip.
   // This is a debug function, not for production USE.
   uint8_t initRadioChipDebug(void) {
    uint8_t testCode = FM_TEST_OK;

    Log.infoln("Initializing QN8027 FM Radio Chip ...");

    Serial.printf("Read 0x06 should be 0x44, is %02x\r\n", radio.read1Byte(0x06));
    radio.write1Byte(0x00,0x81);
    delay(28);
    radio.write1Byte(0x03,0x3A);
    radio.write1Byte(0x04,0x33);
    radio.write1Byte(0x10,0x78);
    radio.write1Byte(0x1F,0x00);
    radio.write1Byte(0x00,0x41);
    delay(17);
    radio.write1Byte(0x00,0x01);
    delay(20);
    radio.write1Byte(0x18,0xE4);
    radio.write1Byte(0x18,0xF0);
    radio.write1Byte(0x00,0x31);
    radio.write1Byte(0x81,0x6A);
    radio.write1Byte(0x02,0x87);
    Serial.printf("Read 0x10 should be 0x78, is %02x\r\n", radio.read1Byte(0x10));
    radio.write1Byte(0x10,0x78);
    Serial.printf("Read 0x00 should be 0x31, is %02x\r\n", radio.read1Byte(0x00));
    radio.write1Byte(0x00,0x11);
    Serial.printf("Read 0x00 should be 0x11, is %02x\r\n", radio.read1Byte(0x00));
    radio.write1Byte(0x00,0x31);
    Serial.printf("Read 0x00 should be 0x31, is %02x\r\n", radio.read1Byte(0x00));
    radio.write1Byte(0x00,0x39);
    Serial.printf("Read 0x00 should be 0x39, is %02x\r\n", radio.read1Byte(0x00));
    radio.write1Byte(0x00,0x29);
    radio.write1Byte(0x01,0x00);
    Serial.printf("Read 0x00 should be 0x29, is %02x\r\n", radio.read1Byte(0x00));
    radio.write1Byte(0x00,0x29);
    Serial.printf("Read 0x00 should be 0x29, is %02x\r\n", radio.read1Byte(0x00));
    radio.write1Byte(0x00,0x21);
    Serial.printf("Read 0x00 should be 0x21, is %02x\r\n", radio.read1Byte(0x00));
    radio.write1Byte(0x00,0x21);
    delay(200);
    Serial.printf("Read 0x00 should be 0x21, is %02x\r\n", radio.read1Byte(0x00));
    radio.write1Byte(0x00,0x21);
    return testCode;
   }
 */

// *********************************************************************************************
// initRadioChip(); Initialize the QN8027 FM Radio Chip. Returns Test Result Code.
uint8_t initRadioChip(void) {
    bool successFlg  = true;
    uint8_t testCode = FM_TEST_OK;
    uint8_t regVal;
    char    logBuff[50];

    Log.infoln("Initializing QN8027 FM Radio Chip ...");

    if (checkRadioIsPresent()) {
        Log.verboseln("-> QN8027 is Present");
    }
    else {
        Log.errorln("-> QN8027 is Missing");
        return FM_TEST_FAIL;
    }

    regVal = radio.read1Byte(CID1_REG);

    if ((regVal > 0x00) && ((regVal & 0x0C) == 0x00)) {
        successFlg = true;
        sprintf(logBuff, "-> CID1 Chip Family ID: 0x%02X", regVal);
        Log.verboseln(logBuff);
    }
    else {
        sprintf(logBuff, "-> Incorrect CID1 Chip Family ID: 0x%02X", regVal);
        Log.errorln(logBuff);

        // return FM_TEST_FAIL;
    }

    regVal = radio.read1Byte(CID2_REG);

    if ((regVal & 0xF0) == 0x40) {
        successFlg = true;
        sprintf(logBuff, "-> CID2 Chip Version: 0x%02X", regVal);
        Log.verboseln(logBuff);
    }
    else {
        sprintf(logBuff, "-> Incorrect CID2 Version: 0x%02X", regVal);
        Log.errorln(logBuff);

        // return FM_TEST_FAIL;
    }

    radio.reset();
    delay(30);

    // sprintf(logBuff,"Radio Reset Status is: %02X", radio.getStatus());
    // Log.infoln(logBuff);

    radio.setClockSource(0x00); // XTAL on pins 1 & 2.
    //    delay(5);
    radio.setCrystalFreq(12);

    //    delay(5);
    radio.setCrystalCurrent(30); // 30% of 400uA Max = 120uA.
    //    delay(5);

    // sprintf(logBuff,"Radio XTal Status is: %02X", radio.getStatus());
    // Log.infoln(logBuff);

    radio.setTxFreqDeviation(0x81); // 75Khz, Total Broadcast channel Bandwidth
    //    delay(10);

    // sprintf(logBuff,"Radio FreqDev Status is: %02X", radio.getStatus());
    // Log.infoln(logBuff);

    radio.setTxPilotFreqDeviation(9); // Use default 9% (6.75KHz) Pilot Tone Deviation.
    //    delay(10);

    // sprintf(logBuff,"Radio TxPilotDeviation Status is: %02X", radio.getStatus());
    // Log.infoln(logBuff);

    setRfPower();
    waitForIdle(25);

    // sprintf(logBuff,"Radio Tx Power Status is: %02X", radio.getStatus());
    // Log.infoln(logBuff);

    for (uint8_t i = 0; i < RADIO_CAL_RETRY; i++) { // Allow several attempts to get good port matching results.
        if (calibrateAntenna()) {                   // QN8027 RF Port Matching OK, exit.
            testCode = FM_TEST_OK;
            break;
        }
        else {
            testCode = FM_TEST_VSWR; // Report High VSWR.

            if (i < RADIO_CAL_RETRY - 1) {
                sprintf(logBuff, "-> Retesting QN8027 RF Port Matching, Retry #%d", i + 1);
                Log.warningln(logBuff);
            }
        }
    }
    waitForIdle(50);

    // sprintf(logBuff,"Calibrate Antenna Status is: %02X", radio.getStatus());
    // Log.infoln(logBuff);

    setPreEmphasis();
    setVgaGain(); // Tx Input Buffer Gain.
    setDigitalGain();
    setAudioImpedance();
    waitForIdle(10);

    // sprintf(logBuff,"Radio Audio Configuration Status: %02X", radio.getStatus());
    // Log.infoln(logBuff);

    radio.MonoAudio(!stereoEnbFlg);
    delay(5);

    // sprintf(logBuff,"Radio Mono Audio Status is: %02X", radio.getStatus());
    // Log.infoln(logBuff);

    radio.scrambleAudio(OFF);
    delay(5);

    // sprintf(logBuff,"Radio Privacy Status is: %02X", radio.getStatus());
    // Log.infoln(logBuff);

    radio.clearAudioPeak();
    delay(5);

    // sprintf(logBuff,"Radio Clear Peak Status is: %02X", radio.getStatus());
    // Log.infoln(logBuff);

    radio.mute(muteFlg);
    delay(5);

    // sprintf(logBuff,"Radio Clear Peak Status is: %02X", radio.getStatus());
    // Log.infoln(logBuff);

    setRfAutoOff();

    // sprintf(logBuff,"Radio RF Auto Shutoff Status is: %02X", radio.getStatus());
    // Log.infoln(logBuff);

    radio.Switch(uint8_t(rfCarrierFlg));
    waitForIdle(50);

    // sprintf(logBuff,"Radio Switch Status is: %02X", radio.getStatus());
    // Log.infoln(logBuff);

    radio.setFrequency((float(fmFreqX10)) / 10.0f);
    waitForIdle(50);
    sprintf(logBuff, "-> Radio Frequency: %3.1f", radio.getFrequency());
    Log.infoln(logBuff);

    radio.setRDSFreqDeviation(10); // RDS Freq Deviation = 0.35KHz * Value.
    delay(1);
    radio.RDS(ON);
    waitForIdle(20);

    // sprintf(logBuff,"Radio RF Carrier Status is: %02X", radio.getStatus());
    // Log.infoln(logBuff);

    radio.updateSYSTEM_REG(); // This is needed to Start FM Broadcast.
    waitForIdle(20);
    radio.clearAudioPeak();
    delay(1);
    sprintf(logBuff, "-> Radio Status: %02X", (radio.getStatus() & 0x07));
    Log.infoln(logBuff);

    radio.setPiCode(rdsLocalPiCode);

    if (successFlg) {
        Log.infoln("-> QN8027 Initialization Complete.");
    }
    else {
        Log.errorln("-> QN8027 Failed Initialization.");
    }

    /*
        // DEBUG ONLY.
        for (int i = 0; i <= 0x1F; i++) {
            sprintf(logBuff, "Radio Register %02X is: %02X", i, radio.read1Byte(i));
            Log.infoln(logBuff);
        }
     */

    return testCode;
}

// *********************************************************************************************
// measureAudioLevel(): Measure the Peak Audio Level. Max 675mV.
uint16_t measureAudioLevel(void) {
    uint16_t mV = 0;

    mV = radio.getStatus() >> 4;
    mV = mV * 45; // Audio Peak is 45mV per count.
    radio.clearAudioPeak();

    return mV;
}

// *********************************************************************************************
// setVgaGain(): Set the Tx Input Buffer Gain (Analog Gain) on the QN8027 chip.
void setVgaGain(void)
{
    uint8_t gainVal = 0;

    // char logBuff[80];
    // sprintf(logBuff, "-> Input Buffer Gain Set to: %s.", vgaGainStr.c_str());
    // Log.verboseln(logBuff);

    if (vgaGainStr == VGA_GAIN0_STR) {
        gainVal = 0x00;
    }
    else if (vgaGainStr == VGA_GAIN1_STR) {
        gainVal = 0x01;
    }
    else if (vgaGainStr == VGA_GAIN2_STR) {
        gainVal = 0x02;
    }
    else if (vgaGainStr == VGA_GAIN3_STR) {
        gainVal = 0x03;
    }
    else if (vgaGainStr == VGA_GAIN4_STR) {
        gainVal = 0x04;
    }
    else if (vgaGainStr == VGA_GAIN5_STR) {
        gainVal = 0x05;
    }
    else {
        Log.errorln("setVgaGain: Invalid Value, Using Default");
        vgaGainStr = VGA_GAIN3_STR;
        gainVal    = 0x03;
    }

    delay(5);
    radio.setTxInputBufferGain(gainVal);
    delay(5);
}

// *********************************************************************************************
// setAudioMute(): Control the Audio Mute on the QN8027 chip.
void setAudioMute(void)
{
    uint8_t mute;

    // char logBuff[40];
    // sprintf(logBuff, "-> Audio Mute Set to: %s", muteFlg ? "On (Muted)." : "Off.");
    // Log.verboseln(logBuff);

    if (muteFlg) {
        mute = ON;
    }
    else {
        mute = OFF;
    }

    delay(5);
    radio.mute(mute);
    delay(5);
}

// *********************************************************************************************
// setAudioImpedance(): Set the Audio Input Impedance on the QN8027 chip.
void setAudioImpedance(void)
{
    uint8_t impedVal = 0;

    // char    logBuff[60];
    // sprintf(logBuff, "-> Audio Impedance Set to: %s.", inpImpedStr.c_str());
    // Log.verboseln(logBuff);

    if (inpImpedStr == INP_IMP05K_STR) {
        impedVal = 5;
    }
    else if (inpImpedStr == INP_IMP10K_STR) {
        impedVal = 10;
    }
    else if (inpImpedStr == INP_IMP20K_STR) {
        impedVal = 20;
    }
    else if (inpImpedStr == INP_IMP40K_STR) {
        impedVal = 40;
    }
    else {
        Log.errorln("setAudioImpedance: Invalid Value, Using Default");
        inpImpedStr = INP_IMP20K_STR;
        impedVal    = 20;
    }

    delay(5);
    radio.setAudioInpImp(impedVal);
    delay(5);
}

// *********************************************************************************************
// setDigitalGain(): Set the Tx Digital Gain on the QN8027 chip.
void setDigitalGain(void)
{
    uint8_t gainVal = 0;

    // char    logBuff[60];
    // sprintf(logBuff, "-> Digital Audio Gain Set to: %s.", digitalGainStr.c_str());
    // Log.verboseln(logBuff);

    if (digitalGainStr == DIG_GAIN0_STR) {
        gainVal = 0; // 0dB.
    }
    else if (digitalGainStr == DIG_GAIN1_STR) {
        gainVal = 1; // 1dB.
    }
    else if (digitalGainStr == DIG_GAIN2_STR) {
        gainVal = 2; // 2dB.
    }
    else {
        Log.errorln("setDigitalGain: Invalid Value, Using Default");
        digitalGainStr = DIG_GAIN0_STR;
        gainVal        = 0;
    }

    delay(5);
    radio.setTxDigitalGain(gainVal);
    delay(5);
}

// *********************************************************************************************
// setFrequency(): Set the Radio Frequency on the QN8027.
void setFrequency(void)
{
    float   freq;
    uint8_t rfOn;

    // char logBuff[60];
    // sprintf(logBuff, "-> Radio Frequency Set to: %d.", fmFreqX10);
    // Log.verboseln(logBuff);

    freq = (float(fmFreqX10)) / 10.0f;

    if (rfCarrierFlg) {
        rfOn = ON;
    }
    else {
        rfOn = OFF;
    }

    waitForIdle(5);
    radio.Switch(OFF); // Turn Off Carrier.
    waitForIdle(5);
    radio.setFrequency(freq);
    waitForIdle(25);
    radio.Switch(rfOn); // Restore Carrier to user's state.
    waitForIdle(25);
}

// *********************************************************************************************
// setMonoAudio(): Control the Audio Stereo/Mono mode on the QN8027 chip.
void setMonoAudio(void)
{
    uint8_t mono;

    // char logBuff[40];
    // sprintf(logBuff, "-> Audio Set to: %s", stereoEnbFlg ? "Mono." : "Stereo.");
    // Log.verboseln(logBuff);

    if (stereoEnbFlg) {
        mono = OFF;
    }
    else {
        mono = ON;
    }

    delay(5);
    radio.MonoAudio(mono);
    delay(5);
}

// *********************************************************************************************
// ssetPreEmphasis(): Set the QN8027 chip's pre-Emphasis.
// ON = 50uS (Eur/UK/Australia), OFF = 75uS (North America/Japan).
void setPreEmphasis(void)
{
    uint8_t emphVal;
    uint8_t rfOn;

    // char logBuff[60];
    // sprintf(logBuff, "-> Pre-Emphasis Set to: %s.", preEmphasisStr.c_str());
    // Log.verboseln(logBuff);

    if (preEmphasisStr == PRE_EMPH_EUR_STR) {
        emphVal = ON;
    }
    else if (preEmphasisStr == PRE_EMPH_USA_STR) {
        emphVal = OFF;
    }
    else {
        Log.errorln("setPreEmphasis: Invalid Value, Using Default");
        preEmphasisStr = PRE_EMPH_USA_STR;
        emphVal        = OFF;
    }

    if (rfCarrierFlg) {
        rfOn = ON;
    }
    else {
        rfOn = OFF;
    }

    delay(1);
    radio.Switch(OFF); // Turn Off Carrier.
    delay(1);
    radio.setPreEmphTime50(emphVal);
    waitForIdle(10);
    radio.Switch(rfOn); // Restore Carrier to user's state.
    waitForIdle(25);
}

// *********************************************************************************************
// setRfAutoOff():
// IMPORTANT: Sending RDS Messages and/or using updateUiAudioLevel() Will prevent 60S Turn Off.
// rfAutoOff = true: Turn-off RF Carrier if Audio is missing for > 60 seconds.
// rfAutoOff = false: Never turn off.
void setRfAutoOff(void)
{
    uint8_t autoOff;
    uint8_t rfOn;

    // char logBuff[60];
    // sprintf(logBuff, "-> RF Auto Off Set to: %s", rfAutoFlg ? "60S Timeout." : "Always On.");
    // Log.verboseln(logBuff);

    if (rfAutoFlg) {
        autoOff = ON;
    }
    else {
        autoOff = OFF;
    }

    if (rfCarrierFlg) {
        rfOn = ON;
    }
    else {
        rfOn = OFF;
    }

    radio.Switch(OFF); // Turn Off Carrier.
    delay(5);
    radio.radioNoAudioAutoOFF(autoOff);
    delay(5);
    radio.Switch(rfOn); // Restore Carrier to user's state.

    // char logBuff[60];
    // sprintf(logBuff, "AutoOff Value = %02Xh\r\n", radio.read1Byte(GPLT_REG));
    // Log.verbose()
}

// *********************************************************************************************
// setRfCarrier(): Set the QN8027 chip's RF Carrier Output (On/Off).
void setRfCarrier(void)
{
    uint8_t rfOn;

    // char    logBuff[60];
    // sprintf(logBuff, "-> RF Carrier Set to: %s", rfCarrierFlg ? "On" : "Off");
    // Log.verboseln(logBuff);

    if (rfCarrierFlg) {
        rfOn = ON;
    }
    else {
        rfOn = OFF;
    }

    waitForIdle(10);
    radio.Switch(rfOn); // Update QN8027 Carrier.
    waitForIdle(25);
}

// *********************************************************************************************
// setRfPower(): Set the QN8027 chip's RF Power Output. Max 121dBuVp.
// Note: Power is not changed until RF Carrier is toggled Off then On.
void setRfPower(void)
{
    uint8_t pwrVal = RF_HIGH_POWER;

    // char    logBuff[60];
    // sprintf(logBuff, "-> RF Power Set to: %s", rfPowerStr.c_str());
    // Log.verboseln(logBuff);

    if (rfPowerStr == RF_PWR_LOW_STR) {
        pwrVal = RF_LOW_POWER;
    }
    else if (rfPowerStr == RF_PWR_MED_STR) {
        pwrVal = RF_MED_POWER;
    }
    else if (rfPowerStr == RF_PWR_HIGH_STR) {
        pwrVal = RF_HIGH_POWER;
    }
    else {
        Log.errorln("setRfPower: Invalid Value, Using High Power");
        rfPowerStr = RF_PWR_HIGH_STR;
        pwrVal     = RF_HIGH_POWER;
    }

    waitForIdle(10);
    radio.setTxPower(pwrVal);
    waitForIdle(10);

    if (rfCarrierFlg) {
        radio.Switch(OFF); // Turn Off Carrier.
    }

    setRfCarrier();        // Restore runtime RF Carrier state.
    waitForIdle(50);
}

// *********************************************************************************************
// updateOnAirSign(): Turn on the "On Air" LED Sign if RF Carrier is present.
void updateOnAirSign(void)
{
    if (rfCarrierFlg) {
        digitalWrite(ON_AIR_PIN, SIGN_ON);
    }
    else {
        digitalWrite(ON_AIR_PIN, SIGN_OFF);
    }
}

// *********************************************************************************************
// updateRadioSettings(): Update Any Radio Setting that has changed by the Web UI.
// This routine must be placed in main loop();
// QN8027 specific settings must not be changed in the Web UI Callbacks due to ESP32 core
// limitations. So instead the callbacks set a flag that tells this routine to perform the action.
void updateRadioSettings(void)
{
    if (newVgaGainFlg) {
        newVgaGainFlg = false;
        setVgaGain(); // Update analog (VGA) gain setting on QN8027 FM Radio Chip.
    }

    if (newAutoRfFlg) {
        newAutoRfFlg = false;
        setRfAutoOff();
    }

    if (newDigGainFlg) {
        newDigGainFlg = false;
        setDigitalGain(); // Update setting on QN8027 FM Radio Chip.
    }

    if (newFreqFlg) {
        newFreqFlg = false;
        setFrequency();
    }

    if (newInpImpFlg) {
        newInpImpFlg = false;
        setAudioImpedance(); // Update Impednace setting on QN8027 FM Radio Chip.
    }

    if (newAudioModeFlg) {
        newAudioModeFlg = false;
        setMonoAudio();
    }

    if (newMuteFlg) {
        newMuteFlg = false;
        setAudioMute(); // Update QN8027 Mute Register.
    }

    if (newPreEmphFlg) {
        newPreEmphFlg = false;
        setPreEmphasis(); // Update QN8027 Radio Chip's setting.
    }

    if (newRfPowerFlg) {
        newRfPowerFlg = false;
        setRfPower();    // Update RF Power Setting on QN8027 FM Radio Chip.
    }

    if (newCarrierFlg) { // Update RF Carrier last for best results.
        newCarrierFlg = false;
        setRfCarrier();
    }
}

// *********************************************************************************************
// waitForIdle(): Wait for QN8027 to enter Idle State. This means register command has executed.
void waitForIdle(uint16_t waitMs) {
    // char logBuff[50];
    uint8_t stateCode1;
    uint8_t stateCode2;

    stateCode1 = radio.getStatus() & 0x07;

    // sprintf(logBuff,"waitForIdle Start Status is: %02X", stateCode1);
    // Serial.println(logBuff);

    delay(5);   // Fixed Entry Delay.

    if (waitMs <= 5) {
        return; // Nothing more to do, exit.
    }
    else if (waitMs < 10) {
        waitMs = 5;
    }
    else {
        waitMs -= 5;
    }

    for (int i = 0; i < waitMs / 5; i++) {
        delay(5);
        stateCode2 = radio.getStatus() & 0x07;

        // sprintf(logBuff,"waitForIdle Cnt: #%d, Status is: %02X", i, stateCode2);
        // Serial.println(logBuff);
        if ((stateCode2 == 0x02) || (stateCode2 == 0x05)) {
            break;
        }
        else if  (stateCode1 != stateCode2) {
            radio.getStatus(); // Clear next status byte.
            break;
        }
    }

    // sprintf(logBuff,"waitForIdle End Status is: %02X", (radio.getStatus() & 0x07));
    // Serial.println(logBuff);
}

// *********************************************************************************************
// OEF
