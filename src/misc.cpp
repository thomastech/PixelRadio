/*
   File: misc.cpp
   Project: PixelRadio, an RBDS/RDS FM Transmitter (QN8027 Digital FM IC)
   Version: 1.1.0
   Creation: Dec-16-2021
   Revised:  Jun-13-2022
   Revision History: See PixelRadio.cpp
   Project Leader: T. Black (thomastech)
   Contributors: thomastech

   (c) copyright T. Black 2021-2022, Licensed under GNU GPL 3.0 and later, under this license absolutely no warranty is given.
   This Code was formatted with the uncrustify extension.
 */

// *********************************************************************************************
#include <Arduino.h>
#include <ArduinoLog.h>
#include <EEPROM.h>
#include <SPI.h>
#include <Wire.h>
#include "QN8027Radio.h"
#include "PixelRadio.h"
#include "globals.h"
#include "language.h"

// *********************************************************************************************

extern QN8027Radio radio;

// *********************************************************************************************

// addChipID(): Add the ESP32 Chip ID to the provided name. Used to create unique host names.
const String addChipID(const char *name) {
    uint32_t chipid = 0;
    char     buff[50];
    String   idStr;

    for (int i = 0; i < 17; i = i + 8) {
        chipid |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
    }

    snprintf(buff, SSID_MAX_SZ, "%s-%08X", name, chipid);
    idStr = buff;

    return idStr;
}

// *********************************************************************************************
uint8_t i2cScanner(void)
{
    char   logBuff[50];
    byte   count = 0;
    String devStr;

    Log.infoln("Scanning i2c for Devices ...");

    for (byte addr = 0x01; addr < 0x7f; addr++) {
        Wire.beginTransmission(addr);        // Begin I2C transmission Address (i)

        if (Wire.endTransmission(true) == 0) // Receive 0 = success (ACK response)
        {
            switch (addr) {
              case QN8027_I2C_ADDR:
                  devStr = "FM Tx QN8027";
                  break;
              default:
                  devStr = "Unknown";
            }

            sprintf(logBuff, "-> Found i2c address: 0x%02X (%s)", addr, devStr.c_str());
            Log.infoln(logBuff);
            count++;
        }
    }

    if (count == 0) {
        Log.errorln("-> No i2c Devices Found.");
    }
    else {
        Log.infoln("-> Scanning Complete, Found %u i2c Devices.", count);
    }
    return count;
}

// *********************************************************************************************
// initEprom():  Initialize EEPROM emulation.
// Note: EEPROM is Currently not used in PixelRadio; Available for future use.
void initEprom(void)
{
    EEPROM.begin(EEPROM_SZ);

    if (EEPROM.read(INIT_ADDR) != INIT_BYTE)
    {
        EEPROM.write(INIT_ADDR, INIT_BYTE);

        // EEPROM.write(VOL_SET_ADDR, spkrVolSwitch);
        EEPROM.commit();
        Log.warningln("Initialized Virgin EEPROM (detected first use).");
    }
    else
    {
        // spkrVolSwitch = EEPROM.read(VOL_SET_ADDR);
        Log.verboseln("Restored settings from EEPROM.");
    }
}

// *********************************************************************************************
// rebootSystem(): Reboot the System if global rebootFlg = true.
//                 Function must be in main loop. But can also be used in a function.
//                 Do not directly call this function in a ESPUI callback. Use main loop instead.
void rebootSystem(void)
{
    String tmpStr;

    if (rebootFlg) {
        tmpStr  = "USER REBOOT!";
        tmpStr += "\r\n\r\n";
        tmpStr += "          ******************\r\n";
        tmpStr += "          * SYSTEM REBOOT! *\r\n";
        tmpStr += "          ******************\r\n\r\n";
        Serial.println(tmpStr.c_str()); // MUST use default serial print, not serial Log.

        Serial.flush();
        delay(1000);
        ESP.restart();
    }
}

