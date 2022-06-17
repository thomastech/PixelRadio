/*
   File: webGUI.h
   Project: PixelRadio, an RBDS/RDS FM Transmitter (QN8027 Digital FM IC)
   Version: 1.1.0
   Creation: Dec-16-2021
   Revised:  Jun-13-2022
   Revision History: See PixelRadio.cpp
   Project Leader: T. Black (thomastech)
   Contributors: thomastech

   (c) copyright T. Black 2021-2022, Licensed under GNU GPL 3.0 and later, under this
   license absolutely no warranty is given.
   This Code was formatted with the uncrustify extension.

 */

// ************************************************************************************************
#pragma once

// ************************************************************************************************

extern String tempStr;


extern uint16_t aboutTab;
extern uint16_t adjTab;
extern uint16_t backupTab;
extern uint16_t ctrlTab;
extern uint16_t diagTab;
extern uint16_t gpioTab;
extern uint16_t homeTab;
extern uint16_t radioTab;
extern uint16_t rdsTab;
extern uint16_t wifiTab;

extern uint16_t aboutLogoID;
extern uint16_t aboutVersionID;

extern uint16_t adjAvolID;
extern uint16_t adjFmDispID;
extern uint16_t adjFreqID;
extern uint16_t adjMuteID;
extern uint16_t adjUvolID;
extern uint16_t adjSaveID;
extern uint16_t adjSaveMsgID;
extern uint16_t adjTestModeID;

extern uint16_t backupRestoreID;
extern uint16_t backupRestoreMsgID;
extern uint16_t backupSaveID;
extern uint16_t backupSaveMsgID;
extern uint16_t backupSaveSetID;
extern uint16_t backupSaveSetMsgID;

extern uint16_t ctrlHttpID;
extern uint16_t ctrlLocalID;
extern uint16_t ctrlMqttID;
extern uint16_t ctrlMqttIpID;
extern uint16_t ctrlMqttMsgID;
extern uint16_t ctrlMqttNameID;
extern uint16_t ctrlMqttPortID;
extern uint16_t ctrlMqttUserID;
extern uint16_t ctrlMqttPwID;
extern uint16_t ctrlSerialID;
extern uint16_t ctrlSerialMsgID;
extern uint16_t ctrlSaveID;
extern uint16_t ctrlSaveMsgID;

extern uint16_t diagBootID;
extern uint16_t diagBootMsgID;
extern uint16_t diagLogID;
extern uint16_t diagLogMsgID;
extern uint16_t diagMemoryID;
extern uint16_t diagSoundID;
extern uint16_t diagTimerID;
extern uint16_t diagVbatID;
extern uint16_t diagVdcID;

extern uint16_t gpio19ID;
extern uint16_t gpio23ID;
extern uint16_t gpio33ID;
extern uint16_t gpio19MsgID;
extern uint16_t gpio23MsgID;
extern uint16_t gpio33MsgID;
extern uint16_t gpioSaveID;
extern uint16_t gpioSaveMsgID;

extern uint16_t homeFreqID;
extern uint16_t homeOnAirID;
extern uint16_t homeRdsTextID;
extern uint16_t homeRdsTmrID;
extern uint16_t homeRssiID;
extern uint16_t homeStaID;
extern uint16_t homeStaMsgID;

extern uint16_t radioAudioID;
extern uint16_t radioAudioMsgID;
extern uint16_t radioVgaGainID;
extern uint16_t radioDgainID;
extern uint16_t radioAutoID;
extern uint16_t radioEmphID;
extern uint16_t radioFreqID;
extern uint16_t radioGainID;
extern uint16_t radioImpID;
extern uint16_t radioPwrID;
extern uint16_t radioRfEnbID;
extern uint16_t radioSaveID;
extern uint16_t radioSaveMsgID;

extern uint16_t rdsDspTmID;
extern uint16_t rdsEnbID;
extern uint16_t rdsEnb1ID;
extern uint16_t rdsEnb2ID;
extern uint16_t rdsEnb3ID;
extern uint16_t rdsPiID;
extern uint16_t rdsPtyID;
extern uint16_t rdsPiMsgID;
extern uint16_t rdsProgNameID;
extern uint16_t rdsSaveID;
extern uint16_t rdsSaveMsgID;
extern uint16_t rdsSnameID;
extern uint16_t rdsRstID;
extern uint16_t rdsText1ID;
extern uint16_t rdsText2ID;
extern uint16_t rdsText3ID;
extern uint16_t rdsText1MsgID;
extern uint16_t rdsText2MsgID;
extern uint16_t rdsText3MsgID;

extern uint16_t wifiApBootID;
extern uint16_t wifiApFallID;
extern uint16_t wifiApID;
extern uint16_t wifiApIpID;
extern uint16_t wifiApNameID;
extern uint16_t wifiDevPwMsgID;
extern uint16_t wifiDevUserID;
extern uint16_t wifiDevUserMsgID;
extern uint16_t wifiDevPwID;
extern uint16_t wifiDhcpID;
extern uint16_t wifiDhcpMsgID;
extern uint16_t wifiDnsID;
extern uint16_t wifiGatewayID;
extern uint16_t wifiIpID;
extern uint16_t wifiMdnsNameID;
extern uint16_t wifiNetID;
extern uint16_t wifiRssiID;
extern uint16_t wifiSaveID;
extern uint16_t wifiSaveMsgID;
extern uint16_t wifiSsidID;
extern uint16_t wifiStaID;
extern uint16_t wifiStaMsgID;
extern uint16_t wifiStaNameID;
extern uint16_t wifiSubID;
extern uint16_t wifiWpaKeyID;
