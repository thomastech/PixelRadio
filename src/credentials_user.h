/*
   File: credentials.h
   Project: PixelRadio, an RBDS/RDS FM Transmitter (QN8027 Digital FM IC)
   Version: 1.0
   Creation: Dec-16-2021
   Revised:  Feb-24-2022
   Public Release:
   Project Leader: T. Black (thomastech)
   Contributors: thomastech
   Revision History: See PixelRadio.cpp

   (c) copyright T. Black 2021-2022, Licensed under GNU GPL 3.0 and later, under this license absolutely no warranty is given.
   This Code was formatted with the uncrustify extension.

   -> IMPORTANT: Edit this file and rename to credentials.h
 */

#pragma once

// *********************************************************************************************
//                          Edit this file and rename to credentials.h
// *********************************************************************************************

// Private WiFi credentials. Do NOT publicly share this sensitive data.
// ALternatively, leave as-is and use SD Card Credential Feature.
#define SSID_NM_STR   "YOUR_SSID_NAME_HERE" // SSID, Can be changed in Web UI
#define WPA_KEY_STR   "YOUR_WPA_KEY_HERE"   // WPA Key, Can be changed in Web UI
#define AP_PSK_STR    "Pixel$Lights"        // AP Mode PSK Key, Can only be changed here.


// Private MQTT credentials. Do NOT publicly share this sensitive data.
// Leave as-is if MQTT is not used.
#define MQTT_ID_STR   "YOUR_BROKER_NAME_HERE"     // Broker Name, Can be changed in Web UI.
#define MQTT_PW_STR   "YOUR_BROKER_PASSWORD_HERE" // Broker Password, Can be changed in Web UI.

// Web UI Login Credentials
#define LOGIN_USER_NAME_STR "pixel"               // User Name, Can be changed in Web UI.
#define LOGIN_USER_PW_STR "lights"              // User Password, Can be changed in Web UI.
