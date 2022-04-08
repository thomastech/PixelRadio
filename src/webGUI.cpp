/*
   File: webGUI.cpp
   Project: PixelRadio, an RBDS/RDS FM Transmitter (QN8027 Digital FM IC)
   Version: 1.0
   Creation: Dec-16-2021
   Revised:  Apr-06-2022
   Public Release:
   Project Leader: T. Black (thomastech)
   Contributors: thomastech
   Revision History: See PixelRadio.cpp

   (c) copyright T. Black 2021-2022, Licensed under GNU GPL 3.0 and later, under this
   license absolutely no warranty is given.
   This Code was formatted with the uncrustify extension.

   IMPORTANT:
   =========
   PixelRadio was originally developed to use LITTLEFS to serve the web pages. But
   there's an unresolved ESPUI library bug that causes random ESP32 crashes during
   browser page refreshes or page re-vists. Using EPUI's Embedded mode avoids this
   problem. So for now, do NOT switch to the LITTLEFS option to serve the web pages.

   ESPUI NOTES:
   ============
   NOTE 1. This project uses iangray001's patched release of ESPUI Library created
   on Feb-16-2022.
   I've found that the PR releases often introduce undesireable bugs. So I suggest
   staying with this release until there is a strong reason to try a newer version.

   NOTE 2.
   Custom Updates to the ESPUI Library:
   Any edits to the ESPUI library must be done in the Project's library folder. After
   editing, the ESPUI minified and .h files must be prepared (rebuilt) using the
   following command in platformio terminal: python .scripts/prepare_static_ui_sources.py -a
     Comment 1: It may be necessary to install the support programs (watch the error
      messages). Typically you may need to install the following (use Platformio Terminal):
      pip install csscompressor
      pip install jsmin
      pip install htmlmin
     Comment 2: The python file has been modified (src = "\PixelRadio\lib\ESPUI\data" and
     target = "\PixelRadio\lib\ESPUI\src").
     The prepared data files must be copied to this project's data folder. See below for
     more details.

   NOTE 3.
   Data Files: (Useful advice when using ESPUI's LITTLEFS)
   If LITTLEFS is used then ALL .html, .js, and .css data files must be in
   the PixelRadio/data folder.
   Here's some advice on how to get all the ESPUI data files to fit in a min_spiffs build:
   ESPUI has 50KB of unneeded files. Delete the following files (if present) from
   the /project data folder:
    data/index.min.htm
    data/css/normalize.min.css
    data/css/style.min.css
    data/js/controls.min.js
    data/js/graph.min.js
    data/js/slider.min.js
    data/js/tabbedcontent.min.js
    NOTE: Do NOT delete /data/js/zepto.min.js

    After the deletions the following files MUST remain in your data folder:
    /index.htm"
    /css/style.css
    /css/normalize.css
    /js/zepto.min.js
    /js/controls.js
    /js/slider.js
    /js/graph.js
    /js/tabbedcontent.js
    RadioLogo225x75_base64.gif

    NOTE 4.
    The browser's ESPUI interface can be redrawn by using ESPUI.jsonReload().
    However, it only redraws the currently displayed settings and ignores any programmable
    changes to system variables.

    NOTE 5.
    When new versions of ESPUI are installed please edit the dataIndexHTML.h and
    change <title>Control<title> section to <title>PixelRadio</title>
    This will report the device as "PixelRadio" on network scans.
    Note: If login is enabled then the device will report "Espressif, Inc."

 */

// ************************************************************************************************

#include <ArduinoLog.h>
#include "config.h"
#include "PixelRadio.h"
#include "globals.h"
#include "language.h"
#include "ESPUI.h"
#include "QN8027Radio.h"

// ************************************************************************************************

extern QN8027Radio radio;

// ************************************************************************************************
// Local Strings.
String tempStr;

uint16_t aboutTab  = 0;
uint16_t adjTab    = 0;
uint16_t backupTab = 0;
uint16_t ctrlTab   = 0;
uint16_t diagTab   = 0;
uint16_t gpioTab   = 0;
uint16_t homeTab   = 0;
uint16_t radioTab  = 0;
uint16_t rdsTab    = 0;
uint16_t wifiTab   = 0;

uint16_t aboutLogoID    = 0;
uint16_t aboutVersionID = 0;

uint16_t adjAvolID     = 0;
uint16_t adjFmDispID   = 0;
uint16_t adjFreqID     = 0;
uint16_t adjMuteID     = 0;
uint16_t adjSaveID     = 0;
uint16_t adjSaveMsgID  = 0;
uint16_t radioSoundID  = 0;
uint16_t adjTestModeID = 0;
uint16_t adjUvolID     = 0;

uint16_t backupRestoreID    = 0;
uint16_t backupRestoreMsgID = 0;
uint16_t backupSaveID       = 0;
uint16_t backupSaveMsgID    = 0;
uint16_t backupSaveSetID    = 0;
uint16_t backupSaveSetMsgID = 0;

uint16_t ctrlHttpID      = 0;
uint16_t ctrlLocalID     = 0;
uint16_t ctrlMqttID      = 0;
uint16_t ctrlMqttIpID    = 0;
uint16_t ctrlMqttMsgID   = 0;
uint16_t ctrlMqttNameID  = 0;
uint16_t ctrlMqttPortID  = 0;
uint16_t ctrlMqttUserID  = 0;
uint16_t ctrlMqttPwID    = 0;
uint16_t ctrlSerialID    = 0;
uint16_t ctrlSerialMsgID = 0;
uint16_t ctrlSaveID      = 0;
uint16_t ctrlSaveMsgID   = 0;

uint16_t diagBootID    = 0;
uint16_t diagBootMsgID = 0;
uint16_t diagLogID     = 0;
uint16_t diagLogMsgID  = 0;
uint16_t diagMemoryID  = 0;
uint16_t diagTimerID   = 0;
uint16_t diagVbatID    = 0;
uint16_t diagVdcID     = 0;

uint16_t gpio19ID      = 0;
uint16_t gpio23ID      = 0;
uint16_t gpio33ID      = 0;
uint16_t gpio19MsgID   = 0;
uint16_t gpio23MsgID   = 0;
uint16_t gpio33MsgID   = 0;
uint16_t gpioSaveID    = 0;
uint16_t gpioSaveMsgID = 0;

uint16_t homeFreqID    = 0;
uint16_t homeOnAirID   = 0;
uint16_t homeRdsTextID = 0;
uint16_t homeRdsTmrID  = 0;
uint16_t homeRssiID    = 0;
uint16_t homeStaID     = 0;
uint16_t homeStaMsgID  = 0;
uint16_t homeTextMsgID = 0;

uint16_t radioAudioID    = 0;
uint16_t radioAudioMsgID = 0;
uint16_t radioVgaGainID  = 0;
uint16_t radioDgainID    = 0;
uint16_t radioAutoID     = 0;
uint16_t radioEmphID     = 0;
uint16_t radioFreqID     = 0;
uint16_t radioGainID     = 0;
uint16_t radioImpID      = 0;
uint16_t radioPwrID      = 0;
uint16_t radioRfEnbID    = 0;
uint16_t radioSaveID     = 0;
uint16_t radioSaveMsgID  = 0;

uint16_t rdsDspTmID    = 0;
uint16_t rdsEnbID      = 0;
uint16_t rdsEnb1ID     = 0;
uint16_t rdsEnb2ID     = 0;
uint16_t rdsEnb3ID     = 0;
uint16_t rdsPiID       = 0;
uint16_t rdsPiMsgID    = 0;
uint16_t rdsProgNameID = 0;
uint16_t rdsSaveID     = 0;
uint16_t rdsSaveMsgID  = 0;
uint16_t rdsSnameID    = 0;
uint16_t rdsRstID      = 0;
uint16_t rdsText1ID    = 0;
uint16_t rdsText2ID    = 0;
uint16_t rdsText3ID    = 0;
uint16_t rdsText1MsgID = 0;
uint16_t rdsText2MsgID = 0;
uint16_t rdsText3MsgID = 0;

uint16_t wifiApBootID     = 0;
uint16_t wifiApFallID     = 0;
uint16_t wifiApID         = 0;
uint16_t wifiApIpID       = 0;
uint16_t wifiApNameID     = 0;
uint16_t wifiDevPwMsgID   = 0;
uint16_t wifiDevUserID    = 0;
uint16_t wifiDevUserMsgID = 0;
uint16_t wifiDevPwID      = 0;
uint16_t wifiDhcpID       = 0;
uint16_t wifiDhcpMsgID    = 0;
uint16_t wifiDnsID        = 0;
uint16_t wifiGatewayID    = 0;
uint16_t wifiIpID         = 0;
uint16_t wifiMdnsNameID   = 0;
uint16_t wifiNetID        = 0;
uint16_t wifiRssiID       = 0;
uint16_t wifiSaveID       = 0;
uint16_t wifiSaveMsgID    = 0;
uint16_t wifiSsidID       = 0;
uint16_t wifiStaID        = 0;
uint16_t wifiStaMsgID     = 0;
uint16_t wifiStaNameID    = 0;
uint16_t wifiSubID        = 0;
uint16_t wifiWpaKeyID     = 0;


