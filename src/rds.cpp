/*
   File: rds.cpp
   Project: PixelRadio, an RBDS/RDS FM Transmitter (QN8027 Digital FM IC)
   Version: 1.0
   Creation: Dec-16-2021
   Revised:  Mar-08-2022
   Public Release:
   Project Leader: T. Black (thomastech)
   Contributors: thomastech
   Revision History: See PixelRadio.cpp

   (c) copyright T. Black 2021-2022, Licensed under GNU GPL 3.0 and later, under this
   license absolutely no warranty is given.
   This Code was formatted with the uncrustify extension.

   Note 1: RDS RadioText is sent in a 57KHz sub-carrier. Equivalent bit rate is ~475 baud.
 */

// ************************************************************************************************

#include <Arduino.h>
#include <ArduinoLog.h>
#include <string>
#include "PixelRadio.h"
#include "globals.h"
#include "QN8027Radio.h"

// ************************************************************************************************
extern QN8027Radio radio;

// ************************************************************************************************
// checkactiveTextAvail(): Determine if Serial, HTTP, or MQTT controller is currently sending
// RadioText.
bool checkActiveTextAvail(void) {
    bool availFlg = false;

    if (activeTextSerialFlg || activeTextHttpFlg || activeTextMqttFlg) {
        availFlg = true;
    }
    return availFlg;
}

// ************************************************************************************************
// checkControllerIsAvailable(): Using the supplied Controller ID, check if any HIGHER priority
// controllers are in operation. If none are present, then the supplied controller ID won't be
// blocked and is available to use.
// true = Controller is ready, false = higher priority controller is active.
bool checkControllerIsAvailable(uint8_t controller) {
    bool availFlg = true;

    if (controller == SERIAL_CNTRL) {                   // Priority #1, can never be blocked. Always available.
        return true;
    }
    else if (controller == MQTT_CNTRL) {                // Priority #2. Only Serial Controller can block it.
        if (activeTextSerialFlg) {                      // Higher Priority Controller Active;
            availFlg = false;                           // This controller is unavailable.
        }
    }
    else if (controller == HTTP_CNTRL) {                // Priority #2. Serial and MQTT Controllers can block it.
        if (activeTextSerialFlg || activeTextMqttFlg) { // Higher Priority Controllers Active;
            availFlg = false;                           // This controller is unavailable.
        }
    }

    return availFlg;
}

// ************************************************************************************************
// checkLocalControllerAvail): Determine if Local RDS Controller Mode is Enabled.
bool checkLocalControllerAvail(void) {
    bool availFlg = false;

    if (ctrlLocalFlg) {
        availFlg = true;
    }
    return availFlg;
}

// ************************************************************************************************
// checkLocalRdsAvail() Determine if at least one Local RDS is Enabled.
bool checkLocalRdsAvail(void) {
    bool availFlg = false;

    if (ctrlLocalFlg && (rdsText1EnbFlg || rdsText2EnbFlg || rdsText3EnbFlg)) {
        availFlg = true;
    }
    return availFlg;
}

// ************************************************************************************************
// checkRemoteRdsAvail): Determine if any Serial, HTTP, or MQTT controller Mode is Enabled.
bool checkRemoteRdsAvail(void) {
    bool availFlg = false;

    if (ctrlSerialFlg() || ctrlMqttFlg || ctrlHttpFlg) {
        availFlg = true;
    }
    return availFlg;
}

// ************************************************************************************************
// checkRemoteTextAvail(): Determine if Serial, HTTP, or MQTT controller has RadioText available to
// send. Ignores Local RDS.
bool checkRemoteTextAvail(void) {
    bool availFlg = false;

    if (textSerialFlg || textHttpFlg || textMqttFlg) {
        availFlg = true;
    }
    return availFlg;
}

