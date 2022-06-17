/*
   File: callBacks.cpp (ESPUI Web Page Callback Functions)
   Project: PixelRadio, an RBDS/RDS FM Transmitter (QN8027 Digital FM IC)
   Version: 1.1.0
   Creation: Dec-16-2021
   Revised:  Jun-13-2022
   Revision History: See PixelRadio.cpp
   Project Leader: T. Black (thomastech)
   Contributors: thomastech, dkulp

   (c) copyright T. Black 2021-2022, Licensed under GNU GPL 3.0 and later, under this license absolutely no warranty is given.
   This Code was formatted with the uncrustify extension.
 */

#include <ArduinoLog.h>
#include "PixelRadio.h"
#include "config.h"
#include "ESPUI.h"
#include "globals.h"
#include "language.h"
#include "webGUI.h"

// ************************************************************************************************
void adjFmFreqCallback(Control *sender, int type) {
    char logBuff[60];

    // sprintf(logBuff, "adjFmFreqCallback ID: %d, Value: %s", sender->id, sender->value.c_str());
    // Log.verboseln(logBuff);

    if (sender->id == adjFreqID) {
        if (type == P_LEFT_DOWN) {       // Decr
            fmFreqX10 -= FM_FREQ_SKP_KHZ;
        }
        else if (type == P_RIGHT_DOWN) { // Incr
            fmFreqX10 += FM_FREQ_SKP_KHZ;
        }
        else if (type == P_BACK_DOWN) {  // Decr
            fmFreqX10 -= FM_FREQ_SKP_MHZ;
        }
        else if (type == P_FOR_DOWN) {   // Incr
            fmFreqX10 += FM_FREQ_SKP_MHZ;
        }
        else if ((type == P_LEFT_UP) || (type == P_RIGHT_UP) || (type == P_BACK_UP) || (type == P_FOR_UP)) {
            Log.verboseln("FM Frequency Pad Button Released.");
            return;
        }
        else {
            sprintf(logBuff, "adjFmFreqCallback: %s.", BAD_VALUE_STR);
            Log.errorln(logBuff);
            return;
        }

        if (fmFreqX10 > FM_FREQ_MAX_X10) {
            fmFreqX10 = FM_FREQ_MAX_X10;
        }
        else if (fmFreqX10 < FM_FREQ_MIN_X10) {
            fmFreqX10 = FM_FREQ_MIN_X10;
        }

        newFreqFlg = true; // Tell main loop we have a new setting to update.
        float tempFloat = float(fmFreqX10) / 10.0f;
        tempStr  = String(tempFloat, 1);
        tempStr += UNITS_MHZ_STR;

        updateUiFrequency();
        displaySaveWarning();
        sprintf(logBuff, "FM Frequency Set to: %s.", tempStr.c_str());
        Log.infoln(logBuff);
    }
    else {
        sprintf(logBuff, "adjFmFreqCallback: %s.", BAD_SENDER_STR);
        Log.errorln(logBuff);
    }
}

// ************************************************************************************************
// apFallBkCallback(): Enable/Disable AP Fallback using control element on WiFi Tab.
void apBootCallback(Control *sender, int type)
{
    char logBuff[60];

    // sprintf(logBuff, "apBootCallback ID: %d, Value: %s", sender->id, sender->value.c_str());
    // Log.verboseln(logBuff);

    if (sender->id == wifiApBootID) {
        if (type == S_ACTIVE) {
            WiFiRebootFlg = true;
        }
        else if (type == S_INACTIVE) {
            WiFiRebootFlg = false;
        }
        else {
            sprintf(logBuff, "apBootCallback: %s.", BAD_VALUE_STR);
            Log.errorln(logBuff);
        }
        displaySaveWarning();
        Log.infoln("AP Reboot Set to: %s.", WiFiRebootFlg ? "On" : "Off");
    }
    else {
        sprintf(logBuff, "apBootCallback: %s.", BAD_SENDER_STR);
        Log.errorln(logBuff);
    }
}

// ************************************************************************************************
// apFallBkCallback(): Enable/Disable AP Fallback using control element on WiFi Tab.
void apFallBkCallback(Control *sender, int type)
{
    char logBuff[60];

    // sprintf(logBuff, "apFallBkCallback ID: %d, Value: %s", sender->id, sender->value.c_str());
    // Log.verboseln(logBuff);

    if (sender->id == wifiApFallID) {
        if (type == S_ACTIVE) {
            apFallBackFlg = true;
        }
        else if (type == S_INACTIVE) {
            apFallBackFlg = false;
        }
        else {
            sprintf(logBuff, "apFallBkCallback: %s.", BAD_VALUE_STR);
            Log.errorln(logBuff);
        }
        displaySaveWarning();
        Log.infoln("AP Fallback Set to: %s.", apFallBackFlg ? "On" : "Off");
    }
    else {
        sprintf(logBuff, "apFallBkCallback: %s.", BAD_SENDER_STR);
        Log.errorln(logBuff);
    }
}

// ************************************************************************************************
// audioCallback(): Update the Stereo / Mono Audio modes.
void audioCallback(Control *sender, int type)
{
    char logBuff[60];

    // sprintf(logBuff, "audioCallback ID: %d, Value: %s", sender->id, sender->value.c_str());
    // Log.verboseln(logBuff);

    if (sender->id == radioAudioID) {
        switch (type) {
          case S_ACTIVE:
              stereoEnbFlg = true;
              tempStr      = "Stereo";

              // ESPUI.print(radioAudioMsgID, RADIO_STEREO_STR);
              updateUiAudioMode();
              break;
          case S_INACTIVE:
              stereoEnbFlg = false;
              tempStr      = "Mono";

              // ESPUI.print(radioAudioMsgID, RADIO_MONO_STR);
              updateUiAudioMode();
              break;
          default:
              tempStr = BAD_VALUE_STR;
              break;
        }
        newAudioModeFlg = true; // Tell Main Loop we Have new Setting to Update.
        displaySaveWarning();
        sprintf(logBuff, "Radio Audio Mode Set to: %s.", tempStr.c_str());
        Log.infoln(logBuff);
    }
    else {
        sprintf(logBuff, "audioCallback: %s.", BAD_SENDER_STR);
        Log.errorln(logBuff);
    }
}

// ************************************************************************************************
void backupCallback(Control *sender, int type)
{
    static bool saveSuccessFlg    = true;
    static bool restoreSuccessFlg = true;
    char logBuff[60];
    char fileName[20];

    // sprintf(logBuff, "dhcpCallback ID: %d, Value: %s", sender->id, sender->value.c_str());
    // Log.verboseln(logBuff);

    sprintf(fileName, "%s", BACKUP_FILE_NAME); // Create filename for backup.

    if (sender->id == backupSaveID) {
        if (type == B_DOWN) {
            ESPUI.print(backupSaveMsgID,    ""); // Erase Message.
            ESPUI.print(backupRestoreMsgID, ""); // Erase Message.
            saveSuccessFlg = saveConfiguration(SD_CARD_MODE, fileName);

            if (saveSuccessFlg) {
                ESPUI.print(backupSaveMsgID, BACKUP_SAV_PASS_STR);
                Log.infoln("-> Backup Save Successful.");
            }
            else {
                ESPUI.print(backupSaveMsgID, BACKUP_SAV_FAIL_STR);
                Log.errorln("-> Backup Save Failed.");
            }
        }
        else if (type == B_UP) {}
        else {
            sprintf(logBuff, "backupCallback: %s.", BAD_VALUE_STR);
            Log.errorln(logBuff);
        }
    }
    else if (sender->id == backupRestoreID) {
        if (type == B_DOWN) {
            ESPUI.print(backupSaveMsgID,    ""); // Erase Message.
            ESPUI.print(backupRestoreMsgID, ""); // Erase Message.

            restoreSuccessFlg = restoreConfiguration(SD_CARD_MODE, fileName);

            if (restoreSuccessFlg) {
                restoreSuccessFlg = saveConfiguration(LITTLEFS_MODE, fileName); // Save restored SD backup to file system.
            }

            if (restoreSuccessFlg) {
                ESPUI.print(backupRestoreMsgID, BACKUP_RES_PASS_STR);
                Log.infoln("-> Backup Restore Successful. Reboot Required to Apply Settings.");
            }
            else {
                ESPUI.print(backupRestoreMsgID, BACKUP_RES_FAIL_STR);
                Log.errorln("-> Backup Restore Failed.");
            }
        }
        else if (type == B_UP) {}
        else {
            sprintf(logBuff, "backupCallback: %s.", BAD_VALUE_STR);
            Log.errorln(logBuff);
        }
    }
    else {
        sprintf(logBuff, "backupCallback: %s.", BAD_SENDER_STR);
        Log.errorln(logBuff);
    }
}

// ************************************************************************************************
// dhcpCallback(): Enable/disable DHCP Connection mode. If false, use static IP.
void dhcpCallback(Control *sender, int type)
{
    char logBuff[60];

    // sprintf(logBuff, "dhcpCallback ID: %d, Value: %s", sender->id, sender->value.c_str());
    // Log.verboseln(logBuff);

    if (sender->id == wifiDhcpID) {
        if (type == S_ACTIVE) {
            wifiDhcpFlg = true;
            ESPUI.print(wifiDhcpMsgID, ""); // Erase Status Message.
        }
        else if (type == S_INACTIVE) {
            if (!wifiValidateSettings()) {
                wifiDhcpFlg = true;              // Force back On.
                ESPUI.print(wifiDhcpID,    "1"); // Move Switch Back to On Position.
                ESPUI.print(wifiDhcpMsgID, DHCP_LOCKED_STR);
                Log.warningln("dhcpCallback: Some Wifi Settings Missing, DHCP will be used.");
            }
            else {
                wifiDhcpFlg = false;
            }
        }
        else {
            sprintf(logBuff, "dhcpCallback: %s.", BAD_VALUE_STR);
            Log.errorln(logBuff);
        }
        displaySaveWarning();
        Log.infoln("WiFi Connection Set to: %s.", wifiDhcpFlg ? "DHCP" : "Static IP");
    }
    else {
        sprintf(logBuff, "dhcpCallback: %s.", BAD_SENDER_STR);
        Log.errorln(logBuff);
    }
}