// ************************************************************************************************
// applyCustomCss(): Apply custom CSS to Web GUI controls at the start of runtime.
//                   It is called AFTER ESPUI.begin(), see bottom of startGUI().
//                   Note: width and max-width appear to do the same thing. But try both. Avoid widths <30% or expect text wrap.
void initCustomCss(void)
{
    // START OF PANEL INLINE STYLES
    ESPUI.setPanelStyle(aboutLogoID,    "background-color: white; color: black;");

    ESPUI.setPanelStyle(adjFmDispID,    "font-size: 3.0em;");

    ESPUI.setPanelStyle(ctrlMqttPortID, "font-size: 1.25em;");

    ESPUI.setPanelStyle(diagBootID,     "color: black;");
    ESPUI.setPanelStyle(diagLogID,      "color: black;");
    ESPUI.setPanelStyle(diagMemoryID,   "color: black; font-size: 1.25em;");
    ESPUI.setPanelStyle(diagTimerID,    "color: black; font-size: 1.25em;");
    ESPUI.setPanelStyle(diagVbatID,     "color: black; font-size: 1.25em;");
    ESPUI.setPanelStyle(diagVdcID,      "color: black; font-size: 1.25em;");

    ESPUI.setPanelStyle(homeFreqID,     "font-size: 3.0em;");
    ESPUI.setPanelStyle(homeOnAirID,    "font-size: 3.0em;");
    ESPUI.setPanelStyle(homeRdsTextID,  "font-size: 1.15em;");
    ESPUI.setPanelStyle(homeRssiID,     "font-size: 1.25em;");
    ESPUI.setPanelStyle(homeRdsTmrID,   "font-size: 1.25em;");
    ESPUI.setPanelStyle(homeStaID,      "font-size: 1.15em;");
    ESPUI.setPanelStyle(homeTextMsgID,  "font-size: 1.15em;");

    ESPUI.setPanelStyle(radioFreqID,    "font-size: 3.0em;");
    ESPUI.setPanelStyle(radioGainID,    "font-size: 1.15em;");
    ESPUI.setPanelStyle(radioSoundID,   "font-size: 1.25em;");

    ESPUI.setPanelStyle(rdsProgNameID,  "font-size: 1.35em;");
    ESPUI.setPanelStyle(rdsPiID,        "font-size: 1.35em;");

    // ESPUI.setPanelStyle(rdsProgNameID, "width: 45%; font-size: 1.5em;"); // Bug? See
    //  https://github.com/s00500/ESPUI/pull/147#issuecomment-1009821269.
    // ESPUI.setPanelStyle(rdsPiID,"width: 30%; font-size: 1.25em;"); // Only valid if wide panel is used.
    //  ESPUI.setPanelStyle(rdsPiID,        "max-width: 40%;"); // Does not work. Too bad.

    ESPUI.setPanelStyle(wifiRssiID, "font-size: 1.25em;");
    ESPUI.setPanelStyle(wifiStaID,  "font-size: 1.15em;");

    // END OF PANEL INLINE STYLES.

    // START OF ELEMENT INLINE STYLES

    ESPUI.setElementStyle(aboutVersionID,     "background-color: white; color: black; margin-top: 0px;");

    ESPUI.setElementStyle(adjFmDispID,        "max-width: 75%;");
    ESPUI.setElementStyle(adjMuteID,          (muteFlg ? "background: red;" : "background: #bebebe;"));
    ESPUI.setElementStyle(adjSaveMsgID,       CSS_LABEL_STYLE_RED);

    ESPUI.setElementStyle(backupRestoreMsgID, CSS_LABEL_STYLE_WHITE);
    ESPUI.setElementStyle(backupSaveMsgID,    CSS_LABEL_STYLE_WHITE);
    ESPUI.setElementStyle(backupSaveSetMsgID, CSS_LABEL_STYLE_RED);

    //  ESPUI.setElementStyle(ctrlSerialID,   "max-width: 40%;"); // Does Not Work.
    ESPUI.setElementStyle(ctrlMqttPortID,     "max-width: 40%;");
    ESPUI.setElementStyle(ctrlMqttMsgID,      CSS_LABEL_STYLE_BLACK);
    ESPUI.setElementStyle(ctrlSerialMsgID,    CSS_LABEL_STYLE_BLACK);
    ESPUI.setElementStyle(ctrlSaveMsgID,      CSS_LABEL_STYLE_RED);

    ESPUI.setElementStyle(diagBootMsgID,      CSS_LABEL_STYLE_BLACK);
    ESPUI.setElementStyle(diagMemoryID,       "max-width: 40%;");
    ESPUI.setElementStyle(diagLogMsgID,       CSS_LABEL_STYLE_BLACK);
    ESPUI.setElementStyle(diagTimerID,        "max-width: 50%;");
    ESPUI.setElementStyle(diagVbatID,         "max-width: 30%;");
    ESPUI.setElementStyle(diagVdcID,          "max-width: 30%;");

    ESPUI.setElementStyle(gpio19MsgID,        CSS_LABEL_STYLE_WHITE);
    ESPUI.setElementStyle(gpio23MsgID,        CSS_LABEL_STYLE_WHITE);
    ESPUI.setElementStyle(gpio33MsgID,        CSS_LABEL_STYLE_WHITE);
    ESPUI.setElementStyle(gpioSaveMsgID,      CSS_LABEL_STYLE_RED);

    ESPUI.setElementStyle(homeFreqID,         "max-width: 80%;");
    ESPUI.setElementStyle(homeOnAirID,        "max-width: 80%;");
    ESPUI.setElementStyle(homeRssiID,         "max-width: 30%;");
    ESPUI.setElementStyle(homeRdsTmrID,       "max-width: 30%;");
    ESPUI.setElementStyle(homeStaID,          "max-width: 65%;");
    ESPUI.setElementStyle(homeStaMsgID,       CSS_LABEL_STYLE_BLACK);
    ESPUI.setElementStyle(homeTextMsgID,      CSS_LABEL_STYLE_WHITE);

    // ESPUI.setElementStyle(homeLogoID,       "max-width: 45%; background-color: white; color: black;"); // DOES NOT WORK.

    ESPUI.setElementStyle(radioFreqID,      "width: 75%;");
    ESPUI.setElementStyle(radioGainID,      "width: 35%;");
    ESPUI.setElementStyle(radioAudioMsgID,  CSS_LABEL_STYLE_BLACK);
    ESPUI.setElementStyle(radioSaveMsgID,   CSS_LABEL_STYLE_RED);
    ESPUI.setElementStyle(radioSoundID,     "max-width: 35%;");

    // ESPUI.setElementStyle(rdsPiID,          "font-size: 1.25em;");
    ESPUI.setElementStyle(rdsPiMsgID,       "font-size: 0.8em; background-color: unset; color: black;");

    // ESPUI.setElementStyle(rdsProgNameID,    "color: black;");
    ESPUI.setElementStyle(rdsSaveMsgID,     CSS_LABEL_STYLE_BLACK);
    ESPUI.setElementStyle(rdsText1MsgID,    CSS_LABEL_STYLE_BLACK);
    ESPUI.setElementStyle(rdsText2MsgID,    CSS_LABEL_STYLE_BLACK);
    ESPUI.setElementStyle(rdsText3MsgID,    CSS_LABEL_STYLE_BLACK);

    ESPUI.setElementStyle(wifiDhcpMsgID,    CSS_LABEL_STYLE_BLACK);
    ESPUI.setElementStyle(wifiDevUserMsgID, CSS_LABEL_STYLE_MAROON);
    ESPUI.setElementStyle(wifiDevPwMsgID,   CSS_LABEL_STYLE_MAROON);
    ESPUI.setElementStyle(wifiRssiID,       "max-width: 30%;");
    ESPUI.setElementStyle(wifiSaveMsgID,    CSS_LABEL_STYLE_MAROON);
    ESPUI.setElementStyle(wifiStaID,        "max-width: 65%;");
    ESPUI.setElementStyle(wifiStaMsgID,     CSS_LABEL_STYLE_BLACK);

    if (rdsText1EnbFlg) {
        ESPUI.setEnabled(rdsText1ID, false); // Disable RadioText 1 Message Entry.
        ESPUI.print(rdsText1MsgID, RDS_LOCKED_STR);
    }

    if (rdsText2EnbFlg) {
        ESPUI.setEnabled(rdsText2ID, false); // Disable RadioText 2 Message Entry.
        ESPUI.print(rdsText2MsgID, RDS_LOCKED_STR);
    }

    if (rdsText3EnbFlg) {
        ESPUI.setEnabled(rdsText3ID, false); // Disable RadioText 3 Message Entry.
        ESPUI.print(rdsText3MsgID, RDS_LOCKED_STR);
    }

    // END OF STYLES
}

// ************************************************************************************************
// getAudioGain(): Compute the Audio Gain.
// Radio Audio Gain in dB = ((Analog Input Gain Step + 1) * 3) - (Input Impedance Step * 3)
// Please note that the formula has been modified based on real-world measurements.
// The official formula uses 6dB Impedance steps vs the revised use of 3dB.
int8_t getAudioGain(void)
{
    int8_t vgaGain;
    int8_t impedance;
    int8_t audioGain;

    if (vgaGainStr == VGA_GAIN0_STR) {
        vgaGain = 0;
    }
    else if (vgaGainStr == VGA_GAIN1_STR) {
        vgaGain = 1;
    }
    else if (vgaGainStr == VGA_GAIN2_STR) {
        vgaGain = 2;
    }
    else if (vgaGainStr == VGA_GAIN3_STR) {
        vgaGain = 3;
    }
    else if (vgaGainStr == VGA_GAIN4_STR) {
        vgaGain = 4;
    }
    else if (vgaGainStr == VGA_GAIN5_STR) {
        vgaGain = 5;
    }
    else {
        vgaGain = 0;
        Log.errorln("displayAudioGain: Undefined vgaGainStr!");
    }

    if (inpImpedStr == INP_IMP05K_STR) {
        impedance = 0;
    }
    else if (inpImpedStr == INP_IMP10K_STR) {
        impedance = 1;
    }
    else if (inpImpedStr == INP_IMP20K_STR) {
        impedance = 2;
    }
    else if (inpImpedStr == INP_IMP40K_STR) {
        impedance = 3;
    }
    else {
        impedance = 0;
        Log.errorln("displayAudioGain: Undefined inpImpedStr!");
    }

    audioGain = ((vgaGain + 1) * 3) - (impedance * 3);

    return audioGain;
}