// *********************************************************************************************
// setGpioBootPins(): Set the GPIO Pin Boot States using Web UI settings.
void setGpioBootPins(void)
{
    if (gpio19BootStr == GPIO_OUT_LO_STR) {
        pinMode(GPIO19_PIN, OUTPUT);
        digitalWrite(GPIO19_PIN, LOW);
    }
    else if (gpio19BootStr == GPIO_OUT_HI_STR) {
        pinMode(GPIO19_PIN, OUTPUT);
        digitalWrite(GPIO19_PIN, HIGH);
    }
    else if (gpio19BootStr == GPIO_INP_FT_STR) {
        pinMode(GPIO19_PIN, INPUT);
    }
    else if (gpio19BootStr == GPIO_INP_PU_STR) {
        pinMode(GPIO19_PIN, INPUT_PULLUP);
    }
    else if (gpio19BootStr == GPIO_INP_PD_STR) {
        pinMode(GPIO19_PIN, INPUT_PULLDOWN);
    }
    else {
        Log.errorln("-> setGpioBootPins: GPIO-19 Has Invalid Value.");
    }

    if (gpio23BootStr == GPIO_OUT_LO_STR) {
        pinMode(GPIO23_PIN, OUTPUT);
        digitalWrite(GPIO23_PIN, LOW);
    }
    else if (gpio23BootStr == GPIO_OUT_HI_STR) {
        pinMode(GPIO23_PIN, OUTPUT);
        digitalWrite(GPIO23_PIN, HIGH);
    }
    else if (gpio23BootStr == GPIO_INP_FT_STR) {
        pinMode(GPIO23_PIN, INPUT);
    }
    else if (gpio23BootStr == GPIO_INP_PU_STR) {
        pinMode(GPIO23_PIN, INPUT_PULLUP);
    }
    else if (gpio23BootStr == GPIO_INP_PD_STR) {
        pinMode(GPIO23_PIN, INPUT_PULLDOWN);
    }
    else {
        Log.errorln("-> setGpioBootPins: GPIO-23 Has Invalid Value.");
    }

    if (gpio33BootStr == GPIO_OUT_LO_STR) {
        pinMode(GPIO33_PIN, OUTPUT);
        digitalWrite(GPIO33_PIN, LOW);
    }
    else if (gpio33BootStr == GPIO_OUT_HI_STR) {
        pinMode(GPIO33_PIN, OUTPUT);
        digitalWrite(GPIO33_PIN, HIGH);
    }
    else if (gpio33BootStr == GPIO_INP_FT_STR) {
        pinMode(GPIO33_PIN, INPUT);
    }
    else if (gpio33BootStr == GPIO_INP_PU_STR) {
        pinMode(GPIO33_PIN, INPUT_PULLUP);
    }
    else if (gpio33BootStr == GPIO_INP_PD_STR) {
        pinMode(GPIO33_PIN, INPUT_PULLDOWN);
    }
    else {
        Log.errorln("-> setGpioBootPins: GPIO-33 Has Invalid Value.");
    }
}

// *********************************************************************************************
// spiSdCardShutDown(): Shutdown SD Card SPI Interface and idle important ESP32 Boot Mode Pins.
// This function should be called after any SPI usage (such as RD/WR SD Card). It will allow for
// more reliable Code Flashing if the SD Card remains installed.
void spiSdCardShutDown(void) {
    pinMode(SD_CS_PIN, OUTPUT);          // SD Card Chip Select.
    digitalWrite(SD_CS_PIN, HIGH);       // CS Active Low. High = Disable SD Card.

    pinMode(MISO_PIN,   INPUT_PULLDOWN); // SD D0, Allow pin to Pulldown Low (for reliable Flashing).
    pinMode(MOSI_PIN,   INPUT_PULLUP);   // SD CMD, Allow pin to Pullup High (for reliable Flashing).
    pinMode(SD_CLK_PIN, INPUT_PULLUP);   // SD CLK.
}

// *********************************************************************************************
// strIsUint(): Check if string is unsigned integer. Return true if uint.
bool strIsUint(String intStr) {
    uint8_t i;

    if(intStr.length() == 0) {
        return false;
    }

    for (i = 0; i < intStr.length(); i++) { // Arg must be integer >= 0.
        char c = intStr.charAt(i);

        if ((c == '-') || (c < '0') || (c > '9')) {
            return false;
        }
    }
    return true;
}

// *********************************************************************************************
// toneInit(): Initialize the test tone generator (ESP32 led write) hardware.
//             This function should be called in setup().
void toneInit(void)
{
    ledcSetup(TEST_TONE_CHNL, 1000, 8);
}

// *********************************************************************************************
// toneOff(): Turn off the test tone generator.
void toneOff(uint8_t pin, uint8_t channel)
{
    ledcDetachPin(pin);
    ledcWrite(channel, 0);
}

// *********************************************************************************************
// toneOn(): Turn on the test tone generator.
void toneOn(uint8_t pin, uint16_t freq, uint8_t channel)
{
    if (ledcRead(channel)) {
        Log.warningln("Ignored Tone Request: Channel %u is already in-use", channel);
        return;
    }
    ledcAttachPin(pin, channel);
    ledcWriteTone(channel, freq);
}