// ************************************************************************************************
void controllerCallback(Control *sender, int type)
{
    // char logBuff[60];
    // sprintf(logBuff, "controllerCallback ID: %d, Value: %s", sender->id, sender->value.c_str());
    // Log.verboseln(logBuff);

    if (sender->id == ctrlLocalID) {
        if (type == S_ACTIVE) {
            ctrlLocalFlg = true;
        }
        else if (type == S_INACTIVE) {
            ctrlLocalFlg = false;
        }
        displaySaveWarning();
        tempStr  = "Local Controller Set to: ";
        tempStr += ctrlLocalFlg ? "On" : "Off";
    }
    else if (sender->id == ctrlHttpID) {
        if (type == S_ACTIVE) {
            ctrlHttpFlg = true;
        }
        else if (type == S_INACTIVE) {
            ctrlHttpFlg = false;
        }
        displaySaveWarning();
        tempStr  = "HTTP Controller Set to: ";
        tempStr += ctrlHttpFlg ? "On" : "Off";
    }

    // ------------- START OF OPTIONAL MQTT CONTROLLER ------------------------
    #ifdef MQTT_ENB
    else if (sender->id == ctrlMqttID) {
        if (type == S_ACTIVE) {
            if ((!mqttIpStr.length()) || (!mqttNameStr.length()) ||
                (!mqttUserStr.length()) || (!mqttPwStr.length())) { // Missing MQTT Entries
                ctrlMqttFlg = false;
                updateUiMqttMsg(MQTT_MISSING_STR);
                ESPUI.print(ctrlMqttID, "0");                       // Turn off MQTT Controller Switcher.
                Log.warningln("Missing MQTT Controller Settings, Disabled MQTT.");
            }
            else {
                updateUiMqttMsg("");
                mqttReconnect(true); // Reset MQTT Reconnect values while ctrlMqttFlg is false.
                ctrlMqttFlg = true;  // Must set flag AFTER mqqtReconnect!
            }
        }
        else if (type == S_INACTIVE) {
            ctrlMqttFlg = false; // Must set flag BEFORE mqqtReconnect!
            mqttReconnect(true); // Reset MQTT Reconnect values when ctrlMqttFlg is false.
            mqttOnlineFlg ? updateUiMqttMsg(MQTT_DISCONNECT_STR) : updateUiMqttMsg("");
        }
        displaySaveWarning();
        tempStr  = "MQTT Controller Set to: ";
        tempStr += ctrlMqttFlg ? "On" : "Off";
    }
    #endif // ifdef MQTT_ENB
    // ------------- END OF OPTIONAL MQTT CONTROLLER ------------------------
    else {
        tempStr  = "controllerCallback: ";
        tempStr += BAD_SENDER_STR;
    }
    displayRdsText(); // Update RDS RadioText.

    char charBuff[40];
    sprintf(charBuff, "%s.", tempStr.c_str());
    Log.infoln(charBuff);
}

// ************************************************************************************************
// diagBootCallback(): Reboot ESP32
//                     Must hold button for several seconds, will reboot upon release.
void diagBootCallback(Control *sender, int type)
{
    char logBuff[60];
    uint32_t currentMillis    = millis(); // Snapshot of System Timer.
    static uint32_t oldMillis = 0;

    // sprintf(logBuff, "diagBootCallback ID: %d, Value: %s", sender->id, sender->value.c_str());
    // Log.verboseln(logBuff);

    if (sender->id == diagBootID) {
        switch (type) {
          case B_DOWN:
              oldMillis = millis();
              tempStr   = "color: ";
              tempStr  += COLOR_RED_STR;
              tempStr  += ";";
              ESPUI.setPanelStyle(diagBootID, tempStr); // Red Panel Name.
              ESPUI.print(diagBootMsgID, DIAG_BOOT_MSG1_STR);
              Log.infoln("diagBootCallback: Reboot Button Pressed.");
              break;
          case B_UP:

              if (currentMillis > oldMillis + 5000) {
                  tempStr  = "background-color: ";
                  tempStr += COLOR_RED_STR;
                  tempStr += ";";
                  ESPUI.setPanelStyle(diagBootID, tempStr); // Red Panel Body
                  ESPUI.print(diagBootMsgID, DIAG_BOOT_MSG2_STR);
                  Log.warningln("diagBootCallback: Reboot Activated. Goodbye!");
                  Serial.flush();
                  rebootFlg = true; // Tell Main Loop to reboot.
              }
              else {
                  tempStr  = "color: ";
                  tempStr += COLOR_BLK_STR;
                  tempStr += ";";
                  ESPUI.setPanelStyle(diagBootID, tempStr);                      // White (default) Panel Name.
                  ESPUI.getControl(diagBootID)->color = ControlColor::Sunflower; // Restore orig panel color
                  ESPUI.updateControl(diagBootID);                               // Apply changes to control.
                  ESPUI.print(diagBootMsgID, "");
                  Log.infoln("diagBootCallback: Reboot Button Released (Canceled).");
              }
              break;
          default:
              sprintf(logBuff, "diagBootCallback: %s.", BAD_VALUE_STR);
              Log.errorln(logBuff);
              break;
        }
    }
    else {
        sprintf(logBuff, "diagBootCallback: %s.", BAD_SENDER_STR);
        Log.errorln(logBuff);
    }
}

// ************************************************************************************************
// diagLogCallback() Set the Serial Log Level. Used for system debugging.
void diagLogCallback(Control *sender, int type)
{
    char   logBuff[60];
    String serialMsg;

    // sprintf(logBuff, "diagLogCallback ID: %d, Value: %s", sender->id, sender->value.c_str());
    // Log.verboseln(logBuff);

    if (sender->id == diagLogID) {
        if (sender->value == DIAG_LOG_SILENT_STR) {
            // Log.begin(LOG_LEVEL_SILENT, &Serial);
            // Log.setShowLevel(false); // Do not show loglevel, we will do this in the prefix
            logLevelStr = DIAG_LOG_SILENT_STR;
            tempStr     = logLevelStr;
            serialMsg   = ""; // Erase warning message on Serial Controller Panel (on ctrlTab).
        }
        else if (sender->value == DIAG_LOG_FATAL_STR) {
            // Log.begin(LOG_LEVEL_FATAL, &Serial);
            // Log.setShowLevel(false);           // Do not show loglevel, we will do this in the prefix
            logLevelStr = DIAG_LOG_FATAL_STR;
            tempStr     = logLevelStr;
            serialMsg   = CTLR_SERIAL_MSG_STR; // Warning message on Serial Controller Panel (on ctrlTab).
        }
        else if (sender->value == DIAG_LOG_ERROR_STR) {
            // Log.begin(LOG_LEVEL_ERROR, &Serial);
            // Log.setShowLevel(false); // Do not show loglevel, we will do this in the prefix
            logLevelStr = DIAG_LOG_ERROR_STR;
            tempStr     = logLevelStr;
            serialMsg   = CTLR_SERIAL_MSG_STR;
        }
        else if (sender->value == DIAG_LOG_WARN_STR) {
            // Log.begin(LOG_LEVEL_WARNING, &Serial);
            // Log.setShowLevel(false); // Do not show loglevel, we will do this in the prefix
            logLevelStr = DIAG_LOG_WARN_STR;
            tempStr     = logLevelStr;
            serialMsg   = CTLR_SERIAL_MSG_STR;
        }
        else if (sender->value == DIAG_LOG_INFO_STR) {
            // Log.begin(LOG_LEVEL_INFO, &Serial);
            // Log.setShowLevel(false); // Do not show loglevel, we will do this in the prefix
            logLevelStr = DIAG_LOG_INFO_STR;
            tempStr     = logLevelStr;
            serialMsg   = CTLR_SERIAL_MSG_STR;
        }
        else if (sender->value == DIAG_LOG_TRACE_STR) {
            // Log.begin(LOG_LEVEL_TRACE, &Serial);
            // Log.setShowLevel(false); // Do not show loglevel, we will do this in the prefix
            logLevelStr = DIAG_LOG_TRACE_STR;
            tempStr     = logLevelStr;
            serialMsg   = CTLR_SERIAL_MSG_STR;
        }
        else if (sender->value == DIAG_LOG_VERB_STR) {
            // Log.begin(LOG_LEVEL_VERBOSE, &Serial);
            // Log.setShowLevel(false); // Do not show loglevel, we will do this in the prefix
            logLevelStr = DIAG_LOG_VERB_STR;
            tempStr     = logLevelStr;
            serialMsg   = CTLR_SERIAL_MSG_STR;
        }
        else {
            tempStr   = BAD_VALUE_STR;
            serialMsg = ERROR_MSG_STR;
            sprintf(logBuff, "diagLogCallback: %s.", BAD_VALUE_STR);
            Log.errorln(logBuff);
        }
        initSerialLog(false); // Set new log level.
        ESPUI.print(ctrlSerialMsgID, serialMsg);
        displaySaveWarning();
        sprintf(logBuff, "Serial Log Set to: %s.", tempStr.c_str());

        // Log.warningln(logBuff);
        tempStr  = "--:--:--:--- {SERIAL } ";
        tempStr += logBuff;
        Serial.println(tempStr); // Faux log message.
    }
    else {
        sprintf(logBuff, "diagLogCallback: %s.", BAD_SENDER_STR);
        Log.errorln(logBuff);
    }
}

