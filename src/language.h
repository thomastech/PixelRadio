/*
   File: language.h
   Project: PixelRadio, an RBDS/RDS FM Transmitter (QN8027 Digital FM IC)
   Version: 1.1.0
   Creation: Dec-16-2021
   Revised:  Jun-13-2022
   Revision History: See PixelRadio.cpp
   Project Leader: T. Black (thomastech)
   Contributors: thomastech

   (c) copyright T. Black 2021-2022.
   Licensed under GNU GPL 3.0 and later, under this license absolutely no warranty is given.
   This Code was formatted with the uncrustify extension.

   Note 1: Language text uses define's instead of const String. This saves a LOT of Ram and Flash.

 */

// ************************************************************************************************
#pragma once
#include "config.h"

// ************************************************************************************************

#define ABOUT_TAB_STR  "About"
#define ABOUT_VERS_STR "About PixelRadio"

#define ADJUST_ANA_VOL_STR    "AUDIO VOLUME"
#define ADJUST_FM_FRQ_STR     "FM FREQUENCY"
#define ADJUST_FRQ_CTRL_STR   "FM TRANSMIT FREQUENCY"
#define ADJUST_FRQ_ADJ_STR    "FREQUENCY ADJUST"
#define ADJUST_MUTE_STR       "AUDIO MUTE"
#define ADJUST_AUDIO_SEP_STR  "AUDIO CONTROLS"
#define ADJUST_TAB_STR        "Adjust"
#define ADJUST_TEST_STR       "TEST TONES"
#define ADJUST_USB_VOL_STR    "USB VOLUME"

#define AUDIO_TEST_STR    "PIXELRADIO AUDIO TEST"
#define AUDIO_PSN_STR     "TestTone"  // RDS PSN for Audio Test Tone mode, Max 8 Chars.

#define VGA_GAIN0_STR     "3dB"
#define VGA_GAIN1_STR     "6dB"
#define VGA_GAIN2_STR     "9dB"
#define VGA_GAIN3_STR     "12dB (default)"
#define VGA_GAIN4_STR     "15dB"
#define VGA_GAIN5_STR     "18dB"
#define VGA_GAIN_DEF_STR  VGA_GAIN3_STR;

#define INP_IMP05K_STR   "5K Ohms"
#define INP_IMP10K_STR   "10K Ohms"
#define INP_IMP20K_STR   "20K Ohms (default)"
#define INP_IMP40K_STR   "40K Ohms"
#define INP_IMP_DEF_STR  INP_IMP20K_STR;

#define APP_NAME_STR  "PixelRadio"

#define BACKUP_TAB_STR       "Backup"
#define BACKUP_SAVE_STR      "SAVE"
#define BACKUP_SAV_CFG_STR   "SAVE TO SD CARD"
#define BACKUP_SAV_FAIL_STR  "SAVE FAILED: CHECK SD CARD"
#define BACKUP_SAV_PASS_STR  "Configuration Saved"
#define BACKUP_RESTORE_STR   "RESTORE"
#define BACKUP_RES_CFG_STR   "RESTORE FROM SD CARD"
#define BACKUP_RES_FAIL_STR  "RESTORE FAILED: CHECK SD CARD"
#define BACKUP_RES_PASS_STR  "Restore Complete: Reboot Required"

#define CTRL_LOCAL_STR       "LOCAL CONTROL"
#define CTRL_TAB_STR         "Controllers"
#define CTRL_HTTP_STR        "HTTP CONTROL"
#define CTRL_HTPP_SET_STR    "HTTP CONTROL SETTINGS"
#define CTRL_LOCAL_SEP_STR   "LOCAL CONTROL SETTINGS"
#define CTRL_MQTT_IP_STR     "BROKER IP ADDRESS"
#define CTRL_MQTT_PW_STR     "BROKER PASSWORD"
#define CTRL_MQTT_PORT_STR   "BROKER PORT NUMBER"
#define CTRL_MQTT_STR        "MQTT CONTROL"
#define CTRL_MQTT_SEP_STR    "MQTT CONTROL SETTINGS"
#define CTRL_MQTT_USER_STR   "BROKER USERNAME"
#define CTRL_SERIAL_STR      "SERIAL CONTROL"
#define CTLR_SERIAL_MSG_STR  "WARNING: DIAGNOSTIC SERIAL LOG IS ON"
#define CTRL_USB_SERIAL_STR  "USB SERIAL CONTROL SETTINGS"

