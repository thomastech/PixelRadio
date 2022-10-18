/*
   File: mqtt.cpp
   Project: PixelRadio, an RBDS/RDS FM Transmitter (QN8027 Digital FM IC)
   Version: 1.1.2
   Creation: Dec-16-2021
   Revised:  Oct-18-2022
   Revision History: See PixelRadio.cpp
   Project Leader: T. Black (thomastech)
   Contributors: thomastech, dkulp

   (c) copyright T. Black 2021-2022, Licensed under GNU GPL 3.0 and later, under this license absolutely no warranty is given.
   This Code was formatted with the uncrustify extension.

   Note 1: MQTT is disabled in AP (HotSpot) mode. Must be connected in STA mode.
 */

// *************************************************************************************************************************
#include <ArduinoLog.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include "config.h"
#include "PixelRadio.h"
#include "credentials.h" // User supplied file. Contains WiFi and MQTT broker credentials.
#include "globals.h"
#include "language.h"

// *************************************************************************************************************************
#ifdef MQTT_ENB // This file is excluded if MQTT not defined in config.h

// *************************************************************************************************************************


// WiFi and MQTT Classes
WiFiClient wifiClient;

// IPAddress    mqttServer(mqttIP); // IP of OpenHabian MQTT server.
PubSubClient mqttClient(wifiClient);


// *************************************************************************************************************************
// gpioMqttControl(): MQTT handler for GPIO Commands. This is a companion to mqttCallback().
void gpioMqttControl(String payloadStr, uint8_t pin)
{
    bool   successFlg = true;
    char   charBuff[60];
    String topicStr;

    topicStr = mqttNameStr + MQTT_GPIO_STR;

    sprintf(charBuff, "-> MQTT Controller: Received GPIO Pin-%d Command", pin);
    Log.infoln(charBuff);

    successFlg = gpioCmd(payloadStr, MQTT_CNTRL, pin);

    if (!successFlg) {
        sprintf(charBuff, "{\"%s%d\": \"fail\"}", CMD_GPIO_STR, pin);
    }
    else if (payloadStr == CMD_GPIO_READ_STR) {
        sprintf(charBuff, "{\"%s%d\": \"%d\"}", CMD_GPIO_STR, pin, digitalRead(pin));
    }
    else {
        sprintf(charBuff, "{\"%s%d\": \"ok\"}", CMD_GPIO_STR, pin);
    }
    mqttClient.publish(topicStr.c_str(), charBuff);
}

// ************************************************************************************************
// makeMqttCmdStr(): Return the MQTT command string. On entry cmdStr has command keyword.
//                   This is a companion to mqttCallback().
String makeMqttCmdStr(String cmdStr) {
    cmdStr = String(MQTT_CMD_STR) + cmdStr;

    return cmdStr;
}