// ************************************************************************************************
// displayActiveController(): Display the currently active RadioText Controller.
void displayActiveController(uint8_t controller)
{
    if (controller == SERIAL_CNTRL) {
        ESPUI.print(homeTextMsgID, "Source: Serial Controller");
    }
    else if (controller == MQTT_CNTRL) {
        ESPUI.print(homeTextMsgID, "Source: MQTT Controller");
    }
    else if (controller == HTTP_CNTRL) {
        ESPUI.print(homeTextMsgID, "Source: HTTP Controller");
    }
    else if (controller == LOCAL_CNTRL) {
        ESPUI.print(homeTextMsgID, "Source: Local Controller");
    }
    else if (controller == NO_CNTRL) {
        ESPUI.print(homeTextMsgID, " ");
    }
    else {
        ESPUI.print(homeTextMsgID, " ");
    }
}

// ************************************************************************************************
// displayRdsText(): Check if any controllers are enabled and updated RDS with text or status message.
void displayRdsText(void)
{
    char logBuff[RDS_TEXT_MAX_SZ + 35];

    if (!rfCarrierFlg) {
        updateUiRdsText(RDS_RF_DISABLED_STR);
        sprintf(logBuff, "displayRdsText: Warning %s.", RDS_RF_DISABLED_STR);
        Log.warningln(logBuff);
    }
    else if (testModeFlg) {
        return;
    }
    else if (!checkControllerRdsAvail()) { // No controllers are available. All off.
        updateUiRdsText(RDS_CTRLS_DIS_STR);
        sprintf(logBuff, "displayRdsText: Warning %s.", RDS_CTRLS_DIS_STR);
        Log.warningln(logBuff);
    }
    else if (checkRemoteRdsAvail() && checkRemoteTextAvail()) { // Show Remote RadioText.
        updateUiRdsText(rdsTextMsgStr);
        sprintf(logBuff, "Remote RadioText: %s.", rdsTextMsgStr.c_str());
        Log.traceln(logBuff);
    }
    else if (!checkRemoteRdsAvail() && !checkLocalRdsAvail()) {
        updateUiRdsText(RDS_LOCAL_DIS_STR);
        sprintf(logBuff, "displayRdsText: %s.", RDS_LOCAL_DIS_STR);
        Log.traceln(logBuff);
    }
    else if (checkLocalRdsAvail() && (rdsTextMsgStr.length() > 0)) { // Show Local RadioText.
        updateUiRdsText(rdsTextMsgStr);
        sprintf(logBuff, "Local RadioText: %s.", rdsTextMsgStr.c_str());
        Log.traceln(logBuff);
    }
    else if (checkLocalRdsAvail()) { // local RadioText is blank.
        updateUiRdsText(RDS_LOCAL_BLANK_STR);
        sprintf(logBuff, "Local RadioText: Warning %s.", RDS_LOCAL_BLANK_STR);
        Log.warningln(logBuff);
    }
    else {
        updateUiRdsText(RDS_WAITING_STR);
        Log.verboseln("Local RadioText is Disabled.");
    }
}

// ************************************************************************************************
// displaySaveWarning(): Show the "Save Required" Message on all configuration pages.
void displaySaveWarning(void)
{
    ESPUI.print(adjSaveMsgID,       SAVE_SETTINGS_MSG_STR);
    ESPUI.print(backupSaveSetMsgID, SAVE_SETTINGS_MSG_STR);
    ESPUI.print(ctrlSaveMsgID,      SAVE_SETTINGS_MSG_STR);
    ESPUI.print(gpioSaveMsgID,      SAVE_SETTINGS_MSG_STR);
    ESPUI.print(radioSaveMsgID,     SAVE_SETTINGS_MSG_STR);
    ESPUI.print(rdsSaveMsgID,       SAVE_SETTINGS_MSG_STR);
    ESPUI.print(wifiSaveMsgID,      SAVE_SETTINGS_MSG_STR);

}

// ************************************************************************************************
// startGUI(): Must be called once in startup, AFTER wifiConnect()
void startGUI(void)
{
    buildGUI();

    ESPUI.jsonUpdateDocumentSize  = 2000;                              // Default is 2000.
    ESPUI.jsonInitialDocumentSize = 8000;                              // Default is 8000.

    //ESPUI.setVerbosity(Verbosity::VerboseJSON);                        // Debug mode.
    ESPUI.setVerbosity(Verbosity::Quiet);                              // Production mode.

    if ((userNameStr.length() == 0) ||  (userPassStr.length() == 0)) { // Missing credentials, use automatic login.
        ESPUI.begin("PixelRadio");

        // Don't use LITTLEFS, browser refreshes will crash.
        // ESPUI.beginLITTLEFS(APP_NAME_STR);
    }
    else {
        ESPUI.begin("PixelRadio", userNameStr.c_str(), userPassStr.c_str());

        // Don't use LITLEFS, browser refreshes will crash.
        // ESPUI.beginLITTLEFS(APP_NAME_STR, userNameStr.c_str(), userPassStr.c_str());
    }

    initCustomCss();
}

// ************************************************************************************************
// updateUiAudioLevel(): Update the diagTab UI's Audio Level (mV).
void updateUiAudioLevel(void)
{
    uint16_t mV;
    static long previousMillis = 0;
    char logBuff[60];

    if (previousMillis == 0) {
        previousMillis = millis(); // Initialize First entry;
    }
    else if (millis() - previousMillis >= AUDIO_MEAS_TIME) {
        previousMillis = millis();
        mV             = measureAudioLevel();

        if (mV >= AUDIO_LEVEL_MAX) {
            tempStr = ">";
        }
        else {
            tempStr = "";
        }
        tempStr += String(mV);
        tempStr += "mV";
        ESPUI.print(radioSoundID, tempStr);
        sprintf(logBuff, "Peak Audio Amplitude %03umV.", mV);
        Log.verboseln(logBuff);
    }
}

// ************************************************************************************************
// updateUiAudioMode(): Update the Stereo/Mono Audio Mode shown on the UI radioTab.
void updateUiAudioMode(void)
{
    ESPUI.print(radioAudioMsgID, stereoEnbFlg ? RADIO_STEREO_STR : RADIO_MONO_STR);
    ESPUI.updateControlValue(radioAudioID, stereoEnbFlg ? "1" : "0");
}

// ************************************************************************************************
// updateUiAudioMute(): Update the Audio Mute Switch Position on the UI adjTab.
void updateUiAudioMute(void)
{
    ESPUI.setElementStyle(adjMuteID, muteFlg ? "background: red;" : "background: #bebebe;");
    ESPUI.updateControlValue(adjMuteID, muteFlg ? "1" : "0");
}

// ************************************************************************************************
// updateUiGpioMsg(): Update the GPIO Boot Control's Message Label Element.
bool updateUiGpioMsg(uint8_t pin, uint8_t controller) {
    char gpioBuff[50];
    char setBuff[30];
    uint16_t msgID;
    String controllerStr;

    controllerStr = getControllerName(controller);
    controllerStr.toUpperCase();

    if (controllerStr.length() == 0) {
        Log.errorln("-> updateUiGpioMsg: Undefined Controller!");
        return false;
    }

    if (pin == GPIO19_PIN) {
        msgID = gpio19MsgID;
        if (gpio19CtrlStr == CMD_GPIO_OUT_HIGH_STR) {
            sprintf(setBuff, "%s", GPIO_OUT_HI_STR);
        }
        else if (gpio19CtrlStr == CMD_GPIO_OUT_LOW_STR) {
            sprintf(setBuff, "%s", GPIO_OUT_LO_STR);
        }
        else {
            Log.errorln("-> updateUiGpioMsg: Undefined GPIO19 State!");
            return false;
        }
    }
    else if (pin == GPIO23_PIN) {
        msgID = gpio23MsgID;
        if (gpio23CtrlStr == CMD_GPIO_OUT_HIGH_STR) {
            sprintf(setBuff, "%s", GPIO_OUT_HI_STR);
        }
        else if (gpio23CtrlStr == CMD_GPIO_OUT_LOW_STR) {
            sprintf(setBuff, "%s", GPIO_OUT_LO_STR);
        }
        else {
            Log.errorln("-> updateUiGpioMsg: Undefined GPIO23 State!");
            return false;
        }
    }
    else if (pin == GPIO33_PIN) {
        msgID = gpio33MsgID;
        if (gpio33CtrlStr == CMD_GPIO_OUT_HIGH_STR) {
            sprintf(setBuff, "%s", GPIO_OUT_HI_STR);
        }
        else if (gpio33CtrlStr == CMD_GPIO_OUT_LOW_STR) {
            sprintf(setBuff, "%s", GPIO_OUT_LO_STR);
        }
        else {
            Log.errorln("-> updateUiGpioMsg: Undefined GPIO33 State!");
            return false;
        }
    }
    else {
        Log.errorln("-> updateUiGpioMsg: Undefined GPIO Pin.");
        return false;

    }

    sprintf(gpioBuff, "{ SET TO %s BY %s }", setBuff, controllerStr.c_str());
    ESPUI.print(msgID, gpioBuff);

    return true;
}


// ************************************************************************************************
// updateUiIpaddress(): Update the IP address shown on homeTab and wifiTab.
void updateUiIpaddress(String ipStr)
{
    ESPUI.print(homeStaMsgID, ipStr);
    ESPUI.print(wifiStaMsgID, ipStr);
}