// ************************************************************************************************
// checkControllerRdsAvail(): Determine if any (Local, HTTP, MQTT, Serial) RDS Controller is Enabled.
bool checkControllerRdsAvail(void) {
    bool availFlg = false;

    if (ctrlSerialFlg() || ctrlMqttFlg || ctrlHttpFlg || ctrlLocalFlg) {
        availFlg = true;
    }
    return availFlg;
}

// ************************************************************************************************
// processRDS(): Local RDS RadioText Display Handler for homeTab. Must be installed in main loop
//               There are three available Local RadioText Messages. Display time = rdsMsgTime.
//               A round robbin scheduler is used and user can select which messages to show.
void processRDS(void) {
    char logBuff[75 + RDS_TEXT_MAX_SZ];
    static uint8_t loop      = 0;
    int32_t currentMillis    = 0;
    static int32_t rdsMillis = 0;     // Timer for RDS Web GUI updates.
    static int32_t cntMillis = 0;     // Timer for ICStation FM Tx services.
    static String  rdsRefreshPsnStr;
    static String  rdsRefreshTextStr; // RadioText Refresh, Message to repeat.

    rdsRefreshPsnStr.reserve(10);
    rdsRefreshTextStr.reserve(70);

    currentMillis = millis();

    if (cntMillis == 0) {
        updateUiRDSTmr(0);         // Clear Display
        cntMillis = currentMillis; // Initialize First entry;
    }
    else if (testModeFlg) {
        updateTestTones(false);
        cntMillis = currentMillis;
        rdsMillis = currentMillis - rdsMsgTime + 500;         // Schedule next "normal" RadioText in 0.5Sec.
        return;
    }
    else if (currentMillis - cntMillis >= RDS_MSG_UPD_TIME) { // One Second Updates.
        cntMillis = currentMillis;

        if (!rfCarrierFlg) {
            updateUiRDSTmr(0);                            // Clear Displayed Elapsed Timer.
            displayRdsText();
            rdsMillis = currentMillis - rdsMsgTime + 500; // Schedule next RadioText in 0.5Sec.
            return;
        }
        else if (!checkLocalControllerAvail() && !checkControllerRdsAvail()) {
            updateUiRDSTmr(0);                            // Clear Displayed Elapsed Timer.
            displayRdsText();
            rdsMillis = currentMillis - rdsMsgTime + 500; // Schedule next RadioText in 0.5Sec.
            return;
        }
        else if (activeTextSerialFlg && stopSerialFlg) {
            stopSerialFlg       = false;
            activeTextSerialFlg = false;
            rdsMillis           = millis() - rdsMsgTime; // Force Countdown Timeout.
            Log.infoln("Serial Controller's RadioText has Been Stopped.");
        }
        else if (activeTextMqttFlg && stopMqttFlg) {
            stopMqttFlg       = false;
            activeTextMqttFlg = false;
            rdsMillis         = millis() - rdsMsgTime; // Force Countdown Timeout.
            Log.infoln("MQTT Controller's RadioText has Been Stopped.");
        }
        else if (activeTextHttpFlg && stopHttpFlg) {
            stopHttpFlg       = false;
            activeTextHttpFlg = false;
            rdsMillis         = millis() - rdsMsgTime; // Force Countdown Timeout.
            Log.infoln("MQTT Controller's RadioText has Been Stopped.");
        }
        else if (ctrlSerialFlg() && textSerialFlg) {   // Priority #1, New Serial RadioText.
            textSerialFlg = false;
            stopSerialFlg = false;

            activeTextSerialFlg = true;
            activeTextMqttFlg   = false;          // Clear lower priority Controller.
            activeTextHttpFlg   = false;          // Clear lower priority Controller.
            activeTextLocalFlg  = false;          // Clear lower priority Controller.

            rdsRefreshPsnStr  =  rdsSerialPsnStr; // Program Service Name.
            rdsRefreshTextStr = rdsSerialTextStr; // RadioText Message.
            rdsMsgTime        = rdsSerialMsgTime;
            rdsMillis         = currentMillis;

            radio.setPiCode(rdsSerialPiCode); // Set Serial Controller's Pi Code.

            sprintf(logBuff, "Serial Controller Sending RDS Program Service Name (%s)", rdsRefreshPsnStr.c_str());
            Log.infoln(logBuff);
            radio.sendStationName(rdsRefreshPsnStr);
            sprintf(logBuff, "Serial Controller Sending RDS RadioText (%s).", rdsRefreshTextStr.c_str());
            Log.infoln(logBuff);
            radio.sendRadioText(rdsRefreshTextStr);
            updateUiRdsText(rdsRefreshTextStr);
            displayActiveController(SERIAL_CNTRL);
        }
        else if (ctrlMqttFlg && textMqttFlg && !activeTextSerialFlg) { // Priority #2, New MQTT RadioText
            textMqttFlg = false;
            stopMqttFlg = false;

            activeTextMqttFlg  = true;
            activeTextHttpFlg  = false;         // Clear lower priority Controller.
            activeTextLocalFlg = false;         // Clear lower priority Controller.

            rdsRefreshPsnStr  = rdsMqttPsnStr;  // Program Service Name.
            rdsRefreshTextStr = rdsMqttTextStr; // RadioText Message.
            rdsMsgTime        = rdsMqttMsgTime;
            rdsMillis         = currentMillis;

            radio.setPiCode(rdsMqttPiCode); // Set MQTT Controller's Pi Code.

            sprintf(logBuff, "MQTT Controller Sending RDS Program Service Name (%s)", rdsRefreshPsnStr.c_str());
            Log.infoln(logBuff);
            radio.sendStationName(rdsRefreshPsnStr);
            sprintf(logBuff, "MQTT Controller Sending RDS RadioText (%s).", rdsRefreshTextStr.c_str());
            Log.infoln(logBuff);
            radio.sendRadioText(rdsRefreshTextStr);
            updateUiRdsText(rdsRefreshTextStr);
            displayActiveController(MQTT_CNTRL);
        }
        else if (ctrlHttpFlg && textHttpFlg && !activeTextSerialFlg && !activeTextMqttFlg) { // Priority #3, New HTTP RadioText
            textHttpFlg        = false;
            stopHttpFlg        = false;
            activeTextHttpFlg  = true;
            activeTextLocalFlg = false;         // Clear lower priority Controller.

            rdsRefreshPsnStr  =  rdsHttpPsnStr; // Program Service Name.
            rdsRefreshTextStr = rdsHttpTextStr; // RadioText Message.
            rdsMsgTime        = rdsHttpMsgTime;
            rdsMillis         = currentMillis;

            radio.setPiCode(rdsHttpPiCode); // Set HTTP Controller's Pi Code.

            sprintf(logBuff, "HTTP Controller Sending RDS Program Service Name (%s)", rdsRefreshPsnStr.c_str());
            Log.infoln(logBuff);
            radio.sendStationName(rdsRefreshPsnStr);
            sprintf(logBuff, "HTTP Controller Sending RDS RadioText (%s).", rdsRefreshTextStr.c_str());
            Log.infoln(logBuff);
            radio.sendRadioText(rdsRefreshTextStr);
            updateUiRdsText(rdsRefreshTextStr);
            displayActiveController(HTTP_CNTRL);
        }
        else if (!checkLocalRdsAvail()  && checkControllerRdsAvail() && !checkActiveTextAvail()) {
            updateUiRDSTmr(0);                             // Clear Displayed Elapsed Timer.
            displayRdsText();
            rdsMillis = currentMillis - rdsMsgTime + 500;  // Schedule next RadioText in 0.5Sec.
            return;
        }
        else if (currentMillis - rdsMillis < rdsMsgTime) { // RadioText Message Display Time has not ended yet.
            updateUiRDSTmr(rdsMillis);                     // Update Countdown time on GUI homeTab.

            if (rfCarrierFlg) {                            // When On-Air Send Program Service Name (PSN) every second.
                sprintf(logBuff, "Refreshing RDS Program Service Name: %s.", rdsRefreshPsnStr.c_str());
                Log.traceln(logBuff);
                radio.sendStationName(rdsRefreshPsnStr);

                if (((currentMillis - rdsMillis) / 1000) % RDS_REFRESH_TM == 0) { // Time to refresh RadioText.
                    if (rdsRefreshTextStr.length() && (currentMillis - rdsMillis >= RDS_MSG_UPD_TIME)) {
                        sprintf(logBuff, "Refreshing RDS RadioText Message: %s", rdsRefreshTextStr.c_str());
                        Log.traceln(logBuff);
                        radio.sendRadioText(rdsRefreshTextStr);
                    }
                }
            }
        }
    }

    if (!rfCarrierFlg) {                               // Radio Turned Off, nothing else to do. Exit.
        return;
    }
    else if (rdsMillis == 0) {                         // First Function Call, Init RDS Message Countdown Timer Time.
        rdsMillis = millis() + rdsMsgTime;
    }
    else if (currentMillis - rdsMillis < rdsMsgTime) { // Countdown still active. We're done for now, exit.
        return;
    }

    /* Countdown Now Zero. RDS RadioText Message Time has ended. */
    updateUiRDSTmr(rdsMillis); // Show "Expired" on GUI homeTab's RadioText Timer.

    /* Let's Check to see who supplied the RadioText and terminate it. */
    if (activeTextSerialFlg) { // USB Serial RDS Controller is #1 Priority.
        activeTextSerialFlg = false;
        Log.infoln("Serial Controller's RDS Time has Ended.");
    }
    else if (activeTextMqttFlg) { // MQTT RDS Controller is #2 Priority.
        activeTextMqttFlg = false;
        Log.infoln("MQTT Controller's RDS Time has Ended.");
    }
    else if (activeTextHttpFlg) { // HTTP RDS Controller is #3 Priority.
        activeTextHttpFlg = false;
        Log.infoln("HTTP Controller's RDS Time has Ended.");
    }
    else if (checkLocalRdsAvail()) { // Local RDS is Lowest Priority.
        activeTextLocalFlg = true;
        rdsMsgTime         = rdsLocalMsgTime;
        rdsRefreshPsnStr   = rdsLocalPsnStr;

        radio.setPiCode(rdsLocalPiCode); // Set Local Controller's Pi Code.

        sprintf(logBuff, "Local Controller Sending RDS Station Name (%s).", rdsLocalPsnStr.c_str());
        Log.infoln(logBuff);
        radio.sendStationName(rdsLocalPsnStr);

        sprintf(logBuff, "Local Controller Sending RDS RadioText Message");
        Log.infoln(logBuff);

        // Find Next Available Local RadioText Message.
        if ((loop == 0) && !rdsText1EnbFlg) { // RDS Message Disabled, next.
            Log.traceln("-> RDS Text Msg1 is Disabled, Skip to Next");
            loop = 1;
        }

        if ((loop == 1) && !rdsText2EnbFlg) { // RDS Message Disabled, next.
            Log.traceln("-> RDS Text Msg2 is Disabled, Skip to Next");
            loop = 2;
        }

        if ((loop == 2) && !rdsText3EnbFlg) { // This can't happen!. Reset Loop.
            Log.traceln("-> RDS Text Msg3 is Disabled, Skip to Beginning");
            loop = 0;
        }

        updateUiRdsText(" "); // Clear RDS RadioText In Web UI; Use single-space char!

        // Send the chosen RadioText.
        if (loop == 0) {
            if (rdsText1EnbFlg) {
                rdsTextMsgStr     = rdsTextMsg1Str;
                rdsRefreshTextStr = rdsTextMsgStr;
                radio.sendRadioText(rdsTextMsgStr);
                updateUiRdsText(rdsTextMsgStr);
                displayActiveController(LOCAL_CNTRL);
                rdsMillis = millis();
            }
        }
        else if (loop == 1) {
            if (rdsText2EnbFlg) {
                rdsTextMsgStr     = rdsTextMsg2Str;
                rdsRefreshTextStr = rdsTextMsgStr;
                radio.sendRadioText(rdsTextMsgStr);
                updateUiRdsText(rdsTextMsgStr);
                displayActiveController(LOCAL_CNTRL);
                rdsMillis = millis();
            }
        }
        else if (loop == 2) {
            if (rdsText3EnbFlg) {
                rdsTextMsgStr     = rdsTextMsg3Str;
                rdsRefreshTextStr = rdsTextMsgStr;
                radio.sendRadioText(rdsTextMsgStr);
                updateUiRdsText(rdsTextMsgStr);
                displayActiveController(LOCAL_CNTRL);
                rdsMillis = millis();
            }
        }
        else {
            Log.traceln("-> Will Resync Local RDS RadioText on Next Round.");
        }

        sprintf(logBuff, "-> RDS RadioText Sent \"%s\".", rdsTextMsgStr.c_str());
        Log.infoln(logBuff);

        if (++loop > 2) {
            loop = 0;
        }
    }
    else { // No available RadioText message.
        activeTextSerialFlg = false;
        activeTextMqttFlg   = false;
        activeTextHttpFlg   = false;
        activeTextLocalFlg  = false;

        loop              = 0;                            // Reset Local RadioText to first message.
        rdsRefreshTextStr = "";                           // Erase RadioText Refresh message.
        rdsMillis         = millis() - rdsMsgTime + 1000; // Schedule next RadioText in 1Sec.
        Log.warningln("-> No RDS RadioText Available, Nothing Sent.");
        displayActiveController(NO_CNTRL);
    }

    updateUiRDSTmr(rdsMillis); // Refresh Countdown time on GUI homeTab.
}