// ************************************************************************************************
// gainAdjust(): Adjust Digital (USB) or Analog Input Gain. Sets String.
void gainAdjustCallback(Control *sender, int type)
{
    char logBuff[60];

    // sprintf(logBuff, "gainAdjustCallback ID: %d, Value: %s", sender->id, sender->value.c_str());
    // Log.verboseln(logBuff);

    if (sender->id == radioDgainID) {
        if (sender->value == DIG_GAIN0_STR) {
            digitalGainStr = DIG_GAIN0_STR;
            tempStr        = DIG_GAIN0_STR;
        }
        else if (sender->value == DIG_GAIN1_STR) {
            digitalGainStr = DIG_GAIN1_STR;
            tempStr        = DIG_GAIN1_STR;
        }
        else if (sender->value == DIG_GAIN2_STR) {
            digitalGainStr = DIG_GAIN2_STR;
            tempStr        = DIG_GAIN2_STR;
        }
        else {
            tempStr = BAD_VALUE_STR;
        }
        newDigGainFlg = true; // Tell Main Loop we Have New Setting to Update.
        displaySaveWarning();
        sprintf(logBuff, "Digital Gain Set to: %s.", tempStr.c_str());
        Log.infoln(logBuff);
    }
    else if (sender->id == radioVgaGainID) {
        if (sender->value == VGA_GAIN0_STR) {
            vgaGainStr = VGA_GAIN0_STR;
            tempStr    = VGA_GAIN0_STR;
        }
        else if (sender->value == VGA_GAIN1_STR) {
            vgaGainStr = VGA_GAIN1_STR;
            tempStr    = VGA_GAIN1_STR;
        }
        else if (sender->value == VGA_GAIN2_STR) {
            vgaGainStr = VGA_GAIN2_STR;
            tempStr    = VGA_GAIN2_STR;
        }
        else if (sender->value == VGA_GAIN3_STR) {
            vgaGainStr = VGA_GAIN3_STR;
            tempStr    = VGA_GAIN3_STR;
        }
        else if (sender->value == VGA_GAIN4_STR) {
            vgaGainStr = VGA_GAIN4_STR;
            tempStr    = VGA_GAIN4_STR;
        }
        else if (sender->value == VGA_GAIN5_STR) {
            vgaGainStr = VGA_GAIN5_STR;
            tempStr    = VGA_GAIN5_STR;
        }
        else {
            tempStr = BAD_VALUE_STR;
            sprintf(logBuff, "gainAdjust: %s.", BAD_VALUE_STR);
            Log.errorln(logBuff);
        }

        newVgaGainFlg = true; // Tell Main Loop we Have New Setting to Update.
        displaySaveWarning();

        tempStr  = getAudioGain();
        tempStr += " dB";
        ESPUI.print(radioGainID, tempStr);

        sprintf(logBuff, "Analog Input Gain Set to: %s.", tempStr.c_str());
        Log.infoln(logBuff);
    }
    else {
        sprintf(logBuff, "gainAdjust: %s.", BAD_SENDER_STR);
        Log.errorln(logBuff);
    }
}

// ************************************************************************************************
// gpioCallback(): GPIO Pin Configuration
void gpioCallback(Control *sender, int type)
{
    char logBuff[60];
    uint8_t gpioPin = 0;

    if (sender->id == gpio19ID) {
        newGpio19Flg = true; // Tell Main Loop we Have New Setting to Update.
        gpioPin      = 19;

        if (sender->value == GPIO_INP_FT_STR) {
            gpio19BootStr = GPIO_INP_FT_STR;
            tempStr       = GPIO_INP_FT_STR;
        }
        else if (sender->value == GPIO_INP_PD_STR) {
            gpio19BootStr = GPIO_INP_PD_STR;
            tempStr       = GPIO_INP_PD_STR;
        }
        else if (sender->value == GPIO_INP_PU_STR) {
            gpio19BootStr = GPIO_INP_PU_STR;
            tempStr       = GPIO_INP_PU_STR;
        }
        else if (sender->value == GPIO_OUT_LO_STR) {
            gpio19BootStr = GPIO_OUT_LO_STR;
            tempStr       = GPIO_OUT_LO_STR;
        }
        else if (sender->value == GPIO_OUT_HI_STR) {
            gpio19BootStr = GPIO_OUT_HI_STR;
            tempStr       = GPIO_OUT_HI_STR;
        }
        else {
            gpio19BootStr = GPIO_OUT_HI_STR;
            tempStr       = "UNKNOWN";
        }
        ESPUI.print(gpio19MsgID, " "); // Erase GPIO Message Posted by any Controller.
    }
    else if (sender->id == gpio23ID) {
        newGpio23Flg = true;           // Tell Main Loop we Have New Setting to Update.
        gpioPin      = 23;

        if (sender->value == GPIO_INP_FT_STR) {
            gpio23BootStr = GPIO_INP_FT_STR;
            tempStr       = GPIO_INP_FT_STR;
        }
        else if (sender->value == GPIO_INP_PD_STR) {
            gpio23BootStr = GPIO_INP_PD_STR;
            tempStr       = GPIO_INP_PD_STR;
        }
        else if (sender->value == GPIO_INP_PU_STR) {
            gpio23BootStr = GPIO_INP_PU_STR;
            tempStr       = GPIO_INP_PU_STR;
        }
        else if (sender->value == GPIO_OUT_LO_STR) {
            gpio23BootStr = GPIO_OUT_LO_STR;
            tempStr       = GPIO_OUT_LO_STR;
        }
        else if (sender->value == GPIO_OUT_HI_STR) {
            gpio23BootStr = GPIO_OUT_HI_STR;
            tempStr       = GPIO_OUT_HI_STR;
        }
        else {
            gpio23BootStr = GPIO_OUT_HI_STR;
            tempStr       = "UNKNOWN";
        }
        ESPUI.print(gpio23MsgID, " "); // Erase GPIO Message Posted by any Controller.
    }
    else if (sender->id == gpio33ID) {
        newGpio33Flg = true;           // Tell Main Loop we Have New Setting to Update.
        gpioPin      = 33;

        if (sender->value == GPIO_INP_FT_STR) {
            gpio33BootStr = GPIO_INP_FT_STR;
            tempStr       = GPIO_INP_FT_STR;
        }
        else if (sender->value == GPIO_INP_PD_STR) {
            gpio33BootStr = GPIO_INP_PD_STR;
            tempStr       = GPIO_INP_PD_STR;
        }
        else if (sender->value == GPIO_INP_PU_STR) {
            gpio33BootStr = GPIO_INP_PU_STR;
            tempStr       = GPIO_INP_PU_STR;
        }
        else if (sender->value == GPIO_OUT_LO_STR) {
            gpio33BootStr = GPIO_OUT_LO_STR;
            tempStr       = GPIO_OUT_LO_STR;
        }
        else if (sender->value == GPIO_OUT_HI_STR) {
            gpio33BootStr = GPIO_OUT_HI_STR;
            tempStr       = GPIO_OUT_HI_STR;
        }
        else {
            gpio33BootStr = GPIO_OUT_HI_STR;
            tempStr       = "UNKNOWN";
        }
        ESPUI.print(gpio33MsgID, " "); // Erase GPIO Message Posted by any Controller.
    }
    else {
        sprintf(logBuff, "gpioCallback: %s.", BAD_SENDER_STR);
        Log.errorln(logBuff);
        return;
    }

    if (gpioPin != 0) {
        setGpioBootPins(); // Update the GPIO Pins with New Pin Configuration.
        displaySaveWarning();
        sprintf(logBuff, "GPIO-%d Set to: %s.", gpioPin, tempStr.c_str());
        Log.infoln(logBuff);
    }
    else {
        tempStr = BAD_VALUE_STR;
        sprintf(logBuff, "gpioCallback: %s.", BAD_VALUE_STR);
        Log.errorln(logBuff);
    }
}

// ************************************************************************************************
// impAdjustCallback(): Adjust Audio Input Impedance. Sets String.
void impAdjustCallback(Control *sender, int type)
{
    char logBuff[60];

    // sprintf(logBuff, "impAdjustCallback ID: %d, Value: %s", sender->id, sender->value.c_str());
    // Log.verboseln(logBuff);

    if (sender->id == radioImpID) {
        if (sender->value == INP_IMP05K_STR) {
            inpImpedStr = INP_IMP05K_STR;
            tempStr     = INP_IMP05K_STR;
        }
        else if (sender->value == INP_IMP10K_STR) {
            inpImpedStr = INP_IMP10K_STR;
            tempStr     = INP_IMP10K_STR;
        }
        else if (sender->value == INP_IMP20K_STR) {
            inpImpedStr = INP_IMP20K_STR;
            tempStr     = INP_IMP20K_STR;
        }
        else if (sender->value == INP_IMP40K_STR) {
            inpImpedStr = INP_IMP40K_STR;
            tempStr     = INP_IMP40K_STR;
        }
        else {
            tempStr = BAD_VALUE_STR;
            sprintf(logBuff, "impAdjust: %s.", BAD_VALUE_STR);
            Log.errorln(logBuff);
        }
        newInpImpFlg = true; // Tell Main Loop we Have New Setting to Update.
        displaySaveWarning();

        tempStr  = getAudioGain();
        tempStr += " dB";
        ESPUI.print(radioGainID, tempStr);

        sprintf(logBuff, "Input Impedance Set to: %s.", tempStr.c_str());
        Log.infoln(logBuff);
    }
    else {
        sprintf(logBuff, "impAdjustCallback: %s.", BAD_SENDER_STR);
        Log.errorln(logBuff);
    }
}

// ************************************************************************************************
// muteCallback(): Turn audio on/off (true = Mute the audio).
void muteCallback(Control *sender, int type)
{
    char logBuff[60];

    // sprintf(logBuff, "muteCallback ID: %d, Value: %s", sender->id, sender->value.c_str());
    // Log.verboseln(logBuff);

    if (sender->id == adjMuteID) {
        if (type == S_ACTIVE) {
            ESPUI.setElementStyle(adjMuteID, "background: red;");
            muteFlg = true;
            tempStr = "On";
        }
        else if (type == S_INACTIVE) {
            ESPUI.setElementStyle(adjMuteID, "background: #bebebe;");
            muteFlg = false;
            tempStr = "Off";
        }
        else {
            tempStr = BAD_VALUE_STR;
            sprintf(logBuff, "muteCallback: %s.", BAD_VALUE_STR);
            Log.errorln(logBuff);
        }
        newMuteFlg = true; // Tell Main Loop we Have New Setting to Update.
        displaySaveWarning();
        sprintf(logBuff, "Audio Mute Set to: %s.", tempStr.c_str());
        Log.infoln(logBuff);
    }
    else {
        sprintf(logBuff, "muteCallback: %s.", BAD_SENDER_STR);
        Log.errorln(logBuff);
    }
}