// ************************************************************************************************
// updateUiFreeMemory(): Update the Free Heap Memory on the diagTab.
void updateUiFreeMemory(void)
{
    char logBuff[40];
    static uint32_t oldMillis = millis();

    if (millis() > oldMillis + FREE_MEM_UPD_TIME) {
        oldMillis = millis();
        tempStr   = ESP.getFreeHeap();
        tempStr  += " Bytes";
        ESPUI.print(diagMemoryID, tempStr);
        sprintf(logBuff, "Free Heap Memory: %s.", tempStr.c_str());
        Log.verboseln(logBuff);
    }
}

// ************************************************************************************************
// updateUiFrequency(): Update the FM Transmit Frequency on the UI's adjTab, homeTab, and radioTab.
void updateUiFrequency(void)
{
    float tempFloat = float(fmFreqX10) / 10.0f;

    tempStr  = String(tempFloat, 1);
    tempStr += UNITS_MHZ_STR;

    ESPUI.print(adjFmDispID, tempStr);
    ESPUI.print(homeFreqID,  tempStr);
    ESPUI.print(radioFreqID, tempStr);
}

// ************************************************************************************************
// updateUiMqttMsg(): Update the MQTT CONTROL panel's status message on the ctrlTab.
void updateUiMqttMsg(String msgStr)
{
    ESPUI.print(ctrlMqttMsgID, msgStr);
}

// ************************************************************************************************
// updateUiRdsText(): Update the currently playing RDS RadioText shown in the Home tab's RDS text element.
//                    This is a companion function to displayRdsText(). May be used standalone too.
void updateUiRdsText(String textStr)
{
    char logBuff[RDS_TEXT_MAX_SZ + 40];

    if (!rfCarrierFlg) {
        textStr = RDS_RF_DISABLED_STR;
    }
    else if (textStr.length() == 0) { // Blank RadioText Message.
        if (checkRemoteRdsAvail() && checkRemoteTextAvail()) {
            textStr = RDS_REMOTE_BLANK_STR;
        }
        else if (!checkRemoteRdsAvail() && checkLocalControllerAvail()) { // Blank Local RadioText.
            textStr = RDS_LOCAL_BLANK_STR;
        }
        else {                                                            // Could be missing local or Remote RDS Text.
            textStr = RDS_BLANK_STR;
        }
        sprintf(logBuff, "Timer Updated RadioText: %s.", textStr.c_str());
        Log.infoln(logBuff);
    }

    ESPUI.print(homeRdsTextID, textStr); // Update homeTab RDS Message Panel.
}

// ************************************************************************************************
// updateUiRDSTmr(): Updates the GUI's RDS time on homeTab.
//                    On Entry rdsMillis = snapshot time (for countdown calc). Or pass zero to "Disable" display.
void updateUiRDSTmr(int32_t rdsMillis)
{
    long timeCnt = 0;

    if (testModeFlg) {
        ESPUI.print(homeRdsTmrID, " ");
    }
    else if (rfCarrierFlg && checkControllerRdsAvail() && (checkLocalRdsAvail() || checkActiveTextAvail())) {
        timeCnt =  millis() - rdsMillis; // Get Elasped time.
        timeCnt = rdsMsgTime - timeCnt;  // Now we have Countdown time.
        timeCnt = timeCnt / 1000;        // Coverted to Secs.

        if (timeCnt >= 0) {
            tempStr  = timeCnt;
            tempStr += " Secs";
            ESPUI.print(homeRdsTmrID, tempStr);
        }
        else {
            ESPUI.print(homeRdsTmrID, RDS_EXPIRED_STR);
        }
    }
    else if (!checkControllerRdsAvail()) {
        ESPUI.print(homeRdsTmrID, RDS_DISABLED_STR);
    }
    else {
        ESPUI.print(homeRdsTmrID, RDS_EXPIRED_STR);
    }
}

// ************************************************************************************************
// updateUiRSSI(): Update the UI's RSSI label element with dBm value.
//               Note: AP Mode doesn't receive a RSSI sgnal from a router, so it returns a special message.
void updateUiRSSI(void)
{
    static long previousMillis = 0;
    char logBuff[60];

    if (previousMillis == 0) {
        previousMillis = millis();       // Initialize First entry;
    }
    else if (millis() - previousMillis >= RSSI_UPD_TIME) {
        if (getWifiMode() == WIFI_STA) { // Serial log only if STA mode.
            tempStr  = getRSSI();
            tempStr += UNITS_DBM_STR;
            sprintf(logBuff, "WiFi RSSI: %d%s.", getRSSI(), UNITS_DBM_STR);
            Log.traceln(logBuff);
        }
        else {
            tempStr = "- N/A -";
        }
        ESPUI.print(wifiRssiID, tempStr);
        ESPUI.print(homeRssiID, tempStr);
        previousMillis = millis(); // Do this last.
    }
}

// ************************************************************************************************
// updateUiLocalPiCode() Update the Local PI Code on the rdsTab.
void updateUiLocalPiCode(void)
{
    char piBuff[20];

    sprintf(piBuff, "%s :0x%04X", RDS_PI_CODE_STR, rdsLocalPiCode);
    ESPUI.print(rdsPiMsgID, piBuff);
}

// *********************************************************************************************
// updateUiTimer(): Update Elapsed Time on diagTab Page. Show Days + HH:MM:SS.
void updateUiTimer(void)
{
    char timeBuff[25];
    uint8_t  seconds;
    uint8_t  minutes;
    uint8_t  hours;
    int16_t  days;
    uint32_t secs;
    uint32_t clockMillis        = millis();
    static uint32_t timerMillis = millis();

    if (millis() >= timerMillis + ELAPSED_TMR_TIME) {
        timerMillis = millis();
        secs        = clockMillis / MSECS_PER_SEC;
        days        = elapsedDays(secs);
        hours       = numberOfHours(secs);
        minutes     = numberOfMinutes(secs);
        seconds     = numberOfSeconds(secs);
        sprintf(timeBuff, "Days:%d + %02u:%02u:%02u", days, hours, minutes, seconds);
        ESPUI.print(diagTimerID, timeBuff);
    }
}

// ************************************************************************************************
void updateUiVolts(void)
{
    static long previousMillis = 0;
    char logBuff[60];

    if (previousMillis == 0) {
        previousMillis = millis(); // Initialize First entry;
    }
    else if (millis() - previousMillis >= VOLTS_UPD_TIME) {
        previousMillis = millis();
        tempStr        = String(vbatVolts, 1);
        tempStr       += " VDC";
        ESPUI.print(diagVbatID, tempStr);
        sprintf(logBuff, "Health Check, System Voltage: %01.1f VDC.", vbatVolts);
        Log.verboseln(logBuff);

        tempStr  = String(paVolts, 1);
        tempStr += " VDC";
        ESPUI.print(diagVdcID, tempStr);
        sprintf(logBuff, "Health Check, RF AMP Voltage: %01.1f VDC.", paVolts);
        Log.verboseln(logBuff);
    }
}

// ************************************************************************************************
// updateUiWfiMode(): Update the UI's WiFi Mode.
void updateUiWfiMode(void)
{
    tempStr = String(getWifiModeStr());
    ESPUI.print(homeStaID, tempStr);
    ESPUI.print(wifiStaID, tempStr);
}

