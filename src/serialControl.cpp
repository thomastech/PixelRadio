/*
   File: serialControl.cpp
   Project: PixelRadio, an RBDS/RDS FM Transmitter (QN8027 Digital FM IC)
   Version: 1.0
   Creation: Dec-16-2021
   Revised:  Mar-10-2022
   Public Release:
   Project Leader: T. Black (thomastech)
   Contributors: thomastech
   Revision History: See PixelRadio.cpp

   (c) copyright T. Black 2021-2022, Licensed under GNU GPL 3.0 and later, under this
   license absolutely no warranty is given.
   This Code was formatted with the uncrustify extension.

    USB Serial versus Logic Level Serial:
    =====================================
    PixelRadio's default Serial Controller uses USB Serial (Serial-0).
    GPIO "Logic Level" Serial is possible by replacing the RBD_SerialManager.cpp and
    RBD_SerialManager.h files in the library with patched version stored in the
    /extras folder.

 */

// ************************************************************************************************
#include <ArduinoLog.h>
#include "RBD_SerialManager.h"
#include "config.h"
#include "language.h"
#include "PixelRadio.h"
#include "globals.h"

// ************************************************************************************************

RBD::SerialManager serial_manager;

String cmdStr;   // Serial Port Commands from user (CLI).
String paramStr; // Parameter string.

// ================================================================================================
// ctrlSerialFlg(): Return true if Serial Controller is Enabled, else false;
bool ctrlSerialFlg(void) {
    if (ctrlSerialStr == SERIAL_OFF_STR) {
        return false;
    }
    return true;
}

// ================================================================================================
void initSerialControl(void)
{
    cmdStr.reserve(40);                   // Minimize memory re-allocations.
    paramStr.reserve(80);                 // Minimize memory re-allocations.

    Serial.flush();                       // Purge pending serial chars.
    serial_manager.start();               // Start Command Line Processor.
    serial_manager.setFlag(CMD_EOL_TERM); // EOL Termination character.
    serial_manager.setDelimiter('=');     // Parameter delimiter character.
    Serial.flush();                       // Repeat the flushing.
    Log.infoln("Serial Controller CLI is Enabled.");
}

// ================================================================================================
// gpioSerialControl(): Serial handler for GPIO Commands.
void gpioSerialControl(String paramStr, uint8_t pin)
{
    bool successFlg = true;
    char charBuff[60];

    // sprintf(charBuff, "Serial Controller: Received GPIO Pin-%d Command", pin);
    // Log.infoln(charBuff);

    successFlg = gpioCmd(paramStr, SERIAL_CNTRL, pin);

    if (!successFlg) {
        sprintf(charBuff, "{\"%s%d\": \"fail\"}", CMD_GPIO_STR, pin);
    }
    else if (paramStr == CMD_GPIO_READ_STR) {
        sprintf(charBuff, "{\"%s%d\": \"%d\"}", CMD_GPIO_STR, pin, digitalRead(pin));
    }
    else {
        sprintf(charBuff, "{\"%s%d\": \"ok\"}", CMD_GPIO_STR, pin);
    }
    serial_manager.println(charBuff);
}