// ************************************************************************************************
// resetControllerRdsValues(): Reset the Local, Serial, MQTT, and HTTP Controllers' RDS Initial
// Runtime Values. These values can be changed by Serial/MQTT/HTTP controller commands.
// Call this function during boot, after restoreConfiguration().
void resetControllerRdsValues(void)
{
    // Local RDS Controller
    radio.setPiCode(rdsLocalPiCode);    // Local RDS PI Code is Fixed Value.

    // USB Serial RDS Controller. All values can be changed during runtime by Serial Commands.
    rdsSerialPsnStr  = rdsLocalPsnStr;  // Default Program Service Name (Mimic Local Controller).
    rdsSerialTextStr = "";              // Clear Controller's RadioText Message.
    rdsSerialPiCode  = rdsLocalPiCode;  // Default PI Code (Mimic Local Controller).
    rdsSerialMsgTime = rdsLocalMsgTime; // Default RDS Message Time (Mimic Local Controller),

    // MQTT RDS Controller. All values can be changed during runtime by MQTT Commands.
    rdsMqttPsnStr  = rdsLocalPsnStr;    // Default Program Service Name (Mimic Local Controller).
    rdsMqttTextStr = "";                // Clear Controller's RadioText Message.
    rdsMqttPiCode  = rdsLocalPiCode;    // Default PI Code (Mimic Local Controller).
    rdsMqttMsgTime = rdsLocalMsgTime;   // Default RDS Message Time (Mimic Local Controller),

    // HTTP RDS Controller. All values can be changed during runtime by HTTP Commands.
    rdsHttpPsnStr  = rdsLocalPsnStr;    // Default Program Service Name (Mimic Local Controller).
    rdsHttpTextStr = "";                // Clear Controller's RadioText Message.
    rdsHttpPiCode  = rdsLocalPiCode;    // Default PI Code (Mimic Local Controller).
    rdsHttpMsgTime = rdsLocalMsgTime;   // Default RDS Message Time (Mimic Local Controller),
}

// ************************************************************************************************
// EOF
