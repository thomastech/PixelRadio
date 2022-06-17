/*
   File: commands.cpp
   Project: PixelRadio, an RBDS/RDS FM Transmitter (QN8027 Digital FM IC)
   Version: 1.1.0
   Creation: Dec-16-2021
   Revised:  Jun-13-2022
   Revision History: See PixelRadio.cpp
   Project Leader: T. Black (thomastech)
   Contributors: thomastech, dkulp

   (c) copyright T. Black 2021-2022, Licensed under GNU GPL 3.0 and later, under this
   license absolutely no warranty is given.
   This Code was formatted with the uncrustify extension.

   Note 1: The command functions are shared by the three remote controllers: Serial, MQTT, HTTP.

 */

// *************************************************************************************************************************
#include <ArduinoLog.h>
#include "config.h"
#include "PixelRadio.h"
#include "globals.h"
#include "language.h"
#include "QN8027Radio.h"


// *************************************************************************************************************************
// Radio
extern QN8027Radio radio;

// *************************************************************************************************************************
// getControllerStatus(): Returns Hex formatted value that represents which controllers are enabled and which are
// currently sending RadioText.
//  Bit D7: Serial Controller is Enabled.
//  Bit D6: MQTT Controller is Enabled.
//  Bit D5: HTTP Controller is Enabled.
//  Bit D4: Local Controller is Enabled.
//  Bit D3: Serial Controller is Sending RDS.
//  Bit D2: MQTT Controller is Sending RDS.
//  Bit D1: HTTP Controller is Sending RDS.
//  Bit D0: Local Controller is Sending RDS.
//
uint8_t getControllerStatus(void)
{
    uint8_t status = 0;

    if (ctrlSerialFlg()) {
        status = status | 0x80; // Set Bit D7.
    }

    if (ctrlMqttFlg) {
        status = status | 0x40; // Set Bit D6.
    }

    if (ctrlHttpFlg) {
        status = status | 0x20; // Set Bit D5.
    }

    if (checkLocalRdsAvail()) {
        status = status | 0x10; // Set Bit D4.
    }

    if (activeTextSerialFlg) {
        status = status | 0x08; // Set Bit D3.
    }

    if (activeTextMqttFlg) {
        status = status | 0x04; // Set Bit D2.
    }

    if (activeTextHttpFlg) {
        status = status | 0x02; // Set Bit D1.
    }

    if (activeTextLocalFlg) {
        status = status | 0x01; // Set Bit D0.
    }

    return status;
}

// *************************************************************************************************************************
// AudioModeCmd(): Set the Mono/Stereo Audio Mode using the Payload String. On exit, return true if success.
bool audioModeCmd(String payloadStr, uint8_t controller)
{
    char logBuff[100];
    const  uint8_t maxSize = CMD_AUD_MAX_SZ;
    String controllerStr;

    controllerStr = getControllerName(controller);

    if (controllerStr.length() == 0) {
        Log.errorln("-> audioModeCmd: Undefined Controller!");
        return false;
    }

    payloadStr.trim();
    payloadStr.toLowerCase();

    if (payloadStr.length() > maxSize) {
        payloadStr = payloadStr.substring(0, maxSize);
    }

    if (payloadStr == CMD_MODE_STER_STR) {
        stereoEnbFlg    = true;
        newAudioModeFlg = true;
        updateUiAudioMode();
    }
    else if (payloadStr == CMD_MODE_MONO_STR) {
        stereoEnbFlg    = false;
        newAudioModeFlg = true;
        updateUiAudioMode();
    }
    else {
        sprintf(logBuff, "-> %s Controller: Invalid Audio Mode Payload (%s), Ignored.", controllerStr.c_str(), payloadStr.c_str());
        Log.errorln(logBuff);
        return false;
    }

    payloadStr.toUpperCase();
    sprintf(logBuff, "-> %s Controller: Audio Mode Set to %s.", controllerStr.c_str(), payloadStr.c_str());
    Log.verboseln(logBuff);

    return true;
}

