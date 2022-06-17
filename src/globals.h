/*
   File: globals.h
   Project: PixelRadio, an RBDS/RDS FM Transmitter (QN8027 Digital FM IC)
   Version: 1.1.0
   Creation: Dec-16-2021
   Revised:  Jun-13-2022
   Revision History: See PixelRadio.cpp
   Project Leader: T. Black (thomastech)
   Contributors: thomastech, dkulp

   (c) copyright T. Black 2021-2022.
   Licensed under GNU GPL 3.0 and later, under this license absolutely no warranty is given.
   This Code was formatted with the uncrustify extension.
 */

// *********************************************************************************************
#pragma once

// *********************************************************************************************

extern bool activeTextHttpFlg;
extern bool activeTextLocalFlg;
extern bool activeTextMqttFlg;
extern bool activeTextSerialFlg;
extern bool apFallBackFlg;
extern bool ctrlLocalFlg;
extern bool ctrlHttpFlg;
extern bool ctrlMqttFlg;
//extern bool ctrlSerialFlg;
extern bool newVgaGainFlg;
extern bool newAutoRfFlg;
extern bool newCarrierFlg;
extern bool newDigGainFlg;
extern bool newFreqFlg;
extern bool newGpio19Flg;
extern bool newGpio23Flg;
extern bool newGpio33Flg;
extern bool newInpImpFlg;
extern bool newAudioModeFlg;
extern bool newMuteFlg;
extern bool newPreEmphFlg;
extern bool newRfPowerFlg;
extern bool muteFlg;
extern bool mqttOnlineFlg;
extern bool rdsText1EnbFlg;
extern bool rdsText2EnbFlg;
extern bool rdsText3EnbFlg;
extern bool rebootFlg;
extern bool rfAutoFlg;
extern bool rfCarrierFlg;
extern bool stereoEnbFlg;
extern bool stopHttpFlg;
extern bool stopMqttFlg;
extern bool stopSerialFlg;
extern bool testModeFlg;
extern bool textHttpFlg;
extern bool textMqttFlg;
extern bool textSerialFlg;
extern bool wifiDhcpFlg;
extern bool WiFiRebootFlg;

extern uint8_t analogVol;
extern uint8_t fmRadioTestCode;
extern uint8_t usbVol;

extern uint16_t mqttPort;

extern uint16_t rdsHttpPiCode;
extern uint16_t rdsLocalPiCode;
extern uint16_t rdsMqttPiCode;
extern uint16_t rdsSerialPiCode;

extern uint8_t  rdsHttpPtyCode;
extern uint8_t  rdsLocalPtyCode;
extern uint8_t  rdsMqttPtyCode;
extern uint8_t  rdsSerialPtyCode;

extern uint32_t baudRate;
extern uint32_t rdsHttpMsgTime;
extern uint32_t rdsLocalMsgTime;
extern uint32_t rdsMqttMsgTime;
extern uint32_t rdsSerialMsgTime;
extern uint32_t rdsMsgTime;

extern uint16_t fmFreqX10;
extern float vbatVolts;
extern float paVolts;

// WiFi Vars, including Credentials
extern IPAddress hotSpotIP;
extern IPAddress subNet;
extern IPAddress staticIP;
extern IPAddress mqttIP;
extern IPAddress wifiDNS;
extern IPAddress wifiGateway;

// GUI Strings
extern String apNameStr;
extern String apIpAddrStr;
extern String ctrlSerialStr;
extern String preEmphasisStr;
extern String digitalGainStr;
extern String gpio19BootStr;
extern String gpio23BootStr;
extern String gpio33BootStr;
extern String gpio19CtrlStr;
extern String gpio23CtrlStr;
extern String gpio33CtrlStr;
extern String inpImpedStr;
extern String ipAddrStr;
extern String logLevelStr;
extern String mdnsNameStr;
extern String mqttIpStr;
extern String mqttNameStr;
extern String mqttPwStr;
extern String mqttUserStr;
extern String rdsHttpPsnStr;
extern String rdsMqttPsnStr;
extern String rdsSerialPsnStr;
extern String rdsHttpTextStr;
extern String rdsMqttTextStr;
extern String rdsSerialTextStr;
extern String rdsTextMsgStr;
extern String rdsTextMsg1Str;
extern String rdsTextMsg2Str;
extern String rdsTextMsg3Str;
extern String rdsLocalPsnStr;
extern String rfPowerStr;
extern String staNameStr;
extern String staticIpStr;
extern String subNetStr;
extern String userNameStr;
extern String userPassStr;
extern String vgaGainStr;
extern String wifiDnsStr;
extern String wifiGatewayStr;
extern String wifiSSIDStr;
extern String wifiWpaKeyStr;
