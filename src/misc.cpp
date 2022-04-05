/*
   File: misc.cpp
   Project: PixelRadio, an RBDS/RDS FM Transmitter (QN8027 Digital FM IC)
   Version: 1.0
   Creation: Dec-16-2021
   Revised:  Mar-03-2022
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
#include <EEPROM.h>
#include <SPI.h>
#include <Tone32.h>
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
    pinMode(SD_CS_PIN, OUTPUT);        // SD Card Chip Select.
    digitalWrite(SD_CS_PIN, HIGH);     // CS Active Low. High = Disable SD Card.

    pinMode(MISO_PIN, INPUT_PULLDOWN); // SD D0, Allow pin to Pulldown Low (for reliable Flashing).
    pinMode(MOSI_PIN,   INPUT_PULLUP); // SD CMD, Allow pin to Pullup High (for reliable Flashing).
    pinMode(SD_CLK_PIN, INPUT_PULLUP); // SD CLK.
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
// updateTestTones(): If in Test Mode then Produce Audible Test Tones for Radio Installation Testing.
//                    Also Sends Special RadioText message with Elapsed Time.
//                    On entry, true==Reset Tone Sequence.
void updateTestTones(bool resetTimerFlg)
{
    char rdsBuff[50];
    static uint8_t state = 0;
    long seconds;
    long minutes;
    long hours;
    uint32_t secs;
    const uint16_t toneList[]   = { NOTE_A3, NOTE_E4, NOTE_A3, NOTE_C4, NOTE_C5, NOTE_F4, NOTE_F4, NOTE_A4, 0, 0, 0 };
    const uint8_t  listSize     = sizeof(toneList) / sizeof(uint16_t);
    uint32_t clockMillis        = millis();
    static uint32_t baseMillis  = millis();
    static uint32_t timerMillis = millis();

    if (!testModeFlg) {
        digitalWrite(MUX_PIN, TONE_OFF); // Switch Audio Mux chip to Line-In.
        return;
    }
    else if (resetTimerFlg) {   // Reset Tone Sequence.
        baseMillis  = millis();
        timerMillis = baseMillis - TEST_TONE_TIME;
        state       = 0;
        return;                          // Must return here (prevents exception error from UI callback);
    }

    digitalWrite(MUX_PIN, TONE_ON);      // Switch Audio Mux chip to Test Tones.

    clockMillis = millis() - baseMillis; // Elasped Time.
    secs        =  clockMillis / MSECS_PER_SEC;
    hours       = numberOfHours(secs);
    minutes     = numberOfMinutes(secs);
    seconds     = numberOfSeconds(secs);

    if (millis() >= timerMillis + TEST_TONE_TIME) {
        timerMillis = millis();

        if (state < listSize) {
            if (state == 0) {
                sprintf(rdsBuff, "PIXELRADIO AUDIO TEST  [ %02ld:%02ld:%02ld ]", hours, minutes, seconds);
                radio.sendStationName("TestTone");
                String tmpStr = rdsBuff;
                radio.sendRadioText(tmpStr);
                updateUiRdsText(tmpStr);
                updateUiRDSTmr(0); // Clear Displayed Elapsed Timer.
                delay(100);        // Wait for RDS Buffer to empty before applying Tones.
                tone(TONE_PIN, toneList[state], 250, TEST_TONE_CHNL);
                Log.verboseln("New Test Tone Sequence, RadioText Sent.");
            }
            else if (toneList[state] > 0) {
                tone(TONE_PIN, toneList[state], 250, TEST_TONE_CHNL);
            }
            else {
                noTone(TONE_PIN, TEST_TONE_CHNL);
            }
            state++;
        }
        else {
            state = 0;
        }
    }
}