// *************************************************************************************************************************
bool frequencyCmd(String payloadStr, uint8_t controller)
{
    char logBuff[100];
    const  uint8_t maxSize = CMD_FREQ_MAX_SZ;
    int16_t freq;
    String  controllerStr;

    controllerStr = getControllerName(controller);

    if (controllerStr.length() == 0) {
        Log.errorln("-> frequencyCmd: Undefined Controller!");
        return false;
    }

    payloadStr.trim();

    if (payloadStr.length() > maxSize) {
        payloadStr = payloadStr.substring(0, maxSize);
    }

    freq = payloadStr.toInt();

    if ((!strIsUint(payloadStr)) ||
        (freq < FM_FREQ_MIN_X10) || (freq > FM_FREQ_MAX_X10)) {
        sprintf(logBuff, "-> %s Controller: Invalid Radio Frequency Payload (%s), Ignored.", controllerStr.c_str(), payloadStr.c_str());
        Log.errorln(logBuff);
        return false;
    }

    fmFreqX10  = freq;
    newFreqFlg = true;
    updateUiFrequency();
    sprintf(logBuff, "-> %s Controller: Transmit Frequency Set to %2.1fMhz.", controllerStr.c_str(), (float(fmFreqX10)) / 10.0f);
    Log.verboseln(logBuff);
    return true;
}

// *************************************************************************************************************************
// getControllerName(): Return the Controller's Name. If controller type is invalid then return empty String.
String getControllerName(uint8_t controller)
{
    String controllerStr;

    if (controller == SERIAL_CNTRL) {
        controllerStr = "Serial";
    }
    else if (controller == MQTT_CNTRL) {
        controllerStr = "MQTT";
    }
    else if (controller == HTTP_CNTRL) {
        controllerStr = "HTTP";
    }
    else if (controller == LOCAL_CNTRL) {
        controllerStr = "Local";
    }
    else {
        controllerStr = ""; // Return empty String if invalid controller type.
    }

    return controllerStr;
}

