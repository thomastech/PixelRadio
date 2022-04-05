/*
   File: config.h  (Special Configuration Options, User Defined)
   Project: PixelRadio, an RBDS/RDS FM Transmitter (QN8027 Digital FM IC)
   Version: 1.0
   Creation: Dec-16-2021
   Revised:  Apr-05-2022
   Public Release:
   Revision History: See PixelRadio.cpp
   Project Leader: T. Black (thomastech)
   Contributors: thomastech

   (c) copyright T. Black 2021-2022.
   Licensed under GNU GPL 3.0 and later, under this license absolutely no warranty is given.
   This Code was formatted with the uncrustify extension.

   Additional Tricks and Treats:
   The Serial log feature can be omitted by using the #define DISABLE_LOGGING" in Logging.h found
   in the ArduinoLog library. Doing this will reduce binary size.
 */

#pragma once

// ************************************************************************************************************************

/* Uncomment REQUIRE_WIFI_AP_PSK if PSK (Pre Share Key) is required to access WiFi AP (hotspot) Mode. */
#define REQUIRE_WIFI_AP_PSK

/* Uncomment ADV_RADIO_FEATURES to enable Advanced Radio Features (adds extra controls on UI radioTab). */
#define ADV_RADIO_FEATURES

/* Uncomment ADD_CHIP_ID if you want the ESP32's Unique Chip ID to be appended to the AP (Hotspot) Host Name. */
//#define ADD_CHIP_ID

/* COMMAND LINE INTERFACE SECTION (SERIAL PORT RDS CONTROLLER)
   Change CMD_EOL_TERM if your serial console doesn't send a Carriage Return <CR> when [ENTER] is pressed.
   For example, change to  '\n' if your terminal only sends a NEWLINE <LF> when [ENTER] is pressed. */
#define CMD_EOL_TERM    '\r'                            // Serial Manager EOL termination character.

/* Uncomment MQTT_ENB define statement to enable the MQTT Controller. Approx 17KB used. */
#define MQTT_ENB

/* If you have an MQTT Broker then configure it here. */
const IPAddress MQTT_IP_DEF = { 0u, 0u, 0u, 0u }; // Default IP of MQTT Broker server. Can be changed in Web UI.
//const IPAddress MQTT_IP_DEF = { 192u, 168u, 1u, 202u }; // Default IP of MQTT Broker server. Can be changed in Web UI.
const uint32_t MQTT_PORT_DEF = 1883;                // 1883 is Default MQTT Port. Change it here if differnt port is needed.

/* Uncomment HTTP_ENB define statement to enable the HTTP Controller. Approx 13KB used. */
#define HTTP_ENB

/* Uncomment MDNS_ENB define statement to enable mDNS. Approx 1.2KB used.
   Default mDNS name is PixelRadio.local */
#define MDNS_ENB

/* Uncomment OTA_ENB to enable Over-the-Air Firmware Updates. Approx 15KB used. */
//#define OTA_ENB

/* Uncomment OTA_REBOOT_ENB to enable automatic reboots after a successful OTA (Over-The-Air) firmware update.*/
//#define OTA_REBOOT_ENB

/* ----------------------------------------------------------------------------------------------------------------------- */
// EOF