// *********************************************************************************************
// updateGpioBootPins(): Update the GPIO Boot Pins if the Web UI has changed GPIO settings.
void updateGpioBootPins(void) {
    if (newGpio19Flg || newGpio23Flg || newGpio33Flg) {
        newGpio19Flg = false;
        newGpio23Flg = false;
        newGpio33Flg = false;
        setGpioBootPins();
    }
}

// *********************************************************************************************
// updateTestTones(): Test Tone mode creates cascading audio tones for Radio Installation Tests.
//                    On entry, true will Reset Tone Sequence.
//                    The tone sequence is sent every five seconds.
//                    Also Sends Special RadioText message with approximate Elapsed Time.
void updateTestTones(bool resetTimerFlg)
{
    char rdsBuff[sizeof(AUDIO_TEST_STR) + 25];
    static bool rstFlg          = false; // Reset State Machine FLag.
    static bool goFlg           = false; // Go Flag, send new tone sequence if true.
    static bool toneFlg         = false; // Tone Generator is On Flag.
    static uint8_t  hours       = 0;
    static uint8_t  minutes     = 0;
    static uint8_t  seconds     = 0;
    static uint8_t  state       = 0;
    static uint32_t clockMillis = millis();
    static uint32_t timerMillis = millis();
    uint32_t currentMillis      = millis();

    const uint16_t toneList[] =
    { TONE_NONE, TONE_NONE, TONE_NONE, TONE_A3, TONE_E4, TONE_A3, TONE_C4, TONE_C5, TONE_F4, TONE_F4, TONE_A4, TONE_NONE };
    const uint8_t listSize = sizeof(toneList) / sizeof(uint16_t);

    if (resetTimerFlg) { // Caller requests a full state machine reset on next Test Tone.
        rstFlg = true;
        return;
    }
    else if (!testModeFlg) {
        goFlg = false;
        state = 0;
        digitalWrite(MUX_PIN, TONE_OFF); // Switch Audio Mux chip to Line-In.

        if (toneFlg == true) {           // Kill active tone generator.
            toneFlg = false;
            toneOff(TONE_PIN, TEST_TONE_CHNL);
        }
        return;
    }

    digitalWrite(MUX_PIN, TONE_ON); // Switch Audio Mux chip to Test Tones.

    if (rstFlg == true) {           // State machine reset was requested.
        rstFlg      = false;
        goFlg       = true;         // Request tone sequence now.
        clockMillis = millis();
        timerMillis = clockMillis;
        hours       = 0;
        minutes     = 0;
        seconds     = 0;
        state       = 0;
    }

    // Update the test tone clock. HH:MM:SS will be sent as RadioText.
    if ((currentMillis - clockMillis) >= 1000) {
        clockMillis = millis() - ((currentMillis - clockMillis) - 1000);
        seconds++;

        if (seconds >= 60) {
            seconds = 0;
            minutes++;

            if (minutes >= 60) {
                minutes = 0;
                hours++;

                if (hours >= 100) { // Clock wraps at 99:59:59.
                    hours = 0;
                }
            }
        }
    }

    if (seconds % 5 == 0) { // Send new tone sequence every five seconds.
        goFlg = true;
    }

    if (goFlg && (millis() >= timerMillis + TEST_TONE_TIME)) {
        timerMillis = millis();
        toneFlg     = false;
        toneOff(TONE_PIN, TEST_TONE_CHNL);
        delay(5);               // Allow a bit of time for tone channel to shutdown.

        if (state < listSize) { // Cycle through the tone table.
            if (state == 0) {   // Time to send RadioText message.
                state++;
                sprintf(rdsBuff, "%s  [ %02u:%02u:%02u ]", AUDIO_TEST_STR, hours, minutes, seconds);
                String tmpStr = rdsBuff;
                radio.sendStationName(AUDIO_PSN_STR);
                radio.sendRadioText(tmpStr);
                updateUiRdsText(tmpStr);
                updateUiRDSTmr(0);     // Clear Displayed Elapsed Timer.
                Log.verboseln("New Test Tone Sequence, RadioText Sent.");
                return;                // We will send the tones on next entry.
            }

            if (toneList[state] > 0) { // Time to send tones from toneList[] table).
                toneFlg = true;
                toneOn(TONE_PIN, toneList[state], TEST_TONE_CHNL);
            }

            state++;
        }
        else { // At end of Tone table, done with this sequence.
            state = 0;
            goFlg = false;
        }
    }
}