// *************************************************************************************************************************
// gpioCmd(): Read/Write the User's GPIO Pin States.  On exit, return true if success.
bool gpioCmd(String payloadStr, uint8_t controller, uint8_t pin)
{
    char logBuff[110];
    const  uint8_t maxSize = CMD_GPIO_MAX_SZ;
    String controllerStr;
    String gpioPinStr;

    payloadStr.trim();
    payloadStr.toLowerCase();

    controllerStr = getControllerName(controller);

    if (controllerStr.length() == 0) {
        Log.errorln("-> gpioCmd: Undefined Controller!");
        return false;
    }

    if (!((pin == GPIO19_PIN) || (pin == GPIO23_PIN) || (pin == GPIO33_PIN))) {
        sprintf(logBuff, "-> gpioCmd: %s Controller Used Invalid GPIO Pin (%d), Command Ignored.", controllerStr.c_str(), pin);
        Log.errorln(logBuff);
        return false;
    }

    if (payloadStr.length() > maxSize) {
        payloadStr = payloadStr.substring(0, maxSize);
    }

    if (payloadStr == CMD_GPIO_READ_STR) {
        sprintf(logBuff, "-> %s Controller: Read GPIO Pin-%d, Value= %d.", controllerStr.c_str(), pin, digitalRead(pin));
        Log.infoln(logBuff);
        return true;
    }
    else if (payloadStr == CMD_GPIO_OUT_HIGH_STR) {
        if (pin == GPIO19_PIN) {
            if ((gpio19BootStr == GPIO_OUT_HI_STR) || (gpio19BootStr == GPIO_OUT_LO_STR)) {
                digitalWrite(GPIO19_PIN, HIGH);
                gpio19CtrlStr = CMD_GPIO_OUT_HIGH_STR;
                sprintf(logBuff, "-> %s Controller: Setting GPIO Pin-%d to %s.", controllerStr.c_str(), pin, CMD_GPIO_OUT_HIGH_STR);
                Log.infoln(logBuff);
                updateUiGpioMsg(pin, controller);
            }
            else {
                sprintf(logBuff, "-> %s Controller: Cannot Write to GPIO Pin-%d (it is Input Pin).", controllerStr.c_str(), pin);
                Log.warningln(logBuff);
                return false;
            }
        }
        else if (pin == GPIO23_PIN) {
            if ((gpio23BootStr == GPIO_OUT_HI_STR) || (gpio23BootStr == GPIO_OUT_LO_STR)) {
                digitalWrite(GPIO23_PIN, HIGH);
                gpio23CtrlStr = CMD_GPIO_OUT_HIGH_STR;
                sprintf(logBuff, "-> %s Controller: Setting GPIO Pin-%d to %s.", controllerStr.c_str(), pin, CMD_GPIO_OUT_HIGH_STR);
                Log.infoln(logBuff);
                updateUiGpioMsg(pin, controller);
            }
            else {
                sprintf(logBuff, "-> %s Controller: Cannot Write to GPIO Pin-%d (it is Input Pin).", controllerStr.c_str(), pin);
                Log.warningln(logBuff);
                return false;
            }
        }
        else if (pin == GPIO33_PIN) {
            if ((gpio33BootStr == GPIO_OUT_HI_STR) || (gpio33BootStr == GPIO_OUT_LO_STR)) {
                digitalWrite(GPIO33_PIN, HIGH);
                gpio33CtrlStr = CMD_GPIO_OUT_HIGH_STR;
                sprintf(logBuff, "-> %s Controller: Setting GPIO Pin-%d to %s.", controllerStr.c_str(), pin, CMD_GPIO_OUT_HIGH_STR);
                Log.infoln(logBuff);
                updateUiGpioMsg(pin, controller);
            }
            else {
                sprintf(logBuff, "-> %s Controller: Cannot Write to GPIO Pin-%d (it is Input Pin).", controllerStr.c_str(), pin);
                Log.warningln(logBuff);
                return false;
            }
        }
    }
    else if (payloadStr == CMD_GPIO_OUT_LOW_STR) {
        if (pin == GPIO19_PIN) {
            if ((gpio19BootStr == GPIO_OUT_HI_STR) || (gpio19BootStr == GPIO_OUT_LO_STR)) {
                digitalWrite(GPIO19_PIN, LOW);
                gpio19CtrlStr = CMD_GPIO_OUT_LOW_STR;
                sprintf(logBuff, "-> %s Controller: Setting GPIO Pin-%d to %s.", controllerStr.c_str(), pin, CMD_GPIO_OUT_LOW_STR);
                Log.infoln(logBuff);
                updateUiGpioMsg(pin, controller);
            }
            else {
                sprintf(logBuff, "-> %s Controller: Cannot Write to GPIO Pin-%d (it is Input Pin).", controllerStr.c_str(), pin);
                Log.warningln(logBuff);
                return false;
            }
        }
        else if (pin == GPIO23_PIN) {
            if ((gpio23BootStr == GPIO_OUT_HI_STR) || (gpio23BootStr == GPIO_OUT_LO_STR)) {
                digitalWrite(GPIO23_PIN, LOW);
                gpio23CtrlStr = CMD_GPIO_OUT_LOW_STR;
                sprintf(logBuff, "-> %s Controller: Setting GPIO Pin-%d to %s.", controllerStr.c_str(), pin, CMD_GPIO_OUT_LOW_STR);
                Log.infoln(logBuff);
                updateUiGpioMsg(pin, controller);
            }
            else {
                sprintf(logBuff, "-> %s Controller: Cannot Write to GPIO Pin-%d (it is Input Pin).", controllerStr.c_str(), pin);
                Log.warningln(logBuff);
                return false;
            }
        }
        else if (pin == GPIO33_PIN) {
            if ((gpio33BootStr == GPIO_OUT_HI_STR) || (gpio33BootStr == GPIO_OUT_LO_STR)) {
                digitalWrite(GPIO33_PIN, LOW);
                gpio33CtrlStr = CMD_GPIO_OUT_LOW_STR;
                sprintf(logBuff, "-> %s Controller: Setting GPIO Pin-%d to %s.", controllerStr.c_str(), pin, CMD_GPIO_OUT_LOW_STR);
                Log.infoln(logBuff);
                updateUiGpioMsg(pin, controller);
            }
            else {
                sprintf(logBuff, "-> %s Controller: Cannot Write to GPIO Pin-%d (it is Input Pin).", controllerStr.c_str(), pin);
                Log.warningln(logBuff);
                return false;
            }
        }
    }
    else {
        sprintf(logBuff, "-> %s Controller: Invalid GPIO Payload (%s), Ignored.", controllerStr.c_str(), payloadStr.c_str());
        Log.errorln(logBuff);
        return false;
    }

    return true;
}