#define PRE_EMPH_USA_STR     "North America (75uS)" // North America / Japan.
#define PRE_EMPH_EUR_STR     "Europe (50uS)"        // Europe, Australia, China.
#define PRE_EMPH_DEF_STR     PRE_EMPH_USA_STR;

#define DIAG_BOOT_MSG1_STR   "WARNING: SYSTEM WILL REBOOT<br>** RELEASE NOW TO ABORT **"
#define DIAG_BOOT_MSG2_STR   "** SYSTEM REBOOTING **<br>WAIT 30 SECONDS BEFORE ACCESSING WEB PAGE."
#define DIAG_DEBUG_SEP_STR   "CODE DEBUGGING"
#define DIAG_FREE_MEM_STR    "FREE MEMORY"
#define DIAG_HEALTH_SEP_STR  "HEALTH"
#define DIAG_LOG_LVL_STR     "SERIAL LOG LEVEL"
#define DIAG_LOG_MSG_STR     "WARNING: SERIAL CONTROLLER IS ON"
#define DIAG_LONG_PRESS_STR  "Long Press (5secs)"
#define DIAG_REBOOT_STR      "REBOOT SYSTEM"
#define DIAG_SYSTEM_SEP_STR  "SYSTEM"
#define DIAG_RUN_TIME_STR    "SYSTEM RUN TIME"
#define DIAG_TAB_STR         "Diagnostic"
#define DIAG_VBAT_STR        "SYSTEM VOLTAGE"
#define DIAG_VDC_STR         "RF PA VOLTAGE"

#define DIAG_LOG_ERROR_STR   "LOG_LEVEL_ERROR"
#define DIAG_LOG_FATAL_STR   "LOG_LEVEL_FATAL"
#define DIAG_LOG_INFO_STR    "LOG_LEVEL_INFO"
#define DIAG_LOG_SILENT_STR  "LOG_LEVEL_SILENT (default)"
#define DIAG_LOG_WARN_STR    "LOG_LEVEL_WARNING"
#define DIAG_LOG_TRACE_STR   "LOG_LEVEL_TRACE"
#define DIAG_LOG_VERB_STR    "LOG_LEVEL_VERBOSE"
#define DIAG_LOG_DEF_STR      DIAG_LOG_VERB_STR;

#define DHCP_LOCKED_STR  "DHCP LOCKED. INCOMPLETE WIFI SETTINGS."

// #define DIG_AUD_GAIN0_STR     "DIAG0"
// #define DIG_AUD_GAIN1_STR     "DIAG1"
// #define DIG_AUD_GAIN2_STR     "DIAG2"
// #define DIG_AUD_GAIN_DEF_STR  DIG_AUD_GAIN0_STR;

#define DIG_GAIN0_STR     "0 dB (default)"
#define DIG_GAIN1_STR     "1 dB"
#define DIG_GAIN2_STR     "2 dB"
#define DIG_GAIN_DEF_STR  DIG_GAIN0_STR;

#define ERROR_MSG_STR  "ERROR"