// ************************************************************************************************
// buildGUI(): Create the Web GUI. Must call this
//    Enable the following option if you want sliders to be continuous (update during move) and not discrete (update on
// stop):
//    ESPUI.sliderContinuous = true; // Beware, this will spam the webserver with a lot of messages!
//
// Example of image inside label control.
// ESPUI.addControl(ControlType::Label, "IMAGE", (makeWebGif("/RadioLogo100x45_base64.gif", 100, 45, "white")),ControlColor::Peterriver);
// Save this example!
//
void buildGUI(void)
{
    tempStr.reserve(125); // Avoid memory re-allocation fragments on the Global String.
    float tempFloat;

    // ************
    // Menu Tabs
    homeTab   = ESPUI.addControl(ControlType::Tab, "HOME", HOME_TAB_STR);
    adjTab    = ESPUI.addControl(ControlType::Tab, "ADJ", ADJUST_TAB_STR);
    radioTab  = ESPUI.addControl(ControlType::Tab, "RADIO", RADIO_TAB_STR);
    rdsTab    = ESPUI.addControl(ControlType::Tab, "RDS", RDS_TAB_STR);
    wifiTab   = ESPUI.addControl(ControlType::Tab, "WIFI", WIFI_TAB_STR);
    ctrlTab   = ESPUI.addControl(ControlType::Tab, "CNTRL", CTRL_TAB_STR);
    gpioTab   = ESPUI.addControl(ControlType::Tab, "GPIO", GPIO_TAB_STR);
    backupTab = ESPUI.addControl(ControlType::Tab, "BACKUP", BACKUP_TAB_STR);
    diagTab   = ESPUI.addControl(ControlType::Tab, "DIAG", DIAG_TAB_STR);
    aboutTab  = ESPUI.addControl(ControlType::Tab, "ABOUT", ABOUT_TAB_STR);

    // ************
    // Home Tab
    ESPUI.addControl(ControlType::Separator, HOME_FM_SEP_STR, "", ControlColor::None, homeTab);

    if (fmRadioTestCode == FM_TEST_FAIL) {
        tempStr = RADIO_FAIL_STR;
    }
    else if (fmRadioTestCode == FM_TEST_VSWR) {
        tempStr = RADIO_VSWR_STR;
    }
    else if (paVolts < PA_VOLT_MIN || paVolts > PA_VOLT_MAX) {
        tempStr = RADIO_VOLT_STR;
    }
    else {
        tempStr = rfCarrierFlg ? RADIO_ON_AIR_STR : RADIO_OFF_AIR_STR;
    }
    homeOnAirID = ESPUI.addControl(ControlType::Label, HOME_RAD_STAT_STR, tempStr, ControlColor::Peterriver, homeTab);

    tempFloat  = float(fmFreqX10) / 10.0f;
    tempStr    = String(tempFloat, 1);
    tempStr   += UNITS_MHZ_STR;
    homeFreqID = ESPUI.addControl(ControlType::Label,
                                  HOME_FREQ_STR,
                                  tempStr,
                                  ControlColor::Peterriver,
                                  homeTab);

    ESPUI.addControl(ControlType::Separator, HOME_SEP_RDS_STR, "", ControlColor::None, homeTab);
    tempStr       = HOME_RDS_WAIT_STR;
    homeRdsTextID = ESPUI.addControl(ControlType::Label, HOME_CUR_TEXT_STR, tempStr, ControlColor::Peterriver, homeTab);
    homeTextMsgID = ESPUI.addControl(ControlType::Label, "", tempStr, ControlColor::Peterriver, homeRdsTextID);

    homeRdsTmrID = ESPUI.addControl(ControlType::Label, HOME_RDS_TIMER_STR, tempStr, ControlColor::Peterriver, homeTab);

    ESPUI.addControl(ControlType::Separator, HOME_SEP_WIFI_STR, "", ControlColor::None, homeTab);
    tempStr    = getRSSI();
    tempStr   += UNITS_DBM_STR;
    homeRssiID = ESPUI.addControl(ControlType::Label, HOME_WIFI_STR, tempStr, ControlColor::Peterriver, homeTab);

    homeStaID = ESPUI.addControl(ControlType::Label,
                                 HOME_STA_STR,
                                 String(getWifiModeStr()),
                                 ControlColor::Peterriver,
                                 homeTab);
    homeStaMsgID =
        ESPUI.addControl(ControlType::Label, "IP_ADDR", ipAddrStr, ControlColor::Carrot, homeStaID);

    // **************
    // Ajust Tab

    ESPUI.addControl(ControlType::Separator, ADJUST_FRQ_CTRL_STR, "", ControlColor::None, adjTab);

    tempFloat   = float(fmFreqX10) / 10.0f;
    tempStr     = String(tempFloat, 1);
    tempStr    += UNITS_MHZ_STR;
    adjFmDispID = ESPUI.addControl(ControlType::Label, ADJUST_FM_FRQ_STR, tempStr, ControlColor::Wetasphalt, adjTab);

    adjFreqID = ESPUI.addControl(ControlType::Pad,
                                 ADJUST_FRQ_ADJ_STR,
                                 "",
                                 ControlColor::Wetasphalt,
                                 adjTab,
                                 &adjFmFreqCallback);

    ESPUI.addControl(ControlType::Separator, ADJUST_AUDIO_SEP_STR, "", ControlColor::None, adjTab);

    adjTestModeID = ESPUI.addControl(ControlType::Switcher,
                                     ADJUST_TEST_STR,
                                     testModeFlg ? "1" : "0",
                                     ControlColor::Wetasphalt,
                                     adjTab,
                                     &testModeCallback);

    adjMuteID = ESPUI.addControl(ControlType::Switcher,
                                 ADJUST_MUTE_STR,
                                 muteFlg ? "1" : "0",
                                 ControlColor::Wetasphalt,
                                 adjTab,
                                 &muteCallback);


    ESPUI.addControl(ControlType::Separator, SAVE_SETTINGS_STR, "", ControlColor::None, adjTab);
    adjSaveID = ESPUI.addControl(ControlType::Button,
                                 SAVE_SETTINGS_STR,
                                 SAVE_SETTINGS_STR,
                                 ControlColor::Wetasphalt,
                                 adjTab,
                                 &saveSettingsCallback);
    adjSaveMsgID =
        ESPUI.addControl(ControlType::Label, "SAVE", "", ControlColor::Wetasphalt, adjSaveID);


    // ************
    // Radio Tab

    ESPUI.addControl(ControlType::Separator, RADIO_SEP_RF_SET_STR, "", ControlColor::None, radioTab);

    tempFloat   = float(fmFreqX10) / 10.0f;
    tempStr     = String(tempFloat, 1);
    tempStr    += UNITS_MHZ_STR;
    radioFreqID = ESPUI.addControl(ControlType::Label,
                                   RADIO_FM_FRQ_STR,
                                   tempStr,
                                   ControlColor::Emerald,
                                   radioTab);

    radioRfEnbID = ESPUI.addControl(ControlType::Switcher,
                                    RADIO_RF_CARRIER_STR,
                                    rfCarrierFlg ? "1" : "0",
                                    ControlColor::Emerald,
                                    radioTab,
                                    &rfCarrierCallback);

    // RF Power Control is not compatible with the RF Amp Circutry.
    // Low Power levels do not correctly excite the PA Transistor.
    /*
    radioPwrID = ESPUI.addControl(ControlType::Select,
                                  RADIO_RF_POWER_STR,
                                  rfPowerStr,
                                  ControlColor::Emerald,
                                  radioTab,
                                  &rfPowerCallback);
    ESPUI.addControl(ControlType::Option, RF_PWR_LOW_STR,  RF_PWR_LOW_STR,  ControlColor::Emerald, radioPwrID);
    ESPUI.addControl(ControlType::Option, RF_PWR_MED_STR,  RF_PWR_MED_STR,  ControlColor::Emerald, radioPwrID);
    ESPUI.addControl(ControlType::Option, RF_PWR_HIGH_STR, RF_PWR_HIGH_STR, ControlColor::Emerald, radioPwrID);
    */

    /*
       // RF Auto Off is NOT compatible with PixelRadio; Sending RDS Messages and/or using updateUiAudioLevel() will
       // prevent 60S Turn Off. Which makes this a useless menu feature. Code remains as a reminder not to use it.
       tempStr     =  rfAutoFlg ? RF_AUTO_ENB_STR : RF_AUTO_DIS_STR;
       radioAutoID = ESPUI.addControl(ControlType::Select, RADIO_AUTO_OFF_STR, tempStr, ControlColor::Emerald, radioTab, &rfAutoCallback);
       ESPUI.addControl(ControlType::Option,    RF_AUTO_DIS_STR,   RF_AUTO_DIS_STR, ControlColor::Emerald, radioAutoID);
       ESPUI.addControl(ControlType::Option,    RF_AUTO_ENB_STR,   RF_AUTO_ENB_STR, ControlColor::Emerald, radioAutoID);
     */

    ESPUI.addControl(ControlType::Separator, RADIO_SEP_MOD_STR, "", ControlColor::None, radioTab);

    radioAudioID =
        ESPUI.addControl(ControlType::Switcher, RADIO_AUDIO_MODE_STR, stereoEnbFlg ? "1" : "0",
                         ControlColor::Emerald, radioTab, &audioCallback);

    tempStr         = stereoEnbFlg ? RADIO_STEREO_STR : RADIO_MONO_STR;
    radioAudioMsgID =
        ESPUI.addControl(ControlType::Label, RADIO_AUDIO_MODE_STR, tempStr, ControlColor::Emerald, radioAudioID);

    #ifdef ADV_RADIO_FEATURES
    radioEmphID =
        ESPUI.addControl(ControlType::Select,
                         RADIO_PRE_EMPH_STR,
                         preEmphasisStr,
                         ControlColor::Emerald,
                         radioTab,
                         &radioEmphasisCallback);
    ESPUI.addControl(ControlType::Option,    PRE_EMPH_USA_STR,    PRE_EMPH_USA_STR, ControlColor::Emerald, radioEmphID);
    ESPUI.addControl(ControlType::Option,    PRE_EMPH_EUR_STR,    PRE_EMPH_EUR_STR, ControlColor::Emerald, radioEmphID);
    #endif // ifdef ADV_RADIO_FEATURES

    ESPUI.addControl(ControlType::Separator, RADIO_SEP_AUDIO_STR, "",               ControlColor::None,    radioTab);

    radioVgaGainID =
        ESPUI.addControl(ControlType::Select, RADIO_VGA_AUDIO_STR, vgaGainStr, ControlColor::Emerald, radioTab,
                         &gainAdjustCallback);
    ESPUI.addControl(ControlType::Option, VGA_GAIN0_STR, VGA_GAIN0_STR, ControlColor::Emerald, radioVgaGainID);
    ESPUI.addControl(ControlType::Option, VGA_GAIN1_STR, VGA_GAIN1_STR, ControlColor::Emerald, radioVgaGainID);
    ESPUI.addControl(ControlType::Option, VGA_GAIN2_STR, VGA_GAIN2_STR, ControlColor::Emerald, radioVgaGainID);
    ESPUI.addControl(ControlType::Option, VGA_GAIN3_STR, VGA_GAIN3_STR, ControlColor::Emerald, radioVgaGainID);
    ESPUI.addControl(ControlType::Option, VGA_GAIN4_STR, VGA_GAIN4_STR, ControlColor::Emerald, radioVgaGainID);
    ESPUI.addControl(ControlType::Option, VGA_GAIN5_STR, VGA_GAIN5_STR, ControlColor::Emerald, radioVgaGainID);

    radioImpID =
        ESPUI.addControl(ControlType::Select, RADIO_INP_IMP_STR, inpImpedStr, ControlColor::Emerald, radioTab, &impAdjustCallback);
    ESPUI.addControl(ControlType::Option, INP_IMP05K_STR, INP_IMP05K_STR, ControlColor::None, radioImpID);
    ESPUI.addControl(ControlType::Option, INP_IMP10K_STR, INP_IMP10K_STR, ControlColor::None, radioImpID);
    ESPUI.addControl(ControlType::Option, INP_IMP20K_STR, INP_IMP20K_STR, ControlColor::None, radioImpID);
    ESPUI.addControl(ControlType::Option, INP_IMP40K_STR, INP_IMP40K_STR, ControlColor::None, radioImpID);

    #ifdef ADV_RADIO_FEATURES
    radioDgainID =
        ESPUI.addControl(ControlType::Select,
                         RADIO_DIG_AUDIO_STR,
                         digitalGainStr,
                         ControlColor::Emerald,
                         radioTab,
                         &gainAdjustCallback);
    ESPUI.addControl(ControlType::Option,    DIG_GAIN0_STR,       DIG_GAIN0_STR, ControlColor::Emerald, radioDgainID);
    ESPUI.addControl(ControlType::Option,    DIG_GAIN1_STR,       DIG_GAIN1_STR, ControlColor::Emerald, radioDgainID);
    ESPUI.addControl(ControlType::Option,    DIG_GAIN2_STR,       DIG_GAIN2_STR, ControlColor::Emerald, radioDgainID);
    #endif // ifdef ADV_RADIO_FEATURES

    ESPUI.addControl(ControlType::Separator, RADIO_AMEAS_SEP_STR, "",            ControlColor::None,    radioTab);

    tempStr      = "0%"; // Must use Variable & Update each second.
    radioSoundID = ESPUI.addControl(ControlType::Label, RADIO_AUDLVL_STR, tempStr, ControlColor::Emerald, radioTab);

    #ifdef ADV_RADIO_FEATURES
    tempStr  = getAudioGain();
    tempStr += " dB";
    ESPUI.print(radioGainID, tempStr);
    radioGainID = ESPUI.addControl(ControlType::Label, RADIO_AUDIO_GAIN_STR, tempStr, ControlColor::Emerald, radioTab);
    #endif // ifdef ADV_RADIO_FEATURES

    ESPUI.addControl(ControlType::Separator, SAVE_SETTINGS_STR, "", ControlColor::None, radioTab);
    radioSaveID = ESPUI.addControl(ControlType::Button,
                                   SAVE_SETTINGS_STR,
                                   SAVE_SETTINGS_STR,
                                   ControlColor::Emerald,
                                   radioTab,
                                   &saveSettingsCallback);
    radioSaveMsgID =
        ESPUI.addControl(ControlType::Label, "SAVE", "", ControlColor::Emerald, radioSaveID);

    //
    // *****************
    // Local RDS Tab

    ESPUI.addControl(ControlType::Separator, RDS_RADIOTEXT1_STR, "", ControlColor::None, rdsTab);

    rdsText1ID =
        ESPUI.addControl(ControlType::Text, RDS_MSG1_STR, rdsTextMsg1Str, ControlColor::Alizarin, rdsTab,
                         &rdsTextCallback);

    rdsText1MsgID = ESPUI.addControl(ControlType::Label, "MSG_AREA", "", ControlColor::Alizarin, rdsText1ID);

    rdsEnb1ID =
        ESPUI.addControl(ControlType::Switcher,
                         RDS_ENB1_STR,
                         (rdsText1EnbFlg ? "1" : "0"),
                         ControlColor::Alizarin,
                         rdsTab,
                         &rdsEnbCallback);


    ESPUI.addControl(ControlType::Separator, RDS_RADIOTEXT2_STR, "", ControlColor::None, rdsTab);

    rdsText2ID =
        ESPUI.addControl(ControlType::Text, RDS_MSG2_STR, rdsTextMsg2Str, ControlColor::Alizarin, rdsTab,
                         &rdsTextCallback);

    rdsText2MsgID = ESPUI.addControl(ControlType::Label, "MSG_AREA", "", ControlColor::Alizarin, rdsText2ID);

    rdsEnb2ID =
        ESPUI.addControl(ControlType::Switcher,
                         RDS_ENB2_STR,
                         (rdsText2EnbFlg ? "1" : "0"),
                         ControlColor::Alizarin,
                         rdsTab,
                         &rdsEnbCallback);

    ESPUI.addControl(ControlType::Separator, RDS_RADIOTEXT3_STR, "", ControlColor::None, rdsTab);

    rdsText3ID =
        ESPUI.addControl(ControlType::Text, RDS_MSG3_STR, rdsTextMsg3Str, ControlColor::Alizarin, rdsTab,
                         &rdsTextCallback);

    rdsText3MsgID = ESPUI.addControl(ControlType::Label, "MSG_AREA", "", ControlColor::Alizarin, rdsText3ID);

    rdsEnb3ID =
        ESPUI.addControl(ControlType::Switcher,
                         RDS_ENB3_STR,
                         (rdsText3EnbFlg ? "1" : "0"),
                         ControlColor::Alizarin,
                         rdsTab,
                         &rdsEnbCallback);

    ESPUI.addControl(ControlType::Separator, RDS_GENERAL_SET_STR, "", ControlColor::None, rdsTab);

    tempStr    = String(rdsLocalMsgTime / 1000);
    rdsDspTmID =
        ESPUI.addControl(ControlType::Number, RDS_DISPLAY_TIME_STR, tempStr, ControlColor::Alizarin, rdsTab, &rdsDisplayTimeCallback);
    ESPUI.addControl(ControlType::Min, "MIN", String(RDS_DSP_TM_MIN), ControlColor::None, rdsDspTmID);
    ESPUI.addControl(ControlType::Max, "MAX", String(RDS_DSP_TM_MAX), ControlColor::None, rdsDspTmID);

    rdsProgNameID =
        ESPUI.addControl(ControlType::Text, RDS_PROG_SERV_NM_STR, rdsLocalPsnStr, ControlColor::Alizarin, rdsTab,
                         &rdsTextCallback);

    /*    char buff[10];
        sprintf(buff, " :0x%04X", rdsLocalPiCode);
        tempStr    = RDS_PI_CODE_STR;
        tempStr   += buff;
        rdsPiMsgID = ESPUI.addControl(ControlType::Label, "PI_CODE", tempStr, ControlColor::Alizarin, rdsProgNameID);
     */
    char buff[10];
    sprintf(buff, "0x%04X", rdsLocalPiCode);
    rdsPiID =
        ESPUI.addControl(ControlType::Text, RDS_PI_CODE_STR, buff, ControlColor::Alizarin, rdsTab,
                         &setPiCodeCallback);

    ESPUI.addControl(ControlType::Separator, RDS_RESET_SEP_STR, "", ControlColor::None, rdsTab);

    rdsRstID = ESPUI.addControl(ControlType::Button,
                                RDS_RESET_STR,
                                RDS_RESTORE_STR,
                                ControlColor::Alizarin,
                                rdsTab,
                                &rdsRstCallback);

    ESPUI.addControl(ControlType::Separator, SAVE_SETTINGS_STR, "", ControlColor::None, rdsTab);
    rdsSaveID = ESPUI.addControl(ControlType::Button,
                                 SAVE_SETTINGS_STR,
                                 SAVE_SETTINGS_STR,
                                 ControlColor::Alizarin,
                                 rdsTab,
                                 &saveSettingsCallback);
    rdsSaveMsgID =
        ESPUI.addControl(ControlType::Label, "SAVE", "", ControlColor::Alizarin, rdsSaveID);

    //
    // *************
    //  WiFi Tab

    ESPUI.addControl(ControlType::Separator, WIFI_STATUS_SEP_STR, "", ControlColor::None, wifiTab);

    tempStr    = getRSSI();
    tempStr   += UNITS_DBM_STR;
    wifiRssiID =
        ESPUI.addControl(ControlType::Label, WIFI_RSSI_STR, tempStr, ControlColor::Carrot, wifiTab);
    wifiStaID =
        ESPUI.addControl(ControlType::Label, WIFI_STA_STATUS_STR, String(getWifiModeStr()), ControlColor::Carrot, wifiTab);
    wifiStaMsgID =
        ESPUI.addControl(ControlType::Label, "IP_ADDR", ipAddrStr, ControlColor::Carrot, wifiStaID);


    ESPUI.addControl(ControlType::Separator, WIFI_CRED_SEP_STR, "", ControlColor::None, wifiTab);

    wifiSsidID =
        ESPUI.addControl(ControlType::Text, WIFI_SSID_STR, wifiSSIDStr, ControlColor::Carrot, wifiTab,
                         &setWiFiAuthenticationCallback);
    wifiWpaKeyID =
        ESPUI.addControl(ControlType::Text,
                         WIFI_WPA_KEY_STR,
                         WIFI_PASS_HIDE_STR,
                         ControlColor::Carrot,
                         wifiTab,
                         &setWiFiAuthenticationCallback);

    ESPUI.addControl(ControlType::Separator, WIFI_ADDR_SEP_STR, "", ControlColor::None, wifiTab);

    wifiIpID =
        ESPUI.addControl(ControlType::Text,
                         WIFI_IP_ADDR_STR,
                         staticIpStr,
                         ControlColor::Carrot,
                         wifiTab,
                         &setWiFiAddrsCallback);

    wifiGatewayID =
        ESPUI.addControl(ControlType::Text,
                         WIFI_GATEWAY_STR,
                         wifiGatewayStr,
                         ControlColor::Carrot,
                         wifiTab,
                         &setWiFiAddrsCallback);

    wifiSubID =
        ESPUI.addControl(ControlType::Text,
                         WIFI_SUBNET_STR,
                         subNetStr,
                         ControlColor::Carrot,
                         wifiTab,
                         &setWiFiAddrsCallback);

    wifiDnsID =
        ESPUI.addControl(ControlType::Text,
                         WIFI_DNS_STR,
                         wifiDnsStr,
                         ControlColor::Carrot,
                         wifiTab,
                         &setWiFiAddrsCallback);


    if ((wifiDhcpFlg == false) && (wifiValidateSettings() == false)) {
        tempStr     = DHCP_LOCKED_STR;
        wifiDhcpFlg = true; // Force DHCP mode.
    }
    else {
        tempStr = "";
    }
    wifiDhcpID =
        ESPUI.addControl(ControlType::Switcher,
                         WIFI_WEB_DHCP_STR,
                         wifiDhcpFlg ? "1" : "0",
                         ControlColor::Carrot,
                         wifiTab,
                         &dhcpCallback);
    wifiDhcpMsgID = ESPUI.addControl(ControlType::Label, "MSG_AREA", tempStr, ControlColor::Carrot, wifiDhcpID);


    ESPUI.addControl(ControlType::Separator, WIFI_NAME_SEP_STR, "", ControlColor::None, wifiTab);
    wifiStaNameID =
        ESPUI.addControl(ControlType::Text, WIFI_WEBSRV_NAME_STR, staNameStr, ControlColor::Carrot, wifiTab, &setWiFiNamesCallback);

    wifiApNameID =
        ESPUI.addControl(ControlType::Text, WIFI_AP_NAME_STR, apNameStr, ControlColor::Carrot, wifiTab, &setWiFiNamesCallback);

    // ------------------ START OF OPTIONAL MDNS SECTION ----------------------
    #ifdef MDNS_ENB
    wifiMdnsNameID =
        ESPUI.addControl(ControlType::Text, WIFI_MDNS_NAME_STR, mdnsNameStr, ControlColor::Carrot, wifiTab, &setWiFiNamesCallback);
    #endif // ifdef MDNS_ENB
    // ------------------ END OF OPTIONAL MDNS SECTION ----------------------

    ESPUI.addControl(ControlType::Separator, WIFI_DEV_CRED_SEP_STR, "", ControlColor::None, wifiTab);

    wifiDevUserID = ESPUI.addControl(ControlType::Text,
                                     WIFI_DEV_USER_NM_STR,
                                     userNameStr,
                                     ControlColor::Carrot,
                                     wifiTab,
                                     &setLoginCallback);

    wifiDevUserMsgID = ESPUI.addControl(ControlType::Label, "MSG_AREA", WIFI_BLANK_MSG_STR, ControlColor::Alizarin, wifiDevUserID);


    wifiDevPwID = ESPUI.addControl(ControlType::Text,
                                   WIFI_DEV_USER_PW_STR,
                                   userPassStr,
                                   ControlColor::Carrot,
                                   wifiTab,
                                   &setLoginCallback);

    wifiDevPwMsgID = ESPUI.addControl(ControlType::Label, "MSG_AREA", WIFI_BLANK_MSG_STR, ControlColor::Alizarin, wifiDevPwID);

    ESPUI.addControl(ControlType::Separator, WIFI_AP_IP_SEP_STR, "", ControlColor::None, wifiTab);

    wifiApIpID =
        ESPUI.addControl(ControlType::Text, WIFI_AP_IP_ADDR_STR, apIpAddrStr, ControlColor::Carrot, wifiTab, &setWiFiAddrsCallback);

    wifiApFallID =
        ESPUI.addControl(ControlType::Switcher, WIFI_AP_FALLBK_STR, "1", ControlColor::Carrot, wifiTab, &apFallBkCallback);

    //    ESPUI.addControl(ControlType::Separator, WIFI_BOOT_SEP_STR, "", ControlColor::None, wifiTab);

    wifiApBootID =
        ESPUI.addControl(ControlType::Switcher,
                         WIFI_AP_REBOOT_STR,
                         WiFiRebootFlg ? "1" : "0",
                         ControlColor::Carrot,
                         wifiTab,
                         &apBootCallback);

    ESPUI.addControl(ControlType::Separator, SAVE_SETTINGS_STR, "", ControlColor::None, wifiTab);
    wifiSaveID = ESPUI.addControl(ControlType::Button,
                                  SAVE_SETTINGS_STR,
                                  SAVE_SETTINGS_STR,
                                  ControlColor::Carrot,
                                  wifiTab,
                                  &saveSettingsCallback);
    wifiSaveMsgID =
        ESPUI.addControl(ControlType::Label, "SAVE", "", ControlColor::Carrot, wifiSaveID);


    //
    // *************
    //  Controller Tab

    ESPUI.addControl(ControlType::Separator, CTRL_USB_SERIAL_STR, "", ControlColor::None, ctrlTab);

    ctrlSerialID =
        ESPUI.addControl(ControlType::Select, CTRL_SERIAL_STR, ctrlSerialStr, ControlColor::Turquoise, ctrlTab, &serialCallback);
    ESPUI.addControl(ControlType::Option, SERIAL_OFF_STR, SERIAL_OFF_STR, ControlColor::None, ctrlSerialID);
    ESPUI.addControl(ControlType::Option, SERIAL_096_STR, SERIAL_096_STR, ControlColor::None, ctrlSerialID);
    ESPUI.addControl(ControlType::Option, SERIAL_192_STR, SERIAL_192_STR, ControlColor::None, ctrlSerialID);
    ESPUI.addControl(ControlType::Option, SERIAL_576_STR, SERIAL_576_STR, ControlColor::None, ctrlSerialID);
    ESPUI.addControl(ControlType::Option, SERIAL_115_STR, SERIAL_115_STR, ControlColor::None, ctrlSerialID);

    tempStr         = logLevelStr != DIAG_LOG_SILENT_STR ? CTLR_SERIAL_MSG_STR : "";
    ctrlSerialMsgID = ESPUI.addControl(ControlType::Label, "SERIAL_MSG", tempStr, ControlColor::Turquoise, ctrlSerialID);

    // ------------- START OF OPTIONAL MQTT CONTROLLER ------------------------
    #ifdef MQTT_ENB
    ESPUI.addControl(ControlType::Separator, CTRL_MQTT_SEP_STR, "", ControlColor::None, ctrlTab);
    ctrlMqttID =
        ESPUI.addControl(ControlType::Switcher, CTRL_MQTT_STR, ctrlMqttFlg ? "1" : "0", ControlColor::Turquoise, ctrlTab,
                         &controllerCallback);

    if ((ctrlMqttFlg == true) && ((!mqttIpStr.length()) || (!mqttNameStr.length()) ||
                                  (!mqttUserStr.length()) || (!mqttPwStr.length()))) {
        tempStr     = MQTT_MISSING_STR;
        ctrlMqttFlg = false; // Force DHCP mode.
    }
    else if (getWifiMode() == WIFI_AP) {
        tempStr = MQTT_NOT_AVAIL_STR;
    }
    else {
        tempStr = "";
    }
    ctrlMqttMsgID  = ESPUI.addControl(ControlType::Label, "MSG_AREA", tempStr, ControlColor::Turquoise, ctrlMqttID);
    ctrlMqttNameID =
        ESPUI.addControl(ControlType::Text, MQTT_SUBSCR_NM_STR, mqttNameStr, ControlColor::Turquoise, ctrlTab, &setMqttNameCallback);
    ctrlMqttIpID =
        ESPUI.addControl(ControlType::Text, CTRL_MQTT_IP_STR, mqttIpStr, ControlColor::Turquoise, ctrlTab,
                         &setMqttIpAddrCallback);
    ctrlMqttPortID =
        ESPUI.addControl(ControlType::Label, CTRL_MQTT_PORT_STR, String(mqttPort), ControlColor::Turquoise, ctrlTab);
    ctrlMqttUserID =
        ESPUI.addControl(ControlType::Text, CTRL_MQTT_USER_STR, mqttUserStr, ControlColor::Turquoise, ctrlTab,
                         &setMqttAuthenticationCallback);
    ctrlMqttPwID =
        ESPUI.addControl(ControlType::Text, CTRL_MQTT_PW_STR, MQTT_PASS_HIDE_STR, ControlColor::Turquoise, ctrlTab,
                         &setMqttAuthenticationCallback);
    #endif // ifdef MQTT_ENB

    // ------------- END OF OPTIONAL MQTT CONTROLLER ------------------------


    // ------------- START OF OPTIONAL HTTP CONTROLLER ------------------------
    #ifdef HTTP_ENB
    ESPUI.addControl(ControlType::Separator, CTRL_HTPP_SET_STR, "", ControlColor::None, ctrlTab);
    ctrlHttpID =
        ESPUI.addControl(ControlType::Switcher, CTRL_HTTP_STR, ctrlHttpFlg ? "1" : "0", ControlColor::Turquoise, ctrlTab,
                         &controllerCallback);
    #endif
    // ------------- END OF OPTIONAL HTTP CONTROLLER ------------------------

    ESPUI.addControl(ControlType::Separator, CTRL_LOCAL_SEP_STR, "", ControlColor::None, ctrlTab);
    ctrlLocalID =
        ESPUI.addControl(ControlType::Switcher,
                         "LOCAL CONTROL",
                         ctrlLocalFlg ? "1" : "0",
                         ControlColor::Turquoise,
                         ctrlTab,
                         &controllerCallback);

    ESPUI.addControl(ControlType::Separator, SAVE_SETTINGS_STR, "", ControlColor::None, ctrlTab);
    ctrlSaveID = ESPUI.addControl(ControlType::Button,
                                  SAVE_SETTINGS_STR,
                                  SAVE_SETTINGS_STR,
                                  ControlColor::Turquoise,
                                  ctrlTab,
                                  &saveSettingsCallback);
    ctrlSaveMsgID =
        ESPUI.addControl(ControlType::Label, "SAVE", "", ControlColor::Turquoise, ctrlSaveID);

    //
    // *****************
    // GPIO Tab

    ESPUI.addControl(ControlType::Separator, GPIO_SETTINGS_STR, "", ControlColor::None, gpioTab);
    gpio19ID =
        ESPUI.addControl(ControlType::Select, GPIO_19_STR, gpio19BootStr, ControlColor::Dark, gpioTab, &gpioCallback);
    ESPUI.addControl(ControlType::Option, GPIO_INP_FT_STR, GPIO_INP_FT_STR, ControlColor::None, gpio19ID);
    ESPUI.addControl(ControlType::Option, GPIO_INP_PU_STR, GPIO_INP_PU_STR, ControlColor::None, gpio19ID);
    ESPUI.addControl(ControlType::Option, GPIO_INP_PD_STR, GPIO_INP_PD_STR, ControlColor::None, gpio19ID);
    ESPUI.addControl(ControlType::Option, GPIO_OUT_LO_STR, GPIO_OUT_LO_STR, ControlColor::None, gpio19ID);
    ESPUI.addControl(ControlType::Option, GPIO_OUT_HI_STR, GPIO_OUT_HI_STR, ControlColor::None, gpio19ID);
    gpio19MsgID = ESPUI.addControl(ControlType::Label, "GPIO_MSG", " ", ControlColor::None, gpio19ID);

    gpio23ID =
        ESPUI.addControl(ControlType::Select, GPIO_23_STR, gpio23BootStr, ControlColor::Dark, gpioTab, &gpioCallback);
    ESPUI.addControl(ControlType::Option, GPIO_INP_FT_STR, GPIO_INP_FT_STR, ControlColor::None, gpio23ID);
    ESPUI.addControl(ControlType::Option, GPIO_INP_PU_STR, GPIO_INP_PU_STR, ControlColor::None, gpio23ID);
    ESPUI.addControl(ControlType::Option, GPIO_INP_PD_STR, GPIO_INP_PD_STR, ControlColor::None, gpio23ID);
    ESPUI.addControl(ControlType::Option, GPIO_OUT_LO_STR, GPIO_OUT_LO_STR, ControlColor::None, gpio23ID);
    ESPUI.addControl(ControlType::Option, GPIO_OUT_HI_STR, GPIO_OUT_HI_STR, ControlColor::None, gpio23ID);
    gpio23MsgID = ESPUI.addControl(ControlType::Label, "GPIO_MSG", " ", ControlColor::None, gpio23ID);

    gpio33ID =
        ESPUI.addControl(ControlType::Select, GPIO_33_STR, gpio33BootStr, ControlColor::Dark, gpioTab, &gpioCallback);
    ESPUI.addControl(ControlType::Option, GPIO_INP_FT_STR, GPIO_INP_FT_STR, ControlColor::None, gpio33ID);
    ESPUI.addControl(ControlType::Option, GPIO_INP_PU_STR, GPIO_INP_PU_STR, ControlColor::None, gpio33ID);
    ESPUI.addControl(ControlType::Option, GPIO_INP_PD_STR, GPIO_INP_PD_STR, ControlColor::None, gpio33ID);
    ESPUI.addControl(ControlType::Option, GPIO_OUT_LO_STR, GPIO_OUT_LO_STR, ControlColor::None, gpio33ID);
    ESPUI.addControl(ControlType::Option, GPIO_OUT_HI_STR, GPIO_OUT_HI_STR, ControlColor::None, gpio33ID);
    gpio33MsgID = ESPUI.addControl(ControlType::Label, "GPIO_MSG", " ", ControlColor::None, gpio33ID);

    ESPUI.addControl(ControlType::Separator, SAVE_SETTINGS_STR, "", ControlColor::None, gpioTab);
    gpioSaveID = ESPUI.addControl(ControlType::Button,
                                  SAVE_SETTINGS_STR,
                                  SAVE_SETTINGS_STR,
                                  ControlColor::Dark,
                                  gpioTab,
                                  &saveSettingsCallback);
    gpioSaveMsgID =
        ESPUI.addControl(ControlType::Label, "SAVE", "", ControlColor::None, gpioSaveID);


    //
    // *****************
    // Backup Tab

    ESPUI.addControl(ControlType::Separator, SAVE_SETTINGS_STR, "", ControlColor::None, backupTab);
    backupSaveSetID = ESPUI.addControl(ControlType::Button,
                                       SAVE_SETTINGS_STR,
                                       SAVE_SETTINGS_STR,
                                       ControlColor::Wetasphalt,
                                       backupTab,
                                       &saveSettingsCallback);
    backupSaveSetMsgID =
        ESPUI.addControl(ControlType::Label, "SAVE", "", ControlColor::Wetasphalt, backupSaveSetID);

    ESPUI.addControl(ControlType::Separator, SAVE_BACKUP_STR, "", ControlColor::None, backupTab);
    backupSaveID =
        ESPUI.addControl(ControlType::Button,
                         BACKUP_SAV_CFG_STR,
                         BACKUP_SAVE_STR,
                         ControlColor::Wetasphalt,
                         backupTab,
                         &backupCallback);
    backupSaveMsgID = ESPUI.addControl(ControlType::Label, "SAVE_MSG", "", ControlColor::Wetasphalt, backupSaveID);

    backupRestoreID =
        ESPUI.addControl(ControlType::Button,
                         BACKUP_RES_CFG_STR,
                         BACKUP_RESTORE_STR,
                         ControlColor::Wetasphalt,
                         backupTab,
                         &backupCallback);
    backupRestoreMsgID = ESPUI.addControl(ControlType::Label, "RESTORE_MSG", "", ControlColor::Wetasphalt, backupRestoreID);

    //
    // ******************
    // Diagnostics Tab

    ESPUI.addControl(ControlType::Separator, DIAG_HEALTH_SEP_STR, "", ControlColor::None, diagTab);
    tempStr    = String(vbatVolts, 1);
    tempStr   += " VDC";
    diagVbatID = ESPUI.addControl(ControlType::Label, DIAG_VBAT_STR, tempStr, ControlColor::Sunflower, diagTab);

    tempStr   = String(paVolts, 1);
    tempStr  += " VDC";
    diagVdcID = ESPUI.addControl(ControlType::Label, DIAG_VDC_STR, tempStr, ControlColor::Sunflower, diagTab);

    ESPUI.addControl(ControlType::Separator, DIAG_DEBUG_SEP_STR, "", ControlColor::None, diagTab);
    diagLogID =
        ESPUI.addControl(ControlType::Select, DIAG_LOG_LVL_STR, logLevelStr, ControlColor::Sunflower, diagTab, &diagLogCallback);
    ESPUI.addControl(ControlType::Option,
                     DIAG_LOG_SILENT_STR,
                     DIAG_LOG_SILENT_STR,
                     ControlColor::None,
                     diagLogID);
    ESPUI.addControl(ControlType::Option,
                     DIAG_LOG_FATAL_STR,
                     DIAG_LOG_FATAL_STR,
                     ControlColor::None,
                     diagLogID);
    ESPUI.addControl(ControlType::Option,
                     DIAG_LOG_ERROR_STR,
                     DIAG_LOG_ERROR_STR,
                     ControlColor::None,
                     diagLogID);
    ESPUI.addControl(ControlType::Option,
                     DIAG_LOG_WARN_STR,
                     DIAG_LOG_WARN_STR,
                     ControlColor::None,
                     diagLogID);
    ESPUI.addControl(ControlType::Option,
                     DIAG_LOG_INFO_STR,
                     DIAG_LOG_INFO_STR,
                     ControlColor::None,
                     diagLogID);
    ESPUI.addControl(ControlType::Option,
                     DIAG_LOG_TRACE_STR,
                     DIAG_LOG_TRACE_STR,
                     ControlColor::None,
                     diagLogID);
    ESPUI.addControl(ControlType::Option,
                     DIAG_LOG_VERB_STR,
                     DIAG_LOG_VERB_STR,
                     ControlColor::None,
                     diagLogID);

    tempStr      = ctrlSerialStr == SERIAL_OFF_STR ? "" : DIAG_LOG_MSG_STR;
    diagLogMsgID = ESPUI.addControl(ControlType::Label, "LOG_MSG", tempStr, ControlColor::Sunflower, diagLogID);

    ESPUI.addControl(ControlType::Separator, DIAG_SYSTEM_SEP_STR, "", ControlColor::None, diagTab);

    tempStr      = ESP.getFreeHeap();
    tempStr     += " Bytes";
    diagMemoryID = ESPUI.addControl(ControlType::Label, DIAG_FREE_MEM_STR, tempStr, ControlColor::Sunflower, diagTab);

    diagTimerID = ESPUI.addControl(ControlType::Label, DIAG_RUN_TIME_STR, "", ControlColor::Sunflower, diagTab);

    diagBootID =
        ESPUI.addControl(ControlType::Button,
                         DIAG_REBOOT_STR,
                         DIAG_LONG_PRESS_STR,
                         ControlColor::Sunflower,
                         diagTab,
                         &diagBootCallback);
    diagBootMsgID = ESPUI.addControl(ControlType::Label, "REBOOT_MSG", "", ControlColor::Sunflower, diagBootID);

    //
    // ******************
    // About Tab

    tempStr     = "Version ";
    tempStr    += VERSION_STR;
    tempStr    += "<br>";
    tempStr    += BLD_DATE_STR;
    tempStr    += "<br>";
    tempStr    += AUTHOR_STR;
    tempStr    += "<br>";
    tempStr    += GITHUB_REPO_STR;
    tempStr    += "<br>";
    aboutLogoID = ESPUI.addControl(ControlType::Label,
                                   ABOUT_VERS_STR,

/*(makeWebGif("/RadioLogo225x75_base64.gif", 225, 75, "white")),*/
                                   (makeWebGif("/RadioLogo225x75_base64.gif", 200, 66, "white")),
                                   ControlColor::None,
                                   aboutTab);

    aboutVersionID = ESPUI.addControl(ControlType::Label, ABOUT_VERS_STR, tempStr, ControlColor::None, aboutLogoID);
}