// *************************************************************************************************************************
bool infoCmd(String payloadStr, uint8_t controller)
{
    char logBuff[100];
    const  uint8_t maxSize = CMD_SYS_MAX_SZ;
    String controllerStr;

    controllerStr = getControllerName(controller);

    if (controllerStr.length() == 0) {
        Log.errorln("-> infoCmd: Undefined Controller!");
        return false;
    }

    payloadStr.trim();
    payloadStr.toLowerCase();

    if (payloadStr.length() > maxSize) {
        payloadStr = payloadStr.substring(0, maxSize);
    }

    if (payloadStr == CMD_SYS_CODE_STR) {
        sprintf(logBuff, "-> %s Controller: Info Command.", controllerStr.c_str());
        Log.verboseln(logBuff);
    }
    else {
        sprintf(logBuff, "-> %s Controller: Invalid INFO Payload (%s), Ignored.", controllerStr.c_str(), payloadStr.c_str());
        Log.errorln(logBuff);
        return false;
    }
    return true;
}

// *************************************************************************************************************************
// logCmd(): Set the Serial Log Level to Silent or reset back to system (Web UI) setting.
// This command is only used by the Serial Controller; The MQTT and HTTP controllers do not observe this command.
bool logCmd(String payloadStr, uint8_t controller)
{
    char logBuff[100];
    const  uint8_t maxSize = CMD_LOG_MAX_SZ;
    String controllerStr;

    controllerStr = getControllerName(controller);

    if (controllerStr.length() == 0) {
        Log.errorln("-> logCmd: Undefined Controller!");
        return false;
    }

    payloadStr.trim();
    payloadStr.toLowerCase();

    if (payloadStr.length() > maxSize) {
        payloadStr = payloadStr.substring(0, maxSize);
    }

    if (payloadStr == CMD_LOG_SIL_STR) {
        sprintf(logBuff, "-> %s Controller: Log Level Set to LOG_LEVEL_SILENT.", controllerStr.c_str());
        Log.verboseln(logBuff);
        Serial.flush();
        Log.begin(LOG_LEVEL_SILENT, &Serial);
        Log.setShowLevel(false); // Do not show loglevel, we will do this in the prefix
    }
    else if (payloadStr == CMD_LOG_RST_STR) {
        sprintf(logBuff, "-> %s Controller: Log Level Restored to %s.", controllerStr.c_str(), logLevelStr.c_str());
        Log.verboseln(logBuff);
        Serial.flush();
        Log.begin(getLogLevel(), &Serial);
        Log.setShowLevel(false); // Do not show loglevel, we will do this in the prefix
    }
    else {
        sprintf(logBuff, "-> %s Controller: Invalid LOG Payload (%s), Ignored.", controllerStr.c_str(), payloadStr.c_str());
        Log.errorln(logBuff);
        return false;
    }
    Serial.flush();

    return true;
}