#define GPIO_19_STR        "GPIO PIN 19"
#define GPIO_23_STR        "GPIO PIN 23"
#define GPIO_33_STR        "GPIO PIN 33"
#define GPIO_OUT_LO_STR    "DIGITAL OUTPUT = LOW"
#define GPIO_OUT_HI_STR    "DIGITAL OUTPUT = HIGH"
#define GPIO_INP_FT_STR    "DIGITAL INPUT = FLOAT"
#define GPIO_INP_PU_STR    "DIGITAL INPUT = PULLUP"
#define GPIO_INP_PD_STR    "DIGITAL INPUT = PULLDOWN"
#define GPIO_DEF_STR       GPIO_INP_PD_STR
#define GPIO_RD_HI_STR     "PIN READ: 1"
#define GPIO_RD_LO_STR     "PIN READ: 0"
#define GPIO_SETTINGS_STR  "GPIO BOOT STATES"
#define GPIO_TAB_STR       "GPIO"
#define GPIO_SERIAL_STR    "Serial"
#define GPIO_MQTT_STR      "MQTT"
#define GPIO_HTTP_STR      "HTTP"
#define GPIO_LOCAL_STR     "Local"

#define HOME_CUR_TEXT_STR   "CURRENT RADIOTEXT"
#define HOME_RDS_WAIT_STR   "Waiting for RDS RadioText ..."
#define HOME_FM_SEP_STR     "FM RADIO STATUS"
#define HOME_FREQ_STR       "FM FREQUENCY"
#define HOME_RAD_STAT_STR   "RADIO STATUS"
#define HOME_RDS_TIMER_STR  "RDS TIMER COUNTDOWN"
#define HOME_SEP_WIFI_STR   "WIFI"
#define HOME_SEP_RDS_STR    "RADIO DATA SYSTEM (RDS)"
#define HOME_STA_STR        "STA / AP STATUS"
#define HOME_TAB_STR        "Home"
#define HOME_WIFI_STR       "WIFI RSSI"

#define MQTT_SUBSCR_NM_STR   "BROKER SUBSCRIBE NAME"
#define MQTT_DISABLED_STR    "CONNECTION FAILED!<br>DISABLED MQTT CONTROLLER"
#define MQTT_DISCONNECT_STR  "DISCONNECTED"
#define MQTT_MISSING_STR     "MISSING SETTINGS : MQTT DISABLED"
#define MQTT_NOT_AVAIL_STR   "MQTT NOT AVAILABLE IN AP MODE"
#define MQTT_ONLINE_STR      "ONLINE"
#define MQTT_PASS_HIDE_STR   "{PASSWORD HIDDEN}"
#define MQTT_RETRY_STR       "RETRY CONNECTION"
#define MQTT_RETRY_FAIL_STR  "RETRY FAIL #"
#define MQTT_SUBCR_FAIL_STR  "MQTT SUBCRIBE FAILED"

#define RADIO_VGA_AUDIO_STR   "ANALOG (VGA) AUDIO GAIN"
#define RADIO_AMEAS_SEP_STR   "AUDIO MEASUREMENT"
#define RADIO_AUDIO_GAIN_STR  "AUDIO GAIN"
#define RADIO_AUDLVL_STR      "PEAK AUDIO LEVEL"
#define RADIO_AUDIO_MODE_STR  "AUDIO MODE"
#define RADIO_AUTO_OFF_STR    "RF AUTO OFF"
#define RADIO_PRE_EMPH_STR    "FM PRE-EMPHASIS"
#define RADIO_DIG_AUDIO_STR   "DIGITAL AUDIO GAIN"
#define RADIO_FAIL_STR        "<font color=\"" + String(COLOR_RED_STR) + "\">-FAIL-</font>"
#define RADIO_FM_FRQ_STR      "FM FREQUENCY"
#define RADIO_INP_IMP_STR     "INPUT IMPEDANCE"
#define RADIO_MONO_STR        "MONO AUDIO"
#define RADIO_OFF_AIR_STR     "<font color=\"" + String(COLOR_RED_STR) + "\">OFF AIR</font>"
#define RADIO_ON_AIR_STR      "<font color=\"" + String(COLOR_GRN_STR) + "\">ON AIR</font>"
#define RADIO_RF_CARRIER_STR  "RF CARRIER ENABLE"
#define RADIO_RF_POWER_STR    "RF POWER"
#define RADIO_SEP_AUDIO_STR   "AUDIO SETTINGS"
#define RADIO_SEP_MOD_STR     "MODULATION SETTINGS"
#define RADIO_SEP_RF_SET_STR  "RF SETTINGS"
#define RADIO_STEREO_STR      "STEREO AUDIO"
#define RADIO_TAB_STR         "Radio"
#define RADIO_VOLT_STR        "<font color=\"" + String(COLOR_RED_STR) + "\">BAD-VDC</font>"
#define RADIO_VSWR_STR        "<font color=\"" + String(COLOR_RED_STR) + "\">HI-VSWR</font>"