// *************************************************************************************************************************
// mqttCallback(): Support function for MQTT message reception.
// mqttCallback is limited to 255 byte packets (topic size + payload size). If larger, topic is corrupted and
// mqttCallback will not be processed. This is a limitation of the PubSubClient library.
static void mqttCallback(const char *topic, byte *payload, unsigned int length)
{
    char   cBuff[length + 2];                    // Allocate Character buffer.
    char   logBuff[length + strlen(topic) + 60]; // Allocate a big buffer space.
    char   mqttBuff[140 + sizeof(VERSION_STR) + STA_NAME_MAX_SZ];
    String payloadStr;
    String topicStr;

    payloadStr.reserve(MQTT_PAYLD_MAX_SZ + 1);
    topicStr.reserve(MQTT_TOPIC_MAX_SZ + 1);

    if (length > MQTT_PAYLD_MAX_SZ) {
        Log.warningln("MQTT Message Length (%u bytes) too long! Truncated to %u bytes.", length, MQTT_PAYLD_MAX_SZ);
        length = MQTT_PAYLD_MAX_SZ;
    }

    // Log.verboseln("MQTT Message Length: %u", length);

    // Copy payload to local char array.
    for (unsigned int i = 0; i < length; i++) {
        cBuff[i] = ((char)payload[i]);
    }
    cBuff[length] = 0;

    payloadStr = cBuff;
    payloadStr.trim();      // Remove leading/trailing spaces. Do NOT change to lowercase!

    topicStr = topic;
    topicStr.trim();        // Remove leading/trailing spaces.
    topicStr.toLowerCase(); // Force lowercase, prevent case matching problems.

    if (topicStr.length() > MQTT_TOPIC_MAX_SZ) {
        topicStr = topicStr.substring(0, MQTT_TOPIC_MAX_SZ);
    }

    // char buff[topicStr.length() + payloadStr.length() + 30];
    // sprintf(buff, "MQTT Message Arrived, [Topic]: %s", topicStr.c_str());
    // Log.infoln(buff);
    // sprintf(buff, "MQTT Message Arrived, Payload: %s", payloadStr.c_str());
    // Log.infoln(buff);


    // *************************************************************************************************************************
    // START OF MQTT COMMAND ACTIONS:

    if (topicStr == mqttNameStr + makeMqttCmdStr(CMD_AUDMODE_STR)) {
        Log.infoln("MQTT: Received Audio Mode Command");
        audioModeCmd(payloadStr, MQTT_CNTRL);
    }
    else if (topicStr == mqttNameStr + makeMqttCmdStr(CMD_FREQ_STR)) {
        Log.infoln("MQTT: Received Radio Frequency Command");
        frequencyCmd(payloadStr, MQTT_CNTRL);
    }
    else if (topicStr == mqttNameStr + makeMqttCmdStr(CMD_GPIO19_STR)) {
        Log.infoln("MQTT: Received GPIO19 Command");
        gpioMqttControl(payloadStr, GPIO19_PIN);
    }
    else if (topicStr == mqttNameStr + makeMqttCmdStr(CMD_GPIO23_STR)) {
        Log.infoln("MQTT: Received GPIO23 Command");
        gpioMqttControl(payloadStr, GPIO23_PIN);
    }
    else if (topicStr == mqttNameStr + makeMqttCmdStr(CMD_GPIO33_STR)) {
        Log.infoln("MQTT: Received GPIO33 Command");
        gpioMqttControl(payloadStr, GPIO33_PIN);
    }
    else if (topicStr == mqttNameStr + makeMqttCmdStr(CMD_INFO_STR)) {
        Log.infoln("MQTT: Received System Information Command.");

        if (infoCmd(payloadStr, MQTT_CNTRL)) {
            sprintf(mqttBuff,
                    "{\"%s\": \"ok\", \"version\": \"%s\", \"hostName\": \"%s\", \"ip\": \"%s\", \"rssi\": %d, \"status\": \"0x%02X\"}",
                    CMD_INFO_STR,
                    VERSION_STR,
                    staNameStr.c_str(),
                    WiFi.localIP().toString().c_str(),
                    WiFi.RSSI(),
                    getControllerStatus());
        }
        else {
            sprintf(mqttBuff, "{\"%s\": \"fail\"}", CMD_INFO_STR);
        }
        topicStr = mqttNameStr + MQTT_INFORM_STR;
        mqttClient.publish(topicStr.c_str(), mqttBuff);
    }
    else if (topicStr == mqttNameStr + makeMqttCmdStr(CMD_MUTE_STR)) {
        Log.infoln("MQTT: Received Audio Mute Command");
        muteCmd(payloadStr, MQTT_CNTRL);
    }
    else if (topicStr == mqttNameStr + makeMqttCmdStr(CMD_PICODE_STR)) {
        Log.infoln("MQTT Received RDS PI Code Command");
        piCodeCmd(payloadStr, MQTT_CNTRL);
    }
    else if (topicStr == mqttNameStr + makeMqttCmdStr(CMD_PTYCODE_STR)) {
        Log.infoln("MQTT Received RDS PTY Code Command");
        ptyCodeCmd(payloadStr, MQTT_CNTRL);
    }
    else if (topicStr == mqttNameStr + makeMqttCmdStr(CMD_PSN_STR)) {
        Log.infoln("MQTT: Received RDS Program Station Name Command");
        programServiceNameCmd(payloadStr, MQTT_CNTRL);
    }
    else if (topicStr == mqttNameStr + makeMqttCmdStr(CMD_RADIOTEXT_STR)) {
        Log.infoln("MQTT: Received RadioText Command");
        radioTextCmd(payloadStr, MQTT_CNTRL);
    }
    else if (topicStr == mqttNameStr + makeMqttCmdStr(CMD_RF_CARRIER_STR)) {
        Log.infoln("MQTT: Received RF Carrier Control Command");
        radioTextCmd(payloadStr, MQTT_CNTRL);
    }
    else if (topicStr == mqttNameStr + makeMqttCmdStr(CMD_REBOOT_STR)) {
        Log.infoln("MQTT: Received System Reboot Command.");
        rebootCmd(payloadStr, MQTT_CNTRL);
    }
    else if (topicStr == mqttNameStr + makeMqttCmdStr(CMD_START_STR)) {
        Log.infoln("MQTT: Received RDS Restart Command.");
        startCmd(payloadStr, MQTT_CNTRL);
    }
    else if (topicStr == mqttNameStr + makeMqttCmdStr(CMD_STOP_STR)) {
        Log.infoln("MQTT: Received RDS Stop Command.");
        stopCmd(payloadStr, MQTT_CNTRL);
    }
    else if (topicStr == mqttNameStr + makeMqttCmdStr(CMD_PERIOD_STR)) {
        Log.infoln("MQTT: Received RDS Display Time Command");
        rdsTimePeriodCmd(payloadStr, MQTT_CNTRL);
    }
    else {
        sprintf(logBuff, "MQTT: Received Unknown Command (%s), Ignored.", topicStr.c_str());
        Log.errorln(logBuff);
    }
}