// ************************************************************************************************
// radioEmphasisCallback(): Set Radio pre-emphasis to North America or European. Sets String.
void radioEmphasisCallback(Control *sender, int type)
{
    char logBuff[60];

    // sprintf(logBuff, "radioEmphasisCallback ID: %d, Value: %s", sender->id, sender->value.c_str());
    // Log.verboseln(logBuff);

    if (sender->id == radioEmphID) {
        if (sender->value == PRE_EMPH_USA_STR) {
            preEmphasisStr = PRE_EMPH_USA_STR;
            tempStr        = PRE_EMPH_USA_STR;
        }
        else if (sender->value == PRE_EMPH_EUR_STR) {
            preEmphasisStr = PRE_EMPH_EUR_STR;
            tempStr        = PRE_EMPH_EUR_STR;
        }
        else {
            // preEmphasisStr = PRE_EMPH_DEF_STR;
            tempStr = BAD_VALUE_STR;
            sprintf(logBuff, "radioEmphasisCallback: %s.", BAD_VALUE_STR);
            Log.errorln(logBuff);
        }
        newPreEmphFlg = true; // Tell Main Loop we Have New Setting to Update.
        displaySaveWarning();
        sprintf(logBuff, "Pre-Emphasis Set to: %s.", tempStr.c_str());
        Log.infoln(logBuff);
    }
    else {
        sprintf(logBuff, "radioEmphasisCallback: %s.", BAD_SENDER_STR);
        Log.errorln(logBuff);
    }
}

// ************************************************************************************************
// rdsDisplayTimeCallback(): Allow user to change the Local RDS RadioText display time.
void rdsDisplayTimeCallback(Control *sender, int type)
{
    uint16_t timerVal = 0;

    // char logBuff[60];
    // sprintf(logBuff, "rdsDisplayTimeCallback ID: %d, Value: %s", sender->id, sender->value.c_str());
    // Log.verboseln(logBuff);

    timerVal = sender->value.toInt();

    if (timerVal < RDS_DSP_TM_MIN) {
        timerVal = RDS_DSP_TM_MIN;
        ESPUI.print(rdsDspTmID, String(timerVal));
    }
    else if (timerVal > RDS_DSP_TM_MAX) {
        timerVal = RDS_DSP_TM_MAX;
        ESPUI.print(rdsDspTmID, String(timerVal));
    }
    displaySaveWarning();
    Log.infoln("Local RDS Message Time Set to: %u.", timerVal);
    rdsLocalMsgTime = ((uint32_t)(timerVal)) * 1000; // Convert Secs to mSecs.
}

// ************************************************************************************************
// rdsEnbCallback(): Enable/disable local RDS Messages 1-3.
//                   Add "Disabled" to homeTab's RDS field if all RDS controllers are turned off.
void rdsEnbCallback(Control *sender, int type)
{
    char logBuff[60];

    // sprintf(logBuff, "rdsEnbCallback ID: %d, Value: %s", sender->id, sender->value.c_str());
    // Log.verboseln(logBuff);

    if (sender->id == rdsEnb1ID) { // RDS RadioText #1 Enable.
        if (type == S_ACTIVE) {
            rdsText1EnbFlg = true;
            tempStr        = "On";
            ESPUI.print(rdsText1MsgID, RDS_LOCKED_STR);
            ESPUI.setEnabled(rdsText1ID, false); // Disable RadioText #1 Message Text Entry.
        }
        else if (type == S_INACTIVE) {           // RDS RadioText #1 Disable.
            rdsText1EnbFlg = false;
            tempStr        = "Off";
            ESPUI.print(rdsText1MsgID, "");
            ESPUI.setEnabled(rdsText1ID, true); // Enable RadioText #1 Message Text Entry.
        }
        else {
            tempStr = BAD_VALUE_STR;
            sprintf(logBuff, "rdsEnbCallback #1: %s.", BAD_VALUE_STR);
            Log.errorln(logBuff);
        }
        displaySaveWarning();
        sprintf(logBuff, "Local RDS #1 Enable Set to: %s.", tempStr.c_str());
        Log.infoln(logBuff);
    }
    else if (sender->id == rdsEnb2ID) { // RDS RadioText #2 Enable.
        if (type == S_ACTIVE) {
            rdsText2EnbFlg = true;
            tempStr        = "On";
            ESPUI.print(rdsText2MsgID, RDS_LOCKED_STR);
            ESPUI.setEnabled(rdsText2ID, false); // Disable RadioText #2 Message Text Entry.
        }
        else if (type == S_INACTIVE) {
            rdsText2EnbFlg = false;
            tempStr        = "Off";
            ESPUI.print(rdsText2MsgID, "");
            ESPUI.setEnabled(rdsText2ID, true); // Enable RadioText #2 Message Text Entry.
        }
        else {
            tempStr = BAD_VALUE_STR;
            sprintf(logBuff, "rdsEnbCallback #2: %s.", BAD_VALUE_STR);
            Log.errorln(logBuff);
        }
        displaySaveWarning();
        sprintf(logBuff, "Local RDS #2 Enable Set to: %s.", tempStr.c_str());
        Log.infoln(logBuff);
    }
    else if (sender->id == rdsEnb3ID) { // RDS RadioText #3 Enable.
        if (type == S_ACTIVE) {
            rdsText3EnbFlg = true;
            tempStr        = "On";
            ESPUI.print(rdsText3MsgID, RDS_LOCKED_STR);
            ESPUI.setEnabled(rdsText3ID, false); // Disable RadioText #3 Message Text Entry.
        }
        else if (type == S_INACTIVE) {
            rdsText3EnbFlg = false;
            tempStr        = "Off";
            ESPUI.print(rdsText3MsgID, "");
            ESPUI.setEnabled(rdsText3ID, true); // Enable RadioText #3 Message Text Entry.
        }
        else {
            tempStr = BAD_VALUE_STR;
            sprintf(logBuff, "rdsEnbCallback #3: %s.", BAD_VALUE_STR);
            Log.errorln(logBuff);
        }
        displaySaveWarning();
        sprintf(logBuff, "Local RDS #3 Enable Set to: %s.", tempStr.c_str());
        Log.infoln(logBuff);
    }
    else {
        sprintf(logBuff, "rdsEnbCallback: %s.", BAD_SENDER_STR);
        Log.errorln(logBuff);
    }

    displayRdsText();
}

// ************************************************************************************************
// rdsRstCallback(): Reset all RDS Fields to default values.
void rdsRstCallback(Control *sender, int type)
{
    // char logBuff[60];
    // sprintf(logBuff, "rdsResetCallback ID: %d, Value: %s", sender->id, sender->value.c_str());
    // Log.verboseln(logBuff);

    if ((sender->id == rdsRstID) && (type == B_DOWN)) {
        rdsText1EnbFlg = true;
        rdsText2EnbFlg = true;
        rdsText3EnbFlg = true;

        rdsTextMsg1Str = RDS_TEXT1_DEF_STR;
        ESPUI.print(rdsText1ID,    rdsTextMsg1Str);
        ESPUI.print(rdsText1MsgID, "");

        rdsTextMsg2Str = RDS_TEXT2_DEF_STR;
        ESPUI.print(rdsText2ID,    rdsTextMsg2Str);
        ESPUI.print(rdsText2MsgID, "");

        rdsTextMsg3Str = RDS_TEXT3_DEF_STR;
        ESPUI.print(rdsText3ID,    rdsTextMsg3Str);
        ESPUI.print(rdsText3MsgID, "");

        rdsLocalPsnStr = RDS_PSN_DEF_STR; // Program Service Name.
        ESPUI.print(rdsProgNameID, rdsLocalPsnStr);

        ESPUI.print(rdsEnbID,      "1");
        ESPUI.print(rdsEnb1ID,     "1");
        ESPUI.print(rdsEnb2ID,     "1");
        ESPUI.print(rdsEnb3ID,     "1");

        rdsLocalMsgTime = RDS_DSP_TM_DEF;
        rdsLocalPiCode  = RDS_PI_CODE_DEF;
        rdsLocalPtyCode = RDS_PTY_CODE_DEF;
        updateUiLocalMsgTime();
        updateUiLocalPiCode();
        updateUiLocalPtyCode();

        tempStr = String(RDS_DSP_TM_DEF / 1000);
        ESPUI.print(rdsDspTmID, tempStr);
        ESPUI.print(rdsRstID,   "RESET!");
        displaySaveWarning();

        Log.infoln("RDS Settings Have Been Reset to Default Values.");
    }
}