#define RDS_BLANK_STR         "{ RADIOTEXT MESSAGE IS BLANK }"
#define RDS_RF_DISABLED_STR   "{ RADIO CARRIER OFF }"
#define RDS_CTRLS_DIS_STR     "{ ALL RDS CONTROLLERS DISABLED }"
#define RDS_DISABLED_STR      "{ DISABLED }"
#define RDS_DISPLAY_TIME_STR  "RDS DISPLAY TIME (SECS)"
#define RDS_ENB1_STR          "ENABLE RDS MESSAGE #1"
#define RDS_ENB2_STR          "ENABLE RDS MESSAGE #2"
#define RDS_ENB3_STR          "ENABLE RDS MESSAGE #3"
#define RDS_EXPIRED_STR       "{ EXPIRED }"
#define RDS_GENERAL_SET_STR   "GENERAL RDS SETTINGS"
#define RDS_LOCAL_DIS_STR     "{ LOCAL RADIOTEXT MESSAGES DISABLED }"
#define RDS_LOCAL_BLANK_STR   "{ LOCAL RADIOTEXT MESSAGE IS BLANK }"
#define RDS_LOCKED_STR        "{ LOCKED }"
#define RDS_MSG1_STR          "RDS MESSAGE #1 (64 chars max)"
#define RDS_MSG2_STR          "RDS MESSAGE #2 (64 chars max)"
#define RDS_MSG3_STR          "RDS MESSAGE #3 (64 chars max)"
#define RDS_NOT_AVAIL_STR     "{ RADIOTEXT UNAVAILABLE }"
#define RDS_PI_CODE_STR       "PI CODE"
#define RDS_PSN_DEF_STR       "PixeyFM"
#define RDS_PROG_SERV_NM_STR  "PROGRAM SERVICE NAME"
#define RDS_PTY_CODE_STR      "PTY CODE"
#define RDS_RADIOTEXT1_STR    "LOCAL RADIOTEXT #1"
#define RDS_RADIOTEXT2_STR    "LOCAL RADIOTEXT #2"
#define RDS_RADIOTEXT3_STR    "LOCAL RADIOTEXT #3"
#define RDS_REMOTE_BLANK_STR  "{ REMOTE RADIOTEXT MESSAGE IS BLANK }"
#define RDS_RESET_SEP_STR     "LOCAL RDS RESET"
#define RDS_RESET_STR         "RESET RDS SETTINGS"
#define RDS_RESTORE_STR       "Restore Defaults"
#define RDS_TAB_STR           "Local RDS"
#define RDS_TEXT1_DEF_STR     "Welcome to Our Drive-by Holiday Light Show"
#define RDS_TEXT2_DEF_STR     "For Safety Keep Automobile Running Lights On"
#define RDS_TEXT3_DEF_STR     "Please Drive Slowly and Watch Out for Children and Pets"
#define RDS_WAITING_STR       "{ WAITING FOR RADIOTEXT }"

#define RF_AUTO_DIS_STR  "RF Always On (default)"
#define RF_AUTO_ENB_STR  "Turn Off RF >60 Secs Silence"
#define RF_AUTO_DEF_STR  RF_AUTO_DIS_STR;