// *************************************************************************************************************************
// mqttInit(): Initialize MQTT and subscribe with broker.
//             MQTT is disabled when in HotSpot mode.
//
void mqttInit(void) {
    String payloadStr;
    String topicStr;

    char logBuff[120];

    if (!ctrlMqttFlg) { // MQTT Controller Disabled.
        mqttOnlineFlg = false;
        return;
    }

    topicStr.reserve(MQTT_NAME_MAX_SZ + 20);

    if (WiFi.status() == WL_CONNECTED) {
        Log.traceln("Initializing MQTT");
        mqttClient.setServer(mqttIP, mqttPort);
        mqttClient.setCallback(mqttCallback); // Topic Subscription callback handler.
        mqttClient.setKeepAlive(MQTT_KEEP_ALIVE);

        if (mqttClient.connect(mqttNameStr.c_str(), mqttUserStr.c_str(), mqttPwStr.c_str())) {
            mqttOnlineFlg = true;
            topicStr      = mqttNameStr;
            topicStr     += MQTT_CONNECT_STR;
            payloadStr    = "{\"boot\": 0}";                          // JSON Formatted payload.
            mqttClient.publish(topicStr.c_str(), payloadStr.c_str()); // Publish reconnect status.

            sprintf(logBuff, "-> MQTT Started. Sent Topic: %s, Payload: %s", topicStr.c_str(), payloadStr.c_str());
            Log.infoln(logBuff);

            updateUiMqttMsg(MQTT_ONLINE_STR);

            topicStr  = mqttNameStr;
            topicStr += MQTT_CMD_SUB_STR;

            if (mqttClient.subscribe(topicStr.c_str())) {
                sprintf(logBuff, "-> MQTT Successfully Subscribed to \"%s\"", topicStr.c_str());
                Log.infoln(logBuff);
            }
            else {
                Log.errorln("-> MQTT subscribe failed!");
                updateUiMqttMsg(MQTT_SUBCR_FAIL_STR);
            }
        }
        else {
            Log.errorln("-> MQTT NOT Started.");
            mqttOnlineFlg = false;
        }
    }
    else {
        updateUiMqttMsg(MQTT_NOT_AVAIL_STR);
        Log.errorln("WiFi Router Not Connected; MQTT Disabled.");
    }
}