// ================================================================================================
// serialCommands(): Process the commands sent through the serial port.
// This is the Command Line Interface (CLI).
void serialCommands(void)
{
    char printBuff[140 + sizeof(VERSION_STR) + STA_NAME_MAX_SZ];

    if (!ctrlSerialFlg()) { // Serial Controller disabled, nothing to do. Exit.
        return;
    }

    if (serial_manager.onReceive()) { // Process any serial commands from user (CLI).
        cmdStr = serial_manager.getCmd();
        cmdStr.trim();
        cmdStr.toLowerCase();

        // serial_manager.print("Raw CMD Parameter: "); // Debug message.
        // serial_manager.println(cmdStr);

        paramStr = serial_manager.getParam();
        paramStr.trim();

        // serial_manager.print("Raw CLI Parameter: "); // Debug message.
        // serial_manager.println(paramStr);


        if ((cmdStr == "?") || (cmdStr == "h") || (cmdStr == "help")) {
            serial_manager.println(                                         "");
            serial_manager.println("=========================================");
            serial_manager.println("**  SERIAL CONTROLLER COMMAND SUMMARY  **");
            serial_manager.println("=========================================");
            serial_manager.println(     " AUDIO MODE      : aud=mono : stereo");

            sprintf(printBuff, " FREQUENCY X10   : freq=%d <-> %d", FM_FREQ_MIN_X10, FM_FREQ_MAX_X10);
            serial_manager.println(                                          printBuff);

            serial_manager.println(" GPIO-19 CONTROL : gpio19=read : outhigh : outlow");
            serial_manager.println(" GPIO-23 CONTROL : gpio23=read : outhigh : outlow");
            serial_manager.println(" GPIO-33 CONTROL : gpio33=read : outhigh : outlow");

            serial_manager.println(                   " INFORMATION     : info=system");
            serial_manager.println(                 " MUTE AUDIO      : mute=on : off");

            sprintf(printBuff, " PROG ID CODE    : pic=0x%04X <-> 0x%04X", RDS_PI_CODE_MIN, RDS_PI_CODE_MAX);
            serial_manager.println(printBuff);

            sprintf(printBuff, " PROG SERV NAME  : psn=[%d char name]", CMD_PSN_MAX_SZ);
            serial_manager.println(printBuff);

            sprintf(printBuff, " RADIOTXT MSG    : rtm=[%d char message]", CMD_RT_MAX_SZ);
            serial_manager.println(printBuff);

            sprintf(printBuff, " RADIOTXT PERIOD : rtper=5 <-> %d secs", RDS_DSP_TM_MAX);
            serial_manager.println(                                  printBuff);

            serial_manager.println(         " REBOOT SYSTEM   : reboot=system");
            serial_manager.println(             " START RDS       : start=rds");
            serial_manager.println(              " STOP RDS        : stop=rds");
            serial_manager.println(  " LOG CONTROL     : log=silent : restore");
            serial_manager.println("=========================================");
            serial_manager.println(                                         "");
        }
        else if (cmdStr == CMD_AUDMODE_STR) {
            if (audioModeCmd(paramStr, SERIAL_CNTRL)) {
                sprintf(printBuff, "{\"%s\": \"ok\"}", CMD_AUDMODE_STR);
            }
            else {
                sprintf(printBuff, "{\"%s\": \"fail\"}", CMD_AUDMODE_STR);
            }
            serial_manager.println(printBuff);
        }
        else if (cmdStr == CMD_FREQ_STR) {
            if (frequencyCmd(paramStr, SERIAL_CNTRL)) {
                sprintf(printBuff, "{\"%s\": \"ok\"}", CMD_FREQ_STR);
            }
            else {
                sprintf(printBuff, "{\"%s\": \"fail\"}", CMD_FREQ_STR);
            }
            serial_manager.println(printBuff);
        }
        else if (cmdStr == CMD_GPIO19_STR) {
            gpioSerialControl(paramStr, GPIO19_PIN);
        }
        else if (cmdStr == CMD_GPIO23_STR) {
            gpioSerialControl(paramStr, GPIO23_PIN);
        }
        else if (cmdStr == CMD_GPIO33_STR) {
            gpioSerialControl(paramStr, GPIO33_PIN);
        }
        else if (cmdStr == CMD_INFO_STR) {
            if (infoCmd(paramStr, SERIAL_CNTRL)) {
                sprintf(printBuff,
                        "{\"%s\": \"ok\", \"version\": \"%s\", \"hostName\": \"%s\", \"ip\": \"%s\", \"rssi\": %d, \"status\": \"0x%02X\"}",
                        CMD_INFO_STR,
                        VERSION_STR,
                        staNameStr.c_str(),
                        WiFi.localIP().toString().c_str(),
                        WiFi.RSSI(),
                        getControllerStatus());
            }
            else {
                sprintf(printBuff, "{\"%s\": \"fail\"}", CMD_INFO_STR);
            }
            serial_manager.println(printBuff);
        }
        else if (cmdStr == SERIAL_LOG_STR) {
            if (logCmd(paramStr, SERIAL_CNTRL)) {
                sprintf(printBuff, "{\"%s\": \"ok\"}", SERIAL_LOG_STR);
            }
            else {
                sprintf(printBuff, "{\"%s\": \"fail\"}", SERIAL_LOG_STR);
            }
            serial_manager.println(printBuff);
        }
        else if (cmdStr == CMD_MUTE_STR) {
            if (muteCmd(paramStr, SERIAL_CNTRL)) {
                sprintf(printBuff, "{\"%s\": \"ok\"}", CMD_MUTE_STR);
            }
            else {
                sprintf(printBuff, "{\"%s\": \"fail\"}", CMD_MUTE_STR);
            }
            serial_manager.println(printBuff);
        }
        else if (cmdStr == CMD_PICODE_STR) {
            if (piCodeCmd(paramStr, SERIAL_CNTRL)) {
                sprintf(printBuff, "{\"%s\": \"ok\"}", CMD_PICODE_STR);
            }
            else {
                sprintf(printBuff, "{\"%s\": \"fail\"}", CMD_PICODE_STR);
            }
            serial_manager.println(printBuff);
        }
        else if (cmdStr == CMD_PTYCODE_STR) {
            if (ptyCodeCmd(paramStr, SERIAL_CNTRL)) {
                sprintf(printBuff, "{\"%s\": \"ok\"}", CMD_PTYCODE_STR);
            }
            else {
                sprintf(printBuff, "{\"%s\": \"fail\"}", CMD_PTYCODE_STR);
            }
            serial_manager.println(printBuff);
        }
        else if (cmdStr == CMD_PSN_STR) {
            if (programServiceNameCmd(paramStr, SERIAL_CNTRL)) {
                sprintf(printBuff, "{\"%s\": \"ok\"}", CMD_PSN_STR);
            }
            else {
                sprintf(printBuff, "{\"%s\": \"fail\"}", CMD_PSN_STR);
            }
            serial_manager.println(printBuff);
        }
        else if (cmdStr == CMD_RADIOTEXT_STR) {
            if (radioTextCmd(paramStr, SERIAL_CNTRL)) {
                sprintf(printBuff, "{\"%s\": \"ok\"}", CMD_RADIOTEXT_STR);
            }
            else {
                sprintf(printBuff, "{\"%s\": \"fail\"}", CMD_RADIOTEXT_STR);
            }
            serial_manager.println(printBuff);
        }
        else if (cmdStr == CMD_START_STR) {
            if (startCmd(paramStr, SERIAL_CNTRL)) {
                sprintf(printBuff, "{\"%s\": \"ok\"}", CMD_START_STR);
            }
            else {
                sprintf(printBuff, "{\"%s\": \"fail\"}", CMD_START_STR);
            }
            serial_manager.println(printBuff);
        }
        else if (cmdStr == CMD_STOP_STR) {
            if (stopCmd(paramStr, SERIAL_CNTRL)) {
                sprintf(printBuff, "{\"%s\": \"ok\"}", CMD_STOP_STR);
            }
            else {
                sprintf(printBuff, "{\"%s\": \"fail\"}", CMD_STOP_STR);
            }
            serial_manager.println(printBuff);
        }
        else if (cmdStr == CMD_PERIOD_STR) {
            if (rdsTimePeriodCmd(paramStr, SERIAL_CNTRL)) {
                sprintf(printBuff, "{\"%s\": \"ok\"}", CMD_PERIOD_STR);
            }
            else {
                sprintf(printBuff, "{\"%s\": \"fail\"}", CMD_PERIOD_STR);
            }
            serial_manager.println(printBuff);
        }
        else if (cmdStr == CMD_REBOOT_STR) {
            if (rebootCmd(paramStr, SERIAL_CNTRL)) {
                sprintf(printBuff, "{\"%s\": \"ok\"}", CMD_REBOOT_STR);
            }
            else {
                sprintf(printBuff, "{\"%s\": \"fail\"}", CMD_REBOOT_STR);
            }
            serial_manager.println(printBuff);
        }
        else {
            if (cmdStr == "") {                                 // Just a CR, no Text.
                sprintf(printBuff, "{\"cmd\": \"ready\"}");     // JSON Fmt.
            }
            else {
                sprintf(printBuff, "{\"cmd\": \"undefined\"}"); // JSON Fmt.
            }
            serial_manager.println(printBuff);
        }
        Serial.flush(); // Purge the serial controller to prevent conflicts with serialLog.
    }
}

//
// EOF