// *************************************************************************************************************************
bool muteCmd(String payloadStr, uint8_t controller)
{
    char logBuff[100];
    const  uint8_t maxSize = CMD_MUTE_MAX_SZ;
    String controllerStr;

    controllerStr = getControllerName(controller);

    if (controllerStr.length() == 0) {
        Log.errorln("-> muteCmd: Undefined Controller!");
        return false;
    }

    payloadStr.trim();
    payloadStr.toLowerCase();

    if (payloadStr.length() > maxSize) {
        payloadStr = payloadStr.substring(0, maxSize);
    }

    if (payloadStr == CMD_MUTE_ON_STR) {
        sprintf(logBuff, "-> %s Controller: Mute Set to ON (Audio Off).", controllerStr.c_str());
        Log.verboseln(logBuff);
        muteFlg    = true;
        newMuteFlg = true;
        updateUiAudioMute();
    }
    else if (payloadStr == CMD_MUTE_OFF_STR) {
        sprintf(logBuff, "-> %s Controller: Mute Set to OFF (Audio On).", controllerStr.c_str());
        Log.verboseln(logBuff);
        muteFlg    = false;
        newMuteFlg = true;
        updateUiAudioMute();
    }
    else {
        sprintf(logBuff, "-> %s Controller: Invalid MUTE Payload (%s), Ignored.", controllerStr.c_str(), payloadStr.c_str());
        Log.errorln(logBuff);
        return false;
    }
    return true;
}

// *************************************************************************************************************************
bool piCodeCmd(String payloadStr, uint8_t controller)
{
    char logBuff[100];
    const  uint8_t maxSize = CMD_PI_MAX_SZ;
    uint32_t tempPiCode;
    String   controllerStr;

    controllerStr = getControllerName(controller);

    if (controllerStr.length() == 0) {
        Log.errorln("-> piCodeCmd: Undefined Controller!");
        return false;
    }

    payloadStr.trim();

    if (payloadStr.length() > maxSize) {
        payloadStr = payloadStr.substring(0, maxSize);
    }

    tempPiCode = strtol(payloadStr.c_str(), NULL, HEX);

    if ((tempPiCode < RDS_PI_CODE_MIN) || (tempPiCode > RDS_PI_CODE_MAX)) {
        sprintf(logBuff, "-> %s Controller: Invalid RDS PI Code Value (%s), Ignored.", controllerStr.c_str(), payloadStr.c_str());
        Log.errorln(logBuff);
        return false;
    }
    else {
        if (radio.getPiCode() != (uint16_t)(tempPiCode)) { // New PI Code.
            if (controller == SERIAL_CNTRL) {
                rdsSerialPiCode = tempPiCode;
                textSerialFlg   = true;                    // Reload Serial RDS values
            }
            else if (controller == MQTT_CNTRL) {
                rdsMqttPiCode = tempPiCode;
                textMqttFlg   = true; // Reload MQTT RDS values
            }
            else if (controller == HTTP_CNTRL) {
                rdsHttpPiCode = tempPiCode;
                textHttpFlg   = true; // Reload HTTP RDS values
            }

            displaySaveWarning();
            sprintf(logBuff, "-> %s Controller: PI Code Set to 0x%04X.", controllerStr.c_str(), tempPiCode);
        }
        else {
            sprintf(logBuff, "-> %s Controller: PI Code Unchanged (0x%04X).", controllerStr.c_str(), tempPiCode);
        }
        Log.verboseln(logBuff);
    }
    return true;
}