// ************************************************************************************************
// rdsTextCallback(): RDS RadioText Editing Handler. Provides Lock state (prevents edits) if RDS field is currently
// Enabled.
//                    Currently there are three RDS RadioTexts for local use.
void rdsTextCallback(Control *sender, int type)
{
    char logBuff[RDS_TEXT_MAX_SZ + 30];

    // sprintf(logBuff, "rdsTextCallback ID: %d, Value: %s", sender->id, sender->value.c_str());
    // Log.verboseln(logBuff);

    if (sender->id == rdsText1ID) {
        if (!rdsText1EnbFlg) {
            rdsTextMsg1Str = sender->value;
            rdsTextMsg1Str.trim();

            if (rdsTextMsg1Str.length() > RDS_TEXT_MAX_SZ) {
                rdsTextMsg1Str = rdsTextMsg1Str.substring(0, RDS_TEXT_MAX_SZ);
            }
            ESPUI.print(rdsText1ID,    rdsTextMsg1Str);
            ESPUI.print(rdsText1MsgID, "");
            displaySaveWarning();
            sprintf(logBuff, "RDS #1 RadioText Set to: \"%s\"", rdsTextMsg1Str.c_str());
            Log.infoln(logBuff);
        }
        else {
            ESPUI.print(rdsText1ID,    rdsTextMsg1Str);
            ESPUI.print(rdsText1MsgID, RDS_LOCKED_STR);
        }
    }
    else if (sender->id == rdsText2ID) {
        if (!rdsText2EnbFlg) {
            rdsTextMsg2Str = sender->value;
            rdsTextMsg2Str.trim();

            if (rdsTextMsg2Str.length() > RDS_TEXT_MAX_SZ) {
                rdsTextMsg2Str = rdsTextMsg2Str.substring(0, RDS_TEXT_MAX_SZ);
            }
            ESPUI.print(rdsText2ID,    rdsTextMsg2Str);
            ESPUI.print(rdsText2MsgID, "");
            displaySaveWarning();
            sprintf(logBuff, "RDS #2 RadioText Set to: \"%s\"", rdsTextMsg2Str.c_str());
            Log.infoln(logBuff);
        }
        else {
            ESPUI.print(rdsText2ID,    rdsTextMsg2Str);
            ESPUI.print(rdsText2MsgID, RDS_LOCKED_STR);
        }
    }
    else if (sender->id == rdsText3ID) {
        if (!rdsText3EnbFlg) {
            rdsTextMsg3Str = sender->value;
            rdsTextMsg3Str.trim(); \
            if (rdsTextMsg3Str.length() > RDS_TEXT_MAX_SZ) {
                rdsTextMsg3Str = rdsTextMsg3Str.substring(0, RDS_TEXT_MAX_SZ);
            }
            ESPUI.print(rdsText3ID,    rdsTextMsg3Str);
            ESPUI.print(rdsText3MsgID, "");
            displaySaveWarning();
            sprintf(logBuff, "RDS #3 RadioText Set to: \"%s\"", rdsTextMsg3Str.c_str());
            Log.infoln(logBuff);
        }
        else {
            ESPUI.print(rdsText3ID,    rdsTextMsg3Str);
            ESPUI.print(rdsText3MsgID, RDS_LOCKED_STR);
        }
    }
    else if (sender->id == rdsProgNameID) {
        rdsLocalPsnStr = sender->value;
        rdsLocalPsnStr.trim();

        if (rdsLocalPsnStr.length() > RDS_PSN_MAX_SZ) {
            rdsLocalPsnStr = rdsLocalPsnStr.substring(0, RDS_PSN_MAX_SZ);
        }
        ESPUI.print(rdsProgNameID, rdsLocalPsnStr);
        displaySaveWarning();
        sprintf(logBuff, "RDS Program Identification Set to: %s.", rdsLocalPsnStr.c_str());
        Log.infoln(logBuff);
    }
    else {
        sprintf(logBuff, "rdsTextCallback: %s.", BAD_SENDER_STR);
        Log.errorln(logBuff);
    }
}

// ************************************************************************************************
// rfAutoOff(): Control Sound Activated RF Carrier Shutdown Feature. Sets Boolean.

/*
   void rfAutoCallback(Control *sender, int type)
   {
    char logBuff[70];

    // sprintf(logBuff, "rdAutoCallback ID: %d, Value: %s", sender->id, sender->value.c_str());
    // Log.verboseln(logBuff);

    if (sender->id == radioAutoID) {
        if (sender->value == RF_AUTO_DIS_STR) {
            rfAutoFlg = false;
            tempStr   = RF_AUTO_DIS_STR;
        }
        else if (sender->value == RF_AUTO_ENB_STR) {
            rfAutoFlg = true;
            tempStr   = RF_AUTO_ENB_STR;
        }
        else {
            tempStr = BAD_VALUE_STR;
            sprintf(logBuff, "rfAutoOff: %s.", BAD_VALUE_STR);
            Log.errorln(logBuff);
        }
        newAutoRfFlg = true; // Tell Main Loop we Have New Setting to Update.
        displaySaveWarning();
        sprintf(logBuff, "RF Carrier Auto Shutdown Set to: %s.", tempStr.c_str());
        Log.infoln(logBuff);
    }
    else {
        sprintf(logBuff, "rfAutoOff: %s.", BAD_SENDER_STR);
        Log.errorln(logBuff);
    }
   }
 */

// ************************************************************************************************
// rfCarrierCallback(): Controls RF Carrier, On/Off.
void rfCarrierCallback(Control *sender, int type)
{
    char logBuff[70];

    // sprintf(logBuff, "rfCarrierCallback ID: %d, Value: %s", sender->id, sender->value.c_str());
    // Log.verboseln(logBuff);

    if (sender->id == radioRfEnbID) {
        switch (type) {
          case S_ACTIVE:
              rfCarrierFlg = true;

              if (fmRadioTestCode == FM_TEST_FAIL) {
                  Log.errorln("rfCarrierCallback: RADIO MODULE FAILURE!");
                  tempStr = "On, Warning: Radio Module Failure";
                  ESPUI.print(homeOnAirID, RADIO_FAIL_STR); // Update homeTab panel.
              }
              else if (fmRadioTestCode == FM_TEST_VSWR) {
                  Log.errorln("rfCarrierCallback: RADIO MODULE HAS HIGH VSWR!");
                  tempStr = "On, Warning: Radio Module RF-Out has High VSWR";
                  ESPUI.print(homeOnAirID, RADIO_VSWR_STR); // Update homeTab panel.
              }
              else if ((paVolts < PA_VOLT_MIN) || (paVolts > PA_VOLT_MAX)) {
                  Log.errorln("rfCarrierCallback: RF PA HAS INCORRECT VOLTAGE!");
                  tempStr = "On, Warning: RF PA has Incorrect Voltage";
                  ESPUI.print(homeOnAirID, RADIO_VOLT_STR); // Update homeTab panel.
              }
              else {
                  tempStr = "On";
                  ESPUI.print(homeOnAirID, RADIO_ON_AIR_STR); // Update homeTab panel.
              }
              break;
          case S_INACTIVE:
              rfCarrierFlg = false;
              tempStr      = "Off";
              ESPUI.print(homeOnAirID, RADIO_OFF_AIR_STR); // Update homeTab panel.
              break;
          default:
              tempStr = BAD_VALUE_STR;
              sprintf(logBuff, "rfCarrierCallback: %s.", BAD_VALUE_STR);
              Log.errorln(logBuff);
              break;
        }
        newCarrierFlg = true; // Tell Main Loop we Have New Setting to Update.
        displaySaveWarning();
        sprintf(logBuff, "RF Carrier Enable Set to: %s.", tempStr.c_str());
        Log.infoln(logBuff);
    }
    else {
        sprintf(logBuff, "rfCarrierCallback: %s.", BAD_SENDER_STR);
        Log.errorln(logBuff);
    }
}

// ************************************************************************************************
// rfPower(): Sets RF Power.
void rfPowerCallback(Control *sender, int type)
{
    char logBuff[60];

    // sprintf(logBuff, "rfPowerCallback ID: %d, Value: %s", sender->id, sender->value.c_str());
    // Log.verboseln(logBuff);

    if (sender->id == radioPwrID) {
        if (sender->value == RF_PWR_LOW_STR) {
            rfPowerStr = RF_PWR_LOW_STR;
            tempStr    = RF_PWR_LOW_STR;
        }
        else if (sender->value == RF_PWR_MED_STR) {
            rfPowerStr = RF_PWR_MED_STR;
            tempStr    = RF_PWR_MED_STR;
        }
        else if (sender->value == RF_PWR_HIGH_STR) {
            rfPowerStr = RF_PWR_HIGH_STR;
            tempStr    = RF_PWR_HIGH_STR;
        }
        else {
            sprintf(logBuff, "rfPower: %s.", BAD_VALUE_STR);
            Log.errorln(logBuff);
            tempStr = BAD_VALUE_STR;
        }
        newRfPowerFlg = true; // Tell Main Loop we Have New Setting to Update.
        displaySaveWarning();
        sprintf(logBuff, "RF Power Set to: %s.", tempStr.c_str());
        Log.infoln(logBuff);
    }
    else {
        sprintf(logBuff, "rfPower: %s.", BAD_SENDER_STR);
        Log.infoln(logBuff);
    }
}

// ************************************************************************************************
void saveSettingsCallback(Control *sender, int type)
{
    bool   saveSuccessFlg = true;
    char   logBuff[60];
    char   fileName[sizeof(BACKUP_FILE_NAME) + 1];
    String passMsg;

    passMsg  = "<p style=\"color:yellow ;\">";
    passMsg += BACKUP_SAV_PASS_STR;
    passMsg += "</p>";

    // sprintf(logBuff, "serialCallback ID: %d, Value: %s", sender->id, sender->value.c_str());
    // Log.verboseln(logBuff);

    if ((sender->id == adjSaveID)  ||
        (sender->id == ctrlSaveID) ||
        (sender->id == backupSaveSetID) ||
        (sender->id == gpioSaveID) ||
        (sender->id == radioSaveID) ||
        (sender->id == rdsSaveID)  ||
        (sender->id == wifiSaveID)) {
        switch (type) {
          case B_DOWN:
              Log.infoln("Saving Settings to File System ...");

              // SAVE SETTINGS HERE
              sprintf(fileName, "%s", BACKUP_FILE_NAME);
              saveSuccessFlg = saveConfiguration(LITTLEFS_MODE, fileName);

              if (saveSuccessFlg) {
                  ESPUI.print(adjSaveMsgID,       passMsg);
                  ESPUI.print(backupSaveSetMsgID, passMsg);
                  ESPUI.print(ctrlSaveMsgID,      passMsg);
                  ESPUI.print(gpioSaveMsgID,      passMsg);
                  ESPUI.print(radioSaveMsgID,     passMsg);
                  ESPUI.print(rdsSaveMsgID,       passMsg);
                  ESPUI.print(wifiSaveMsgID,      passMsg);
                  Log.infoln("-> Configuration Save Successful.");
              }
              else {
                  ESPUI.print(adjSaveMsgID,       BACKUP_SAV_FAIL_STR);
                  ESPUI.print(backupSaveSetMsgID, BACKUP_SAV_FAIL_STR);
                  ESPUI.print(ctrlSaveMsgID,      BACKUP_SAV_FAIL_STR);
                  ESPUI.print(gpioSaveMsgID,      BACKUP_SAV_FAIL_STR);
                  ESPUI.print(radioSaveMsgID,     BACKUP_SAV_FAIL_STR);
                  ESPUI.print(rdsSaveMsgID,       BACKUP_SAV_FAIL_STR);
                  ESPUI.print(wifiSaveMsgID,      BACKUP_SAV_FAIL_STR);
                  Log.errorln("-> Configuration Save Failed.");
              }
              break;
          case B_UP:
              break;
          default:
              sprintf(logBuff, "saveSettingsCallback: %s.", BAD_VALUE_STR);
              Log.errorln(logBuff);
              break;
        }
    }
    else {
        sprintf(logBuff, "saveSettingsCallback: %s.", BAD_SENDER_STR);
        Log.errorln(logBuff);
    }
}