#define RF_PWR_LOW_STR   "Low"
#define RF_PWR_MED_STR   "Med"
#define RF_PWR_HIGH_STR  "High (default)"
#define RF_PWR_DEF_STR   RF_PWR_HIGH_STR;

#define SAVE_SETTINGS_STR      "SAVE SETTINGS"
#define SAVE_SETTINGS_MSG_STR  "[ Settings Changed, Save Required ]"
#define SAVE_BACKUP_STR        "BACKUP SETTINGS TO SD CARD"

#define SERIAL_OFF_STR  "OFF"
#define SERIAL_096_STR  "9600 Baud"
#define SERIAL_192_STR  "19.2K Baud"
#define SERIAL_576_STR  "57.6K Baud"
#define SERIAL_115_STR  "115.2K Baud"
#define SERIAL_DEF_STR  SERIAL_115_STR;

#define UNITS_DBM_STR  " dBm"
#define UNITS_MHZ_STR  " MHz"

#define WIFI_ADDR_SEP_STR      "WIFI ADDRESS SETTINGS"
#define WIFI_DISABLE_STR       "WiFi Disabled"
#define WIFI_AP_FALLBK_STR     "AP FALLBACK ENABLE"
#define WIFI_AP_IP_ADDR_STR    "AP IP ADDRESS"
#define WIFI_AP_IP_SEP_STR     "AP (HOTSPOT) IP SETTINGS"
#define WIFI_AP_MODE_STR       "AP Mode (Hot Spot)"
#define WIFI_AP_NAME_STR       "AP (HOTSPOT) NAME"
#define WIFI_AP_REBOOT_STR     "AP REBOOT"
#define WIFI_AP_STA_STR        "AP/STA Mode"
#define WIFI_BLANK_MSG_STR     "LEAVE BLANK FOR AUTO LOGIN"
#define WIFI_CRED_SEP_STR      "WIFI ROUTER CREDENTIALS"
#define WIFI_DEV_USER_NM_STR   "LOGIN USER NAME"
#define WIFI_DEV_USER_PW_STR   "LOGIN USER PASSWORD"
#define WIFI_DEV_CRED_SEP_STR  "DEVICE LOGIN CREDENTIALS"
#define WIFI_DHCP_LBL_STR      " (DHCP)"
#define WIFI_DNS_STR           "WIFI DNS"
#define WIFI_GATEWAY_STR       "WIFI GATEWAY"
#define WIFI_IP_ADDR_STR       "STATIC IP ADDRESS"
#define WIFI_NAME_SEP_STR      "NAME SETTINGS"
#define WIFI_PASS_HIDE_STR     "{PASSWORD HIDDEN}"
#define WIFI_RSSI_STR          "RSSI"
#define WIFI_SSID_STR          "WIFI SSID"
#define WIFI_STATUS_SEP_STR    "WIFI STATUS"
#define WIFI_STATIC_LBL_STR    " (STATIC)"
#define WIFI_STA_MODE_STR      "Station Mode"
#define WIFI_STA_STATUS_STR    "STA / AP STATUS"
#define WIFI_SUBNET_STR        "SUBNET MASK"
#define WIFI_TAB_STR           "WiFi"
#define WIFI_UNKNOWN_STR       "(- ? -)"
#define WIFI_WEB_DHCP_STR      "WEBSERVER DHCP"
#define WIFI_WEBSRV_NAME_STR   "WEBSERVER NAME"
#define WIFI_WPA_KEY_STR       "WIFI WPA KEY"

#ifdef OTA_ENB
# define WIFI_MDNS_NAME_STR  "MDNS / OTA NAME"
#else // ifdef OTA_ENB
# define WIFI_MDNS_NAME_STR  "MDNS NAME"
#endif // ifdef OTA_ENB