// *************************************************************************************************************************
bool ptyCodeCmd(String payloadStr, uint8_t controller)
{
    char logBuff[100];
    const  uint8_t maxSize = CMD_PTY_MAX_SZ;
    int16_t tempPtyCode;
    String  controllerStr;

    controllerStr = getControllerName(controller);

    if (controllerStr.length() == 0) {
        Log.errorln("-> ptyCodeCmd: Undefined Controller!");
        return false;
    }

    payloadStr.trim();

    if (payloadStr.length() > maxSize) {
        payloadStr = payloadStr.substring(0, maxSize);
    }
    tempPtyCode = payloadStr.toInt();

    if ((strIsUint(payloadStr) == false) ||
        (tempPtyCode < RDS_PTY_CODE_MIN) ||
        (tempPtyCode > RDS_PTY_CODE_MAX)) {
        sprintf(logBuff, "-> %s Controller: Invalid RDS PTY Code Value (%s), Ignored.", controllerStr.c_str(), payloadStr.c_str());
        Log.errorln(logBuff);
        return false;
    }
    else {
        if (radio.getPTYCode() != (uint8_t)(tempPtyCode)) { // New PTY Code.
            if (controller == SERIAL_CNTRL) {
                rdsSerialPtyCode = (uint8_t)(tempPtyCode);
                textSerialFlg    = true;                    // Reload Serial RDS values
            }
            else if (controller == MQTT_CNTRL) {
                rdsMqttPtyCode = (uint8_t)(tempPtyCode);
                textMqttFlg    = true; // Reload MQTT RDS values
            }
            else if (controller == HTTP_CNTRL) {
                rdsHttpPtyCode = (uint8_t)(tempPtyCode);
                textHttpFlg    = true; // Reload HTTP RDS values
            }

            displaySaveWarning();
            sprintf(logBuff, "-> %s Controller: PTY Code Set to %d.", controllerStr.c_str(), tempPtyCode);
        }
        else {
            sprintf(logBuff, "-> %s Controller: PTY Code Unchanged (%d).", controllerStr.c_str(), tempPtyCode);
        }
        Log.verboseln(logBuff);
    }
    return true;
}

// *************************************************************************************************************************
bool programServiceNameCmd(String payloadStr, uint8_t controller)
{
    char logBuff[100];
    const  uint8_t maxSize = CMD_PSN_MAX_SZ;
    String controllerStr;

    controllerStr = getControllerName(controller);

    if (controllerStr.length() == 0) {
        Log.errorln("-> programServiceNameCmd: Undefined Controller!");
        return false;
    }

    payloadStr.trim();

    if (payloadStr.length() > maxSize) {
        payloadStr = payloadStr.substring(0, maxSize);
    }

    if (controller == SERIAL_CNTRL) {
        rdsSerialPsnStr = payloadStr;
        textSerialFlg   = true; // Reload Serial RDS values
    }
    else if (controller == MQTT_CNTRL) {
        rdsMqttPsnStr = payloadStr;
        textMqttFlg   = true; // Reload MQTT RDS values
    }
    else if (controller == HTTP_CNTRL) {
        rdsHttpPsnStr = payloadStr;
        textHttpFlg   = true; // Reload HTTP RDS values
    }

    sprintf(logBuff, "-> %s Controller: RDS PSN Set to %s", controllerStr.c_str(), payloadStr.c_str());
    Log.verboseln(logBuff);
    return true;
}

// *************************************************************************************************************************
bool radioTextCmd(String payloadStr, uint8_t controller)
{
    char logBuff[100 + CMD_RT_MAX_SZ];
    const  uint8_t maxSize = CMD_RT_MAX_SZ;
    String controllerStr;

    controllerStr = getControllerName(controller);

    if (controllerStr.length() == 0) {
        Log.errorln("-> radioTextCmd: Undefined Controller!");
        return false;
    }

    payloadStr.trim();

    if (payloadStr.length() > maxSize) {
        payloadStr = payloadStr.substring(0, maxSize);
    }

    if (controller == SERIAL_CNTRL) {
        textSerialFlg    = true; // Reload Serial RDS values
        rdsSerialTextStr = payloadStr;
    }
    else if (controller == MQTT_CNTRL) {
        textMqttFlg    = true; // Reload MQTT RDS values
        rdsMqttTextStr = payloadStr;
    }
    else if (controller == HTTP_CNTRL) {
        textHttpFlg    = true; // Reload HTTP RDS values
        rdsHttpTextStr = payloadStr;
    }

    sprintf(logBuff, "-> %s Controller: RadioText Changed to %s", controllerStr.c_str(), payloadStr.c_str());
    Log.verboseln(logBuff);
    return true;
}