// ************************************************************************************************
// serialCallback(): Set the Baud Rate for the Serial Controller. The "Off" setting disables the controller.
void serialCallback(Control *sender, int type)
{
    char   logBuff[60];
    String serialMsgStr;

    // sprintf(logBuff, "serialCallback ID: %d, Value: %s", sender->id, sender->value.c_str());
    // Log.verboseln(logBuff);

    if (sender->id == ctrlSerialID) {
        if (sender->value == SERIAL_OFF_STR) {
            // ctrlSerialFlg = false;
            serialMsgStr  = ""; // Erase warning msg on Diagnostic Serial Log panel.
            tempStr       = SERIAL_OFF_STR;
            ctrlSerialStr = sender->value;
            baudRate      = ESP_BAUD_DEF;
        }
        else if (sender->value == SERIAL_096_STR) {
            // ctrlSerialFlg = true;
            serialMsgStr  = DIAG_LOG_MSG_STR; // Warning msg on Diagnostic Serial Log panel.
            tempStr       = SERIAL_096_STR;
            ctrlSerialStr = sender->value;
            baudRate      = 9600;
        }
        else if (sender->value == SERIAL_192_STR) {
            // ctrlSerialFlg = true;
            serialMsgStr  = DIAG_LOG_MSG_STR;
            tempStr       = SERIAL_192_STR;
            ctrlSerialStr = sender->value;
            baudRate      = 19200;
        }
        else if (sender->value == SERIAL_576_STR) {
            // ctrlSerialFlg = true;
            serialMsgStr  = DIAG_LOG_MSG_STR;
            tempStr       = SERIAL_576_STR;
            ctrlSerialStr = sender->value;
            baudRate      = 57600;
        }
        else if (sender->value == SERIAL_115_STR) {
            // ctrlSerialFlg = true;
            serialMsgStr  = DIAG_LOG_MSG_STR;
            tempStr       = SERIAL_115_STR;
            ctrlSerialStr = sender->value;
            baudRate      = 115200;
        }
        else {
            tempStr      = BAD_VALUE_STR;
            serialMsgStr = ERROR_MSG_STR;
            sprintf(logBuff, "serialCallback: %s.", BAD_VALUE_STR);
            Log.errorln(logBuff);
        }

        ESPUI.print(diagLogMsgID, serialMsgStr); // Post warning message on Diagnostic Serial Log Panel.

        if (baudRate != 0) {
            Serial.end();                        // Flush all characters in queue.
            Serial.begin(baudRate);

            while (!Serial && !Serial.available()) {} // Wait for Serial Port to be available.
            Serial.println(); // Push out any corrupted data due to baud change.
        }
    }
    else {
        tempStr = BAD_SENDER_STR;
        sprintf(logBuff, "serialCallback: %s.", BAD_SENDER_STR);
        Log.errorln(logBuff);
    }
    displaySaveWarning();
    sprintf(logBuff, "Serial Controller Set to: %s.", tempStr.c_str());
    Log.infoln(logBuff);
    displayRdsText(); // Update RDS RadioText.
}

// ************************************************************************************************
// setLoginCallback(): Change Device Login Credentials.
void setLoginCallback(Control *sender, int type)
{
    char logBuff[USER_NM_MAX_SZ + 50];

    // sprintf(logBuff, "setLoginCallback ID: %d, Value: %s", sender->id, sender->value.c_str());
    // Log.verboseln(logBuff);

    if (sender->id == wifiDevUserID) {
        userNameStr = sender->value;
        userNameStr = userNameStr.substring(0, USER_NM_MAX_SZ);
        ESPUI.print(wifiDevUserID, userNameStr);
        displaySaveWarning();
        sprintf(logBuff, "Device Username Set to: \"%s\"", userNameStr.c_str());
    }
    else if (sender->id == wifiDevPwID) {
        userPassStr = sender->value;
        userPassStr = userPassStr.substring(0, USER_PW_MAX_SZ);
        ESPUI.print(wifiDevPwID, userPassStr);
        displaySaveWarning();
        sprintf(logBuff, "Device Password Set to: \"%s\"", userPassStr.c_str());
    }
    else {
        sprintf(logBuff, "setLoginCallback: %s.", BAD_SENDER_STR);
        Log.errorln(logBuff);
    }
    Log.infoln(logBuff);
}

// ************************************************************************************************
// setMqttAuthenticationCallback(): Update MQTT Broker Credentials (User Name & Password).
void setMqttAuthenticationCallback(Control *sender, int type)
{
    char   logBuff[MQTT_USER_MAX_SZ + 40];
    String shortHideStr = MQTT_PASS_HIDE_STR;

    // sprintf(logBuff, "setMqttAuthenticationCallback ID: %d, Value: %s", sender->id, sender->value.c_str());
    // Log.verboseln(logBuff);

    if (sender->id == ctrlMqttUserID) {
        mqttUserStr = sender->value;
        mqttUserStr = mqttUserStr.substring(0, MQTT_USER_MAX_SZ);
        ESPUI.print(ctrlMqttUserID, mqttUserStr);
        displaySaveWarning();
        sprintf(logBuff, "MQTT Broker Username Set to: \"%s\"", mqttUserStr.c_str());
        Log.infoln(logBuff);
    }
    else if (sender->id == ctrlMqttPwID) {
        tempStr      = sender->value;
        tempStr      = tempStr.substring(0, MQTT_PW_MAX_SZ);
        shortHideStr = shortHideStr.substring(0, 6); // Get first 5 chars of the "PW Hidden" text.

        if (tempStr.indexOf(shortHideStr) >= 0) {    // User has accidentally deleted part of PW Hidden msg.
            ESPUI.print(ctrlMqttPwID, MQTT_PASS_HIDE_STR);

            // sprintf(logBuff, "MQTT Broker Password Unchanged: \"%s\"", mqttPwStr.c_str()); // Show Password in log.
            sprintf(logBuff, "MQTT Broker Password Unchanged: %s", WIFI_PASS_HIDE_STR); // Hide Password in Log.
        }
        else {
            mqttPwStr = tempStr;
            ESPUI.print(ctrlMqttPwID, mqttPwStr);
            displaySaveWarning();
            sprintf(logBuff, "MQTT Broker Password Set to: \"%s\"", mqttPwStr.c_str());
        }
        Log.infoln(logBuff);
    }
    else {
        sprintf(logBuff, "setMqttAuthenticationCallback: %s.", BAD_SENDER_STR);
        Log.errorln(logBuff);
    }

    if (ctrlMqttFlg && ((!mqttIpStr.length()) || (!mqttNameStr.length()) ||
                        (!mqttUserStr.length()) || (!mqttPwStr.length()))) { // Missing MQTT Entries.
        ctrlMqttFlg = false;
        updateUiMqttMsg(MQTT_MISSING_STR);
        ESPUI.print(ctrlMqttID, "0");                                        // Turn off MQTT Controller Switcher.
        Log.errorln("setMqttAuthenticationCallback: Incomplete MQTT Settings, Disabled MQTT Broker");
    }
    else {
        updateUiMqttMsg(""); // Erase Warning Message.
    }
}

// ************************************************************************************************
// setMqttIpAddrCallback(): Update MQTT Broker IP Address.
#ifdef MQTT_ENB
void setMqttIpAddrCallback(Control *sender, int type)
{
    IPAddress tempAddr;
    char logBuff[60];

    // sprintf(logBuff, "setMqttIpAddrCallback ID: %d, Value: %s", sender->id, sender->value.c_str());
    // Log.verboseln(logBuff);

    if (sender->id == ctrlMqttIpID) {
        tempStr = sender->value;

        if ((tempStr.length() == 0)) {
            ctrlMqttFlg = false;
            mqttIpStr   = "";
            ESPUI.print(ctrlMqttID, "0"); // Missing IP Addreess, Turn Off MQTT Controller.
            Log.errorln("setMqttIpAddrCallback: Broker IP Address Erased. Disabled MQTT Controller.");
        }
        else {
            tempAddr = convertIpString(tempStr); // Convert to IP Class Array.

            if ((tempAddr[0] == 0) && (tempAddr[1] == 0) && (tempAddr[2] == 0) && (tempAddr[3] == 0)) {
                ESPUI.print(ctrlMqttIpID, mqttIpStr);
                Log.errorln("setMqttIpAddrCallback: Broker IP Invalid, Ignored.");
            }
            else {
                mqttIpStr = tempStr;                    // Copy, IP is valid.
                mqttIP    = convertIpString(mqttIpStr); // Convert IP to String.
                mqttIpStr = IpAddressToString(mqttIP);  // Convert the IP String back to IPAddress (reformat it).
                ESPUI.print(ctrlMqttIpID, mqttIpStr);
                displaySaveWarning();
                sprintf(logBuff, "MQTT Broker IP Set to: %s", mqttIpStr.c_str());
                Log.infoln(logBuff);
            }
        }
    }
    else {
        sprintf(logBuff, "setMqttIpAddrCallback: %s.", BAD_SENDER_STR);
        Log.errorln(logBuff);
    }
}

#endif // ifdef MQTT_ENB

// ************************************************************************************************
// setMqttNameCallback(): Update MQTT Device Name.
#ifdef MQTT_ENB
void setMqttNameCallback(Control *sender, int type)
{
    char logBuff[MQTT_NAME_MAX_SZ + 50];

    // sprintf(logBuff, "setMqttNameCallback ID: %d, Value: %s", sender->id, sender->value.c_str());
    // Log.verboseln(logBuff);

    if (sender->id == ctrlMqttNameID) {
        mqttNameStr = sender->value;
        mqttNameStr = mqttNameStr.substring(0, MQTT_NAME_MAX_SZ);

        if (mqttNameStr.length() == 0) {
            mqttNameStr = MQTT_NAME_DEF_STR;
        }
        ESPUI.print(ctrlMqttNameID, mqttNameStr);
        displaySaveWarning();
        sprintf(logBuff, "MQTT Device Name Set to: \"%s\"", mqttNameStr.c_str());
        Log.infoln(logBuff);
    }
    else {
        sprintf(logBuff, "setMqttNameCallback: %s.", BAD_SENDER_STR);
        Log.errorln(logBuff);
    }
}

#endif // ifdef MQTT_ENB