// *************************************************************************************************************************
// mqttReconnect(): If connection to broker has been lost then attempt reconnection.
//                  On entry, if resetFlg arg is true then reset timer and failcount.
void mqttReconnect(bool resetFlg)
{
    char logBuff[120];
    static uint8_t  mqttRetryCount = 0;                 // MQTT Connection Retry Counter, allow several attempts.
    static unsigned long previousWiFiMillis = millis(); // Timer for WiFi services.
    char   payloadBuff[40];
    String topicStr;

    if (resetFlg == true) {
        mqttRetryCount     = 0;
        previousWiFiMillis = millis() - MQTT_RECONNECT_TIME + 2000; // Allow Reconnect in 2 secs.
        Log.traceln("MQTT Reconnect Fail Count has Been Reset.");
        return;
    }

    if (!ctrlMqttFlg) { // MQTT Controller Disabled.
        mqttOnlineFlg = false;

        if (mqttClient.connected()) {
            mqttClient.disconnect();
            Log.traceln("MQTT Controller Disabled: Closed Connection.");
        }
        return;
    }
    else if (millis() - previousWiFiMillis >= MQTT_RECONNECT_TIME) {
        previousWiFiMillis = millis();
    }
    else {
        return;
    }

    topicStr.reserve(sizeof(mqttNameStr) + 20);

    if ((WiFi.status() == WL_CONNECTED) && !mqttClient.connected()) {
        if (!mqttClient.connected() && (mqttRetryCount++ < MQTT_RETRY_CNT)) {
            updateUiMqttMsg(MQTT_RETRY_STR);
            sprintf(logBuff, "Attempting MQTT Reconnection #%u ...", mqttRetryCount);
            Log.infoln(logBuff);
            mqttClient.setServer(mqttIP, mqttPort);
            mqttClient.setCallback(mqttCallback); // Topic Subscription callback handler.
            mqttClient.setKeepAlive(MQTT_KEEP_ALIVE);

            sprintf(logBuff, "-> Broker Name: \"%s\"", mqttNameStr.c_str());
            Log.verboseln(logBuff);
            sprintf(logBuff, "-> Broker User: \"%s\", Password: \"%s\"", mqttUserStr.c_str(), mqttPwStr.c_str());
            Log.verboseln(logBuff);
            sprintf(logBuff, "-> Broker IP: %s, PORT: %u", IpAddressToString(mqttIP).c_str(), mqttPort);
            Log.verboseln(logBuff);

            if (mqttClient.connect(mqttNameStr.c_str(), mqttUserStr.c_str(), mqttPwStr.c_str())) { // Connect to MQTT Server
                mqttOnlineFlg = true;
                topicStr      = mqttNameStr + MQTT_CONNECT_STR;
                sprintf(payloadBuff, "{\"reconnect\": %d}", mqttRetryCount);                       // JSON Formatted payload.
                mqttClient.publish(topicStr.c_str(), payloadBuff);                                 // Publish reconnect status.

                sprintf(logBuff, "-> MQTT Reconnected. Sent Topic:%s, Payload:%s", topicStr.c_str(), payloadBuff);
                Log.infoln(logBuff);

                updateUiMqttMsg(MQTT_ONLINE_STR);

                topicStr = mqttNameStr + MQTT_CMD_SUB_STR;

                if (mqttClient.subscribe(topicStr.c_str())) {
                    sprintf(logBuff, "-> MQTT Successfully Subscribed to %s.", topicStr.c_str());
                    Log.infoln(logBuff);
                }
                else {
                    Log.errorln("-> MQTT subscribe failed!");
                    updateUiMqttMsg(MQTT_SUBCR_FAIL_STR);
                }
                mqttRetryCount = 0; // Successful reconnect, OK to reset counter.
            }
            else if (mqttRetryCount < MQTT_RETRY_CNT) {
                sprintf(logBuff, "-> MQTT Connection Failure #%u, Code= %s.", mqttRetryCount, returnClientCode(mqttClient.state()).c_str());
                Log.warningln(logBuff);
                updateUiMqttMsg(MQTT_RETRY_FAIL_STR + String(mqttRetryCount));
            }
            else {
                Log.errorln("-> MQTT Controller Has Been Disabled, Too Many Reconnect Attempts.");
                updateUiMqttMsg(MQTT_DISABLED_STR);
            }
        }
    }
    else if (WiFi.status() != WL_CONNECTED) {
        updateUiMqttMsg(MQTT_NOT_AVAIL_STR);
        Log.errorln("-> WiFi Router Not Connected; MQTT Disabled.");
    }
}