// *************************************************************************************************************************
// rdsTimePeriodCmd(): Set the RadioText Message Display Time. Input value is in seconds.
bool rdsTimePeriodCmd(String payloadStr, uint8_t controller)
{
    bool capFlg = false;
    char logBuff[100];
    int32_t rtTime = 0;
    String  controllerStr;

    controllerStr = getControllerName(controller);

    if (controllerStr.length() == 0) {
        Log.errorln("-> rdsTimePeriodCmd: Undefined Controller!");
        return false;
    }

    payloadStr.trim();
    rtTime = strtol(payloadStr.c_str(), NULL, 10);

    if ((payloadStr.length() > CMD_TIME_MAX_SZ) || (rtTime <= 0)) {
        Log.errorln("-> RDS Time Period Value is Invalid, Ignored.");
        return false;
    }
    else if (rtTime > RDS_DSP_TM_MAX) {
        capFlg = true;
        rtTime = RDS_DSP_TM_MAX;
    }
    else if (rtTime < RDS_DSP_TM_MIN) {
        capFlg = true;
        rtTime = RDS_DSP_TM_MIN;
    }

    if (controller == SERIAL_CNTRL) {
        textSerialFlg    = true; // Restart Serial Controller's RDS.
        rdsSerialMsgTime = rtTime * 1000;
    }
    else if (controller == MQTT_CNTRL) {
        textMqttFlg    = true; // Restart MQTT Controller's RDS.
        rdsMqttMsgTime = rtTime * 1000;
    }
    else if (controller == HTTP_CNTRL) {
        textHttpFlg    = true; // Restart HTTP Controller's RDS.
        rdsHttpMsgTime = rtTime * 1000;
    }

    if (capFlg) {
        sprintf(logBuff, "-> %s Controller: RDS Time Period Value out-of-range, set to %d secs.", controllerStr.c_str(), rtTime);
    }
    else {
        sprintf(logBuff, "-> %s Controller: RDS Time Period Set to %d Secs.", controllerStr.c_str(), rtTime);
    }
    Log.verboseln(logBuff);

    return true;
}

// *************************************************************************************************************************
bool rebootCmd(String payloadStr, uint8_t controller)
{
    char logBuff[100];
    const  uint8_t maxSize = CMD_SYS_MAX_SZ;
    String controllerStr;

    controllerStr = getControllerName(controller);

    if (controllerStr.length() == 0) {
        Log.errorln("-> rebootCmd: Undefined Controller!");
        return false;
    }

    payloadStr.trim();
    payloadStr.toLowerCase();

    if (payloadStr.length() > maxSize) {
        payloadStr = payloadStr.substring(0, maxSize);
    }

    if (payloadStr == CMD_SYS_CODE_STR) {
        sprintf(logBuff, "-> %s Controller: System Rebooted.", controllerStr.c_str());
        Log.verboseln(logBuff);
        rebootFlg = true; // Request system reboot.
    }
    else {
        sprintf(logBuff, "-> %s Controller: Invalid REBOOT Payload (%s), Ignored.", controllerStr.c_str(), payloadStr.c_str());
        Log.errorln(logBuff);
        return false;
    }
    return true;
}