// ************************************************************************************************

void setPiCodeCallback(Control *sender, int type)
{
    char logBuff[50];
    char piBuff[10];
    uint32_t tempPiCode = 0;
    String   piStr;

    piStr.reserve(20);

    // sprintf(logBuff, "setPiCodeCallback ID: %d, Value: %s", sender->id, sender->value.c_str());
    // Log.verboseln(logBuff);

    if (sender->id == rdsPiID) {
        piStr = sender->value;
        piStr.trim();

        if (piStr.length() == 0) {                 // Missing Data Entry.
            tempPiCode = RDS_PI_CODE_DEF;          // Use default PI Code.
        }
        else if (piStr.length() > CMD_PI_MAX_SZ) { // Improperly Formatted Entry.
            tempPiCode = rdsLocalPiCode;           // Bad value, re-use old PI Code.
        }
        else {
            tempPiCode = strtol(piStr.c_str(), NULL, HEX);
        }

        if ((tempPiCode < RDS_PI_CODE_MIN) || (tempPiCode > RDS_PI_CODE_MAX)) { // Value Out of Range.
            tempPiCode = rdsLocalPiCode;                                        // Bad value, re-use old PI Code.
        }

        rdsLocalPiCode = tempPiCode;
        sprintf(piBuff, "0x%04X", rdsLocalPiCode);
        ESPUI.print(rdsPiID, piBuff);
        displaySaveWarning();

        sprintf(logBuff, "RDS PI Code Set to: \"%s\"", piBuff);
        Log.infoln(logBuff);
    }
    else {
        sprintf(logBuff, "setPiCodeCallback: %s.", BAD_SENDER_STR);
        Log.errorln(logBuff);
    }
}

// ************************************************************************************************

void setPtyCodeCallback(Control *sender, int type)
{
    char logBuff[50];
    char ptyBuff[10];
    int16_t tempPtyCode = 0;
    String  ptyStr;

    ptyStr.reserve(10);

    // sprintf(logBuff, "setPtyCodeCallback ID: %d, Value: %s", sender->id, sender->value.c_str());
    // Log.verboseln(logBuff);

    if (sender->id == rdsPtyID) {
        ptyStr = sender->value;
        ptyStr.trim();
        tempPtyCode = ptyStr.toInt();

        if ((tempPtyCode < RDS_PTY_CODE_MIN) ||
            (tempPtyCode > RDS_PTY_CODE_MAX) ||
            (ptyStr.length() == 0)) {
            tempPtyCode = rdsLocalPtyCode; // Error, Use old value.
            sprintf(ptyBuff, "%0u", rdsLocalPtyCode);
            ESPUI.print(rdsPtyID, ptyBuff);
            sprintf(logBuff, "setPtyCodeCallback: %s.", BAD_VALUE_STR);
            Log.errorln(logBuff);
            return;
        }

        rdsLocalPtyCode = tempPtyCode;
        displaySaveWarning();

        sprintf(logBuff, "RDS PTY Code Set to: %u", rdsLocalPtyCode);
        Log.infoln(logBuff);
    }
    else {
        sprintf(logBuff, "setPtyCodeCallback: %s.", BAD_SENDER_STR);
        Log.errorln(logBuff);
    }
}

// ************************************************************************************************
// setWiFiAddrsCallback(): Update WiFi Static IP and subnet (for Static IP). Also update AP (Hotspot) IP address.
void setWiFiAddrsCallback(Control *sender, int type)
{
    IPAddress tempApAddr;
    IPAddress tempDnsAddr;
    IPAddress tempGatewayAddr;
    IPAddress tempIpAddr;
    IPAddress tempSubAddr;
    char logBuff[60];

    // sprintf(logBuff, "setWiFiAddrsCallback ID: %d, Value: %s", sender->id, sender->value.c_str());
    // Log.verboseln(logBuff);

    if (sender->id == wifiIpID) {
        tempStr = sender->value;

        if ((tempStr.length() == 0)) {
            wifiDhcpFlg = true;
            staticIpStr = "";                // Erase Static IP.
            ESPUI.print(wifiDhcpID,    "1"); // Force to DHCP mode.
            ESPUI.print(wifiDhcpMsgID, DHCP_LOCKED_STR);
            Log.warningln("setWiFi: Static IP Address Erased. Must Use DHCP.");
        }
        else {
            tempIpAddr = convertIpString(tempStr); // Convert to IP Class Array.

            if ((tempIpAddr[0] == 0) && (tempIpAddr[1] == 0) && (tempIpAddr[2] == 0) && (tempIpAddr[3] == 0)) {
                ESPUI.print(wifiIpID, staticIpStr);
                Log.warningln("setWiFi: Static IP Address Invalid, Ignored.");
            }
            else {
                staticIpStr = tempStr;                      // Copy, IP is valid.
                staticIP    = convertIpString(staticIpStr); // Convert IP to String.
                staticIpStr = IpAddressToString(staticIP);  // Convert the IP String back to IPAddress (reformat it).
                ESPUI.print(wifiIpID, staticIpStr);
                displaySaveWarning();
                sprintf(logBuff, "IP Address Set to: %s", staticIpStr.c_str());
                Log.infoln(logBuff);

                tempGatewayAddr = convertIpString(wifiGatewayStr);    // Convert to IP Class Array.

                if ((tempGatewayAddr[0] == 0) && (tempGatewayAddr[1] == 0) && (tempGatewayAddr[2] == 0) && (tempGatewayAddr[3] == 0)) {
                    wifiGateway[0] = staticIP[0];                     // First Octet.
                    wifiGateway[1] = staticIP[1];
                    wifiGateway[2] = staticIP[2];
                    wifiGateway[3] = 1;                               // Empty Gateway = IP address with last octet set to 1.
                    wifiGatewayStr = IpAddressToString(wifiGateway);
                    wifiGateway    = convertIpString(wifiGatewayStr); // Reformat Gateway Address.
                    ESPUI.print(wifiGatewayID, wifiGatewayStr);
                }

                tempDnsAddr = convertIpString(wifiDnsStr);    // Convert to IP Class Array.

                if ((tempDnsAddr[0] == 0) && (tempDnsAddr[1] == 0) && (tempDnsAddr[2] == 0) && (tempDnsAddr[3] == 0)) {
                    wifiDNS[0] = staticIP[0];                 // First Octet.
                    wifiDNS[1] = staticIP[1];
                    wifiDNS[2] = staticIP[2];
                    wifiDNS[3] = 1;                           // Empty DNS = IP address with last octet set to 1.
                    wifiDnsStr = IpAddressToString(wifiDNS);
                    wifiDNS    = convertIpString(wifiDnsStr); // Reformat DNS Address.
                    ESPUI.print(wifiDnsID, wifiDnsStr);
                }

                if (wifiValidateSettings()) {
                    ESPUI.print(wifiDhcpMsgID, ""); // Erase DHCP Status Message.
                }
            }
        }
    }
    else if (sender->id == wifiGatewayID) {
        tempStr = sender->value;

        if ((tempStr.length() == 0)) {
            wifiDhcpFlg    = true;
            wifiGatewayStr = "";             // Erase Static IP.
            wifiGateway    = convertIpString("0.0.0.0");
            ESPUI.print(wifiDhcpID,    "1"); // Force to DHCP mode.
            ESPUI.print(wifiDhcpMsgID, DHCP_LOCKED_STR);
            Log.warningln("setWiFi: Gateway Address Erased. Must Use DHCP.");
        }
        else {
            tempGatewayAddr = convertIpString(tempStr); // Convert to IP Class Array.

            if ((tempGatewayAddr[0] == 0) && (tempGatewayAddr[1] == 0) && (tempGatewayAddr[2] == 0) && (tempGatewayAddr[3] == 0)) {
                ESPUI.print(wifiGatewayID, wifiGatewayStr);
                Log.warningln("setWiFi: Gateway Address Invalid, Ignored.");
            }
            else {
                wifiGatewayStr = tempStr;                         // Copy, IP is valid.
                wifiGateway    = convertIpString(wifiGatewayStr); // Convert IP to String.
                wifiGatewayStr = IpAddressToString(wifiGateway);  // Convert the Gateway String back to IPAddress (reformat it).
                ESPUI.print(wifiGatewayID, wifiGatewayStr);
                displaySaveWarning();
                sprintf(logBuff, "Gateway Address Set to: %s", wifiGatewayStr.c_str());
                Log.infoln(logBuff);
            }

            if (wifiValidateSettings()) {
                ESPUI.print(wifiDhcpMsgID, ""); // Erase DHCP Status Message.
            }
        }
    }
    else if (sender->id == wifiSubID) {
        tempStr = sender->value;

        if (tempStr.length() == 0) {
            wifiDhcpFlg = true;
            subNetStr   = "";                // Erase Static IP SubNet.
            ESPUI.print(wifiDhcpID,    "1"); // Force to DHCP mode.
            ESPUI.print(wifiDhcpMsgID, DHCP_LOCKED_STR);
            Log.warningln("setWiFi: Subnet Mask Erased. Must Use DHCP.");
        }
        else {
            tempSubAddr = convertIpString(tempStr); // Convert to IP Class Array.

            if ((tempSubAddr[0] == 0) && (tempSubAddr[1] == 0) && (tempSubAddr[2] == 0) && (tempSubAddr[3] == 0)) {
                ESPUI.print(wifiSubID, subNetStr);
                Log.warningln("setWiFi: Subnet Mask Invalid, Ignored.");
            }
            else {
                subNetStr = tempStr;                    // Copy, Subnet is valid.
                subNet    = convertIpString(subNetStr); // Convert IP to String.
                subNetStr = IpAddressToString(subNet);  // Convert the IP String back to IPAddress (reformat it).
                ESPUI.print(wifiSubID, subNetStr);
                displaySaveWarning();
                sprintf(logBuff, "Subnet Mask Set to: %s", subNetStr.c_str());
                Log.infoln(logBuff);

                if (wifiValidateSettings()) {
                    ESPUI.print(wifiDhcpMsgID, ""); // Erase DHCP Status Message.
                }
            }
        }
    }
    else if (sender->id == wifiDnsID) {
        tempStr = sender->value;

        if ((tempStr.length() == 0)) {
            wifiDhcpFlg = true;
            wifiDnsStr  = "";                // Erase Static IP.
            wifiDNS     = convertIpString("0.0.0.0");
            ESPUI.print(wifiDhcpID,    "1"); // Force to DHCP mode.
            ESPUI.print(wifiDhcpMsgID, DHCP_LOCKED_STR);
            Log.warningln("setWiFi: DNS Address Erased. Must Use DHCP.");
        }
        else {
            tempDnsAddr = convertIpString(tempStr); // Convert to IP Class Array.

            if ((tempDnsAddr[0] == 0) && (tempDnsAddr[1] == 0) && (tempDnsAddr[2] == 0) && (tempDnsAddr[3] == 0)) {
                ESPUI.print(wifiDnsID, wifiDnsStr);
                Log.warningln("setWiFi: DNS Address Invalid, Ignored.");
            }
            else {
                wifiDnsStr = tempStr;                     // Copy, IP is valid.
                wifiDNS    = convertIpString(wifiDnsStr); // Convert IP to String.
                wifiDnsStr = IpAddressToString(wifiDNS);  // Convert the DNS String back to IPAddress (reformat it).
                ESPUI.print(wifiDnsID, wifiDnsStr);
                displaySaveWarning();
                sprintf(logBuff, "Wifi DNS Address Set to: %s", wifiDnsStr.c_str());
                Log.infoln(logBuff);
            }

            if (wifiValidateSettings()) {
                ESPUI.print(wifiDhcpMsgID, ""); // Erase DHCP Status Message.
            }
        }
    }
    else if (sender->id == wifiApIpID) {
        tempStr    = sender->value;
        tempApAddr = convertIpString(tempStr); // Convert to IP Class Array.

        if ((tempApAddr[0] == 0) && (tempApAddr[1] == 0) && (tempApAddr[2] == 0) && (tempApAddr[3] == 0)) {
            ESPUI.print(wifiApIpID, apIpAddrStr);
            Log.warningln("setWiFi: HotSpot (AP) IP Address Invalid, Ignored.");
        }
        else {
            apIpAddrStr = tempStr;                      // Copy, IP is valid.
            hotSpotIP   = convertIpString(apIpAddrStr); // Convert IP to String.
            apIpAddrStr = IpAddressToString(hotSpotIP); // Convert the IP String back to IPAddress (reformat it).
            hotSpotIP   = tempApAddr;                   // Update Global AP IP Address for webserver.
            ESPUI.print(wifiApIpID, apIpAddrStr);
            displaySaveWarning();
            sprintf(logBuff, "HotSpot (AP) IP Address Set to: %s", apIpAddrStr.c_str());
            Log.infoln(logBuff);
        }
    }
    else {
        sprintf(logBuff, "setWiFiAddrsCallback: %s.", BAD_SENDER_STR);
        Log.errorln(logBuff);
    }
}