// *************************************************************************************************************************
// mqttSendMessages(): Broadcast system messages to MQTT broker.
// The messages are sent periodically (MQTT_MSG_TIME) or immediately whenever they change.
void mqttSendMessages(void)
{
    static bool refresh = true;
    char logBuff[120];
    char payloadBuff[35];
    static unsigned long previousMqttMillis = millis(); // Timer for MQTT services.
    static float    oldVbatVolts       = -1.0f;
    static float    oldPaVolts         = -1.0f;
    String topicStr;

    if (!ctrlMqttFlg) { // MQTT Controller Disabled.
        return;
    }

    if (!mqttClient.connected()) { // MQTT not connected, nothing to do. Exit.
        return;
    }

    if (millis() - previousMqttMillis >= MQTT_MSG_TIME) {
        previousMqttMillis = millis();
        refresh            = true;
    }

    if (refresh ||
        ((vbatVolts > oldVbatVolts + VOLTS_HYSTERESIS) || (vbatVolts < oldVbatVolts - VOLTS_HYSTERESIS)) ||
        ((paVolts > oldPaVolts + VOLTS_HYSTERESIS) || (paVolts < oldPaVolts - VOLTS_HYSTERESIS))) {
        oldVbatVolts = vbatVolts;
        oldPaVolts   = paVolts;
        topicStr     = mqttNameStr + MQTT_VOLTS_STR;
        sprintf(payloadBuff, "{\"vbat\": %0.1f,\"pa\": %0.1f}", vbatVolts, paVolts); // JSON Formatted Payload.
        mqttClient.publish(topicStr.c_str(), payloadBuff);                           // Publish Power Supply Voltage.

        sprintf(logBuff, "MQTT Publish, [Topic]: %s", topicStr.c_str());
        Log.infoln(logBuff);
        sprintf(logBuff, "-> Payload: %s", payloadBuff);
        Log.infoln(logBuff);
    }
    refresh = false;
}

// *************************************************************************************************************************
// mqttService(): Service incoming MQTT message queue.
//
void mqttService(void) {
    # ifdef MQTT_ENB

    if (mqttClient.connected()) {
        mqttClient.loop(); // Service MQTT background tasks.
    }
    # endif // ifdef MQTT_ENB
}

// *************************************************************************************************************************
// processMQTT(): MQTT Service Handler. Must be installed in main loop.
//
void processMQTT(void) {
    # ifdef MQTT_ENB
    mqttService(); // Service MQTT background tasks.
    mqttSendMessages();
    # endif // ifdef MQTT_ENB
}

// *************************************************************************************************************************
// returnClientCode(): Convert Client.state() code to String Message.

/* MQTT_CONNECTION_TIMEOUT     -4
   MQTT_CONNECTION_LOST        -3
   MQTT_CONNECT_FAILED         -2
   MQTT_DISCONNECTED           -1
   MQTT_CONNECTED               0
   MQTT_CONNECT_BAD_PROTOCOL    1
   MQTT_CONNECT_BAD_CLIENT_ID   2
   MQTT_CONNECT_UNAVAILABLE     3
   MQTT_CONNECT_BAD_CREDENTIALS 4
   MQTT_CONNECT_UNAUTHORIZED    5
 */
const String returnClientCode(int code) {
    String tmpStr;

    switch (code) {
      case MQTT_CONNECTION_TIMEOUT:
          tmpStr = "MQTT_CONNECTION_TIMEOUT";
          break;
      case MQTT_CONNECTION_LOST:
          tmpStr = "MQTT_CONNECTION_LOST";
          break;
      case MQTT_CONNECT_FAILED:
          tmpStr = "MQTT_CONNECT_FAILED";
          break;
      case MQTT_DISCONNECTED:
          tmpStr = "MQTT_DISCONNECTED";
          break;
      case MQTT_CONNECTED:
          tmpStr = "MQTT_CONNECTED";
          break;
      case MQTT_CONNECT_BAD_PROTOCOL:
          tmpStr = "MQTT_CONNECT_BAD_PROTOCOL";
          break;
      case MQTT_CONNECT_BAD_CLIENT_ID:
          tmpStr = "MQTT_CONNECT_BAD_CLIENT_ID";
          break;
      case MQTT_CONNECT_UNAVAILABLE:
          tmpStr = "MQTT_CONNECT_UNAVAILABLE";
          break;
      case MQTT_CONNECT_BAD_CREDENTIALS:
          tmpStr = "MQTT_CONNECT_BAD_CREDENTIALS";
          break;
      case MQTT_CONNECT_UNAUTHORIZED:
          tmpStr = "MQTT_CONNECT_UNAUTHORIZED";
          break;
      default:
          tmpStr = "Undefined";
          break;
    }
    return tmpStr;
}

// *************************************************************************************************************************

#endif // END OF MQTT

// *************************************************************************************************************************
// EOF