// *************************************************************************************************************************
bool rfCarrierCmd(String payloadStr, uint8_t controller)
{
    char logBuff[100];
    const  uint8_t maxSize = CMD_RF_MAX_SZ;
    String controllerStr;

    controllerStr = getControllerName(controller);

    if (controllerStr.length() == 0) {
        Log.errorln("-> rfCarrierCmd: Undefined Controller!");
        return false;
    }

    payloadStr.trim();
    payloadStr.toLowerCase();

    if (payloadStr.length() > maxSize) {
        payloadStr = payloadStr.substring(0, maxSize);
    }

    if (payloadStr == CMD_RF_ON_STR) {
        sprintf(logBuff, "-> %s Controller: RF Carrier Set to ON.", controllerStr.c_str());
        Log.verboseln(logBuff);
        rfCarrierFlg  = true;
        newCarrierFlg = true;
        updateUiRfCarrier();
    }
    else if (payloadStr == CMD_RF_OFF_STR) {
        sprintf(logBuff, "-> %s Controller: RF Carrier Set to OFF.", controllerStr.c_str());
        Log.verboseln(logBuff);
        rfCarrierFlg  = false;
        newCarrierFlg = true;
        updateUiRfCarrier();
    }
    else {
        sprintf(logBuff, "-> %s Controller: Invalid RF Carrier Payload (%s), Ignored.", controllerStr.c_str(), payloadStr.c_str());
        Log.errorln(logBuff);
        return false;
    }

    return true;
}

// *************************************************************************************************************************
bool startCmd(String payloadStr, uint8_t controller)
{
    char logBuff[100];
    const  uint8_t maxSize = CMD_RDS_MAX_SZ;
    String controllerStr;

    controllerStr = getControllerName(controller);

    if (controllerStr.length() == 0) {
        Log.errorln("-> startCmd: Undefined Controller!");
        return false;
    }

    payloadStr.trim();
    payloadStr.toLowerCase();

    if (payloadStr.length() > maxSize) {
        payloadStr = payloadStr.substring(0, maxSize);
    }

    if (payloadStr == CMD_RDS_CODE_STR) {
        sprintf(logBuff, "-> %s Controller: Start RDS.", controllerStr.c_str());
        Log.verboseln(logBuff);

        if (controller == SERIAL_CNTRL) {
            textSerialFlg = true; // Restart Serial Controller's RadioText.
        }
        else if (controller == MQTT_CNTRL) {
            textMqttFlg = true;   // Restart MQTT Controller's RadioText.
        }
        else if (controller == HTTP_CNTRL) {
            textHttpFlg = true;   // Restart HTTP Controller's RadioText.
        }
    }
    else {
        sprintf(logBuff, "-> %s Controller: Invalid START Payload (%s), Ignored.", controllerStr.c_str(), payloadStr.c_str());
        Log.errorln(logBuff);
        return false;
    }
    return true;
}

// *************************************************************************************************************************
bool stopCmd(String payloadStr, uint8_t controller)
{
    char logBuff[100];
    const  uint8_t maxSize = CMD_RDS_MAX_SZ;
    String controllerStr;

    controllerStr = getControllerName(controller);

    if (controllerStr.length() == 0) {
        Log.errorln("-> stopCmd: Undefined Controller!");
        return false;
    }

    payloadStr.trim();
    payloadStr.toLowerCase();

    if (payloadStr.length() > maxSize) {
        payloadStr = payloadStr.substring(0, maxSize);
    }

    if (payloadStr == CMD_RDS_CODE_STR) {
        sprintf(logBuff, "-> %s Controller: Stop RDS.", controllerStr.c_str());
        Log.verboseln(logBuff);

        if (controller == SERIAL_CNTRL) {
            stopSerialFlg = true; // Restart Serial Controller's RadioText.
        }
        else if (controller == MQTT_CNTRL) {
            stopMqttFlg = true;   // Restart MQTT Controller's RadioText.
        }
        else if (controller == HTTP_CNTRL) {
            stopHttpFlg = true;   // Restart HTTP Controller's RadioText.
        }
    }
    else {
        sprintf(logBuff, "-> %s Controller: Invalid STOP Payload (%s), Ignored.", controllerStr.c_str(), payloadStr.c_str());
        Log.errorln(logBuff);
        return false;
    }
    return true;
}

// *************************************************************************************************************************
// EOF