// ************************************************************************************************
// setWiFiAuthenticationCallback(): Update WiFi SSID and PW.
void setWiFiAuthenticationCallback(Control *sender, int type)
{
    char   logBuff[SSID_MAX_SZ + 40];
    String shortHideStr = WIFI_PASS_HIDE_STR;

    // sprintf(logBuff, "setWiFiAuthenticationCallback ID: %d, Value: %s", sender->id, sender->value.c_str());
    // Log.verboseln(logBuff);

    if (sender->id == wifiSsidID) {
        wifiSSIDStr = sender->value;

        if (wifiSSIDStr.length() > SSID_MAX_SZ) {
            wifiSSIDStr = wifiSSIDStr.substring(0, SSID_MAX_SZ);
            Log.errorln("setWiFiAuthenticationCallback: SSID Too Long, Truncated to 32 chars.");
        }
        ESPUI.print(wifiSsidID, wifiSSIDStr);
        displaySaveWarning();
        sprintf(logBuff, "WiFi SSID Set to: \"%s\"", wifiSSIDStr.c_str());
        Log.infoln(logBuff);
    }
    else if (sender->id == wifiWpaKeyID) {
        tempStr      = sender->value;
        tempStr      = tempStr.substring(0, PASSPHRASE_MAX_SZ);
        shortHideStr = shortHideStr.substring(0, 6);                            // First 5 chars of the "PW Hidden" text.

        if (tempStr.indexOf(shortHideStr) >= 0) {                               // Hmm, "Hidden" text msg is present.
            ESPUI.print(wifiWpaKeyID, WIFI_PASS_HIDE_STR);                      // Reprint "PW Hidden" msg.
            // sprintf(logBuff, "WiFi WPA Key Unchanged: \"%s\"", wifiWpaKeyStr.c_str());   // Show WPA Key in Log.
            sprintf(logBuff, "WiFi WPA Key Unchanged: %s", WIFI_PASS_HIDE_STR); // Hide WPA Key in Log.
        }
        else {                                                                  // New Password is available.
            wifiWpaKeyStr = tempStr;
            ESPUI.print(wifiWpaKeyID, wifiWpaKeyStr);
            displaySaveWarning();
            sprintf(logBuff, "WiFi WPA Key Set to: \"%s\"", wifiWpaKeyStr.c_str());
        }
        Log.infoln(logBuff);
    }
    else {
        sprintf(logBuff, "setWiFiAuthenticationCallback: %s.", BAD_SENDER_STR);
        Log.errorln(logBuff);
    }
}

// ************************************************************************************************
// setWiFiNamesCallback(): Update WiFi STA and AP names.
void setWiFiNamesCallback(Control *sender, int type)
{
    char logBuff[STA_NAME_MAX_SZ + 50];

    // sprintf(logBuff, "setWifiNamesCallback ID: %d, Value: %s", sender->id, sender->value.c_str());
    // Log.verboseln(logBuff);

    if (sender->id == wifiStaNameID) {
        staNameStr = sender->value;
        staNameStr = staNameStr.substring(0, STA_NAME_MAX_SZ);

        if (staNameStr.length() == 0) {
            staNameStr = STA_NAME_DEF_STR;
        }
        ESPUI.print(wifiStaNameID, staNameStr);
        sprintf(logBuff, "Webserver (STA) Name Set to: \"%s\"", staNameStr.c_str());
        Log.infoln(logBuff);
    }
    else if (sender->id == wifiApNameID) {
        apNameStr = sender->value;
        apNameStr = apNameStr.substring(0, AP_NAME_MAX_SZ);

        if (apNameStr.length() == 0) {
            apNameStr = AP_NAME_DEF_STR;
        }
        ESPUI.print(wifiApNameID, apNameStr);
        displaySaveWarning();
        sprintf(logBuff, "HotSpot (AP) Name Set to: \"%s\"", apNameStr.c_str());
        Log.infoln(logBuff);
    }
    else if (sender->id == wifiMdnsNameID) {
        mdnsNameStr = sender->value;
        mdnsNameStr = mdnsNameStr.substring(0, MDNS_NAME_MAX_SZ);

        if (mdnsNameStr.length() == 0) {
            mdnsNameStr = MDNS_NAME_DEF_STR;
        }
        ESPUI.print(wifiMdnsNameID, mdnsNameStr);
        displaySaveWarning();
        sprintf(logBuff, "MDNS/OTA Name Set to: \"%s\"", mdnsNameStr.c_str());
        Log.infoln(logBuff);
    }
    else {
        sprintf(logBuff, "setWiFiNamesCallback: %s.", BAD_SENDER_STR);
        Log.errorln(logBuff);
    }
}

// ************************************************************************************************
// testModeCallback(): Audio Test Tone Mode Control (true = Audio Test Mode On).
void testModeCallback(Control *sender, int type)
{
    char logBuff[60];

    // sprintf(logBuff, "testModeCallback: %d, Value: %s", sender->id, sender->value.c_str());
    // Log.verboseln(logBuff);

    if (sender->id == adjTestModeID) {
        if (type == S_ACTIVE) {
            ESPUI.setElementStyle(adjTestModeID, "background: red;");
            testModeFlg = true;
            tempStr     = "On";
            updateTestTones(true); // Reset Test Tone Elasped Timer.
        }
        else if (type == S_INACTIVE) {
            ESPUI.setElementStyle(adjTestModeID, "background: #bebebe;");
            testModeFlg = false;
            tempStr     = "Off";
        }
        else {
            tempStr = BAD_VALUE_STR;
            sprintf(logBuff, "testModeCallback: %s.", BAD_VALUE_STR);
            Log.errorln(logBuff);
        }
        sprintf(logBuff, "Test Mode Set to: %s.", tempStr.c_str());
        Log.infoln(logBuff);
    }
    else {
        sprintf(logBuff, "testModeCallback: %s.", BAD_SENDER_STR);
        Log.errorln(logBuff);
    }
}

// ************************************************************************************************
// volAdjustCallback(): Adjust USB or Analog Volume. Sets Int8_t.

/*
   void volAdjustCallback(Control *sender, int type)
   {
    char logBuff[60];
    int16_t limitLo, limitHi;

    // sprintf(logBuff, "volAdjustCallback ID: %d, Value: %s", sender->id, sender->value.c_str());
    // Log.verboseln(logBuff);

    if (sender->id == adjAvolID) {
        analogVol = sender->value.toInt();
        limitLo   = (String(ANA_VOL_MIN_STR)).toInt();
        limitHi   = (String(ANA_VOL_MAX_STR)).toInt();

        if (analogVol < limitLo) {
            analogVol = limitLo;
        }
        else if (analogVol > limitHi) {
            analogVol = limitHi;
        }
        displaySaveWarning();
        Log.infoln("Analog Volume Set to: %u", analogVol);
    }
    else if (sender->id == adjUvolID) {
        usbVol  = sender->value.toInt();
        limitLo = (String(USB_VOL_MIN_STR)).toInt();
        limitHi = (String(USB_VOL_MAX_STR)).toInt();

        if (usbVol < limitLo) {
            usbVol = limitLo;
        }
        else if (usbVol > limitHi) {
            usbVol = limitHi;
        }
        displaySaveWarning();
        Log.infoln("USB Volume Set to: %u", usbVol);
    }
    else {
        sprintf(logBuff, "volAdjustCallback: %s.", BAD_SENDER_STR);
        Log.errorln(logBuff);
    }
   }
 */
