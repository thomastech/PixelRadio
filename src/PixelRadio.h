/*
   File: PixelRadio.h
   Project: PixelRadio, an RBDS/RDS FM Transmitter (QN8027 Digital FM IC)
   Version: 1.1.0
   Creation: Dec-16-2021
   Revised:  Jun-13-2022
   Revision History: See PixelRadio.cpp
   Project Leader: T. Black (thomastech)
   Contributors: thomastech

   (c) copyright T. Black 2021-2022, Licensed under GNU GPL 3.0 and later, under this license
   absolutely no warranty is given.
   This Code was formatted with the uncrustify extension.

    Note 1: All Text uses defines instead of const String. This saves ~30K Ram and ~50K Flash Memory.
 */

// *********************************************************************************************
#pragma once
#include <WiFi.h>
#include "config.h"
#include "credentials.h"
#include "ESPUI.h"

// *********************************************************************************************
// VERSION STRING: Must be updated with each public release.
// The version is shown during boot on Serial Log and on the "About" UI web Tab page.
#define VERSION_STR       "1.1.0"
#define AUTHOR_STR        "by Thomas Black"
#define BLD_DATE_STR      "Jun-13-2022"
#define GITHUB_REPO_STR   "<a href=\"https://github.com/thomastech/PixelRadio/\" target=\"_blank\">Click Here for Information</a>"

// *********************************************************************************************

/* Useful Macros for getting elapsed time */
#define numberOfSeconds(_time_) (_time_ % SECS_PER_MIN)
#define numberOfMinutes(_time_) ((_time_ / SECS_PER_MIN) % SECS_PER_MIN)
#define numberOfHours(_time_) ((_time_ % SECS_PER_DAY) / SECS_PER_HOUR)
#define elapsedDays(_time_) (_time_ / SECS_PER_DAY)

// Controller Command Limits
const uint8_t CMD_AUD_MAX_SZ  = 6;  // AUD Command Arg max length is 6 ("mono" / "stereo").
const uint8_t CMD_FREQ_MAX_SZ = 4;  // FREQ Command Arg max length is 4 (879 - 1079).
const uint8_t CMD_GPIO_MAX_SZ = 7;  // GPIO Cmd Code max length is 7 ("input/inputpd/inputpu/outhigh/outlow/read").
const uint8_t CMD_LOG_MAX_SZ  = 7;  // Serial Log Level Arg max length is 7 ("silent" / "restore");
const uint8_t CMD_MUTE_MAX_SZ = 3;  // MUTE Command Arg max length is 3 ("on" / "off").
const uint8_t CMD_PI_MAX_SZ   = 7;  // PI Command Arg max length is 6 (Examples, "ffff" and/or "0xffff"). Add +1 to trap typos.
const uint8_t CMD_PTY_MAX_SZ  = 3;  // PTY Command Arg max length is 3 (Example, "10" or "010").
const uint8_t CMD_PSN_MAX_SZ  = 8;  // PSN Command Arg max length for Program Service Name.
const uint8_t CMD_RDS_MAX_SZ  = 3;  // RDS Control Cmd Code length is 3 ("rds").
const uint8_t CMD_RF_MAX_SZ   = 3;  // RF Carrier Cmd Arg max length (Example "on" or "off").
const uint8_t CMD_RT_MAX_SZ   = 64; // RT Command Arg max Length for RadioText.
const uint8_t CMD_SYS_MAX_SZ  = 6;  // System Cmd Code length is 6 ("system").
const uint8_t CMD_TIME_MAX_SZ = 4;  // Time Command Arg length is 3 (5-900). Add +1 to trap typos.

// Controller Command Parameters
#define CMD_GPIO_OUT_HIGH_STR "outhigh"
#define CMD_GPIO_OUT_LOW_STR  "outlow"
#define CMD_GPIO_READ_STR     "read"

#define CMD_LOG_RST_STR    "restore"
#define CMD_LOG_SIL_STR    "silent"

#define CMD_MODE_MONO_STR  "mono"
#define CMD_MODE_STER_STR  "stereo"

#define CMD_MUTE_OFF_STR  "off"
#define CMD_MUTE_ON_STR   "on"

#define CMD_RDS_CODE_STR  "rds"

#define CMD_RF_OFF_STR    "off"
#define CMD_RF_ON_STR     "on"

#define CMD_SYS_CODE_STR  "system"

// Controller Command Keywords
#define  CMD_AUDMODE_STR     "aud"    // Radio Stereo / Mono Audio Mode.
#define  CMD_FREQ_STR        "freq"   // FM Transmit Frequency, value is MHz x10.
#define  CMD_GPIO_STR        "gpio"   // GPIO Control.
#define  CMD_GPIO19_STR      "gpio19" // GPIO Pin-19 Control.
#define  CMD_GPIO23_STR      "gpio23" // GPIO Pin-23 Control.
#define  CMD_GPIO33_STR      "gpio33" // GPIO Pin-33 Control.
#define  CMD_INFO_STR        "info"   // System Info.
#define  CMD_MUTE_STR        "mute"   // Radio Audio Mute, On/Off.
#define  CMD_PERIOD_STR      "rtper"  // RadioText Display Time Period, in seconds.
#define  CMD_PICODE_STR      "pic"    // RDS Program Information Code.
#define  CMD_PTYCODE_STR     "pty"    // RDS PTY Code.
#define  CMD_PSN_STR         "psn"    // RadioText Program Service Name, 8 chars max.
#define  CMD_REBOOT_STR      "reboot" // Reboot System.
#define  CMD_RF_CARRIER_STR  "rfc"    // RF Carrier, On/Off.
#define  CMD_RADIOTEXT_STR   "rtm"    // RadioText Message, 64 chars max.
#define  CMD_START_STR       "start"  // Start RDS using existing HTTP settings.
#define  CMD_STOP_STR        "stop"   // Stop HTTP Controller's RDS Transmission.

// Controller Flags
const bool AP_FALLBACK_DEF_FLG = true;
const bool CTRL_DHCP_DEF_FLG   = true;
const bool CTRL_LOCAL_DEF_FLG  = true;
const bool CTRL_HTTP_DEF_FLG   = true;
const bool CTRL_MQTT_DEF_FLG   = true;

// const bool CTRL_SERIAL_DEF_FLG = true;
const bool RADIO_MUTE_DEF_FLG  = false;
const bool RDS_TEXTX_DEF_FLG   = true;
const bool RF_AUTO_OFF_DEF_FLG = false;
const bool RF_CARRIER_DEF_FLG  = true;
const bool STEREO_ENB_DEF_FLG  = true;
const bool WIFI_REBOOT_DEF_FLG = false;

// Controller States
const uint8_t NO_CNTRL     = 0;
const uint8_t SERIAL_CNTRL = 1;
const uint8_t MQTT_CNTRL   = 2;
const uint8_t HTTP_CNTRL   = 3;
const uint8_t LOCAL_CNTRL  = 4;

// EEPROM: (Currently Not Used in PixelRadio)
const uint16_t EEPROM_SZ = 32;            // E2Prom Size, must be large enough to hold all values below.
const uint8_t  INIT_BYTE = 0xA5;          // E2Prom Initialization Stamping value.
const int INIT_ADDR      = 0;             // E2Prom Address for Init value, 8-bits.
const int XXXX_SET_ADDR  = 10;            // E2Prom Address for (unused), 16-bit.

// ESP32
const uint32_t ESP_BAUD_DEF     = 115200; // Default ESP32 Baud Rate.
const uint32_t ELAPSED_TMR_TIME = 1000;   // RDS Elapsed Time Update period, in mS. Do Not change.

// ESPUI (Web UI):
#define ANA_VOL_MIN_STR "0"
#define ANA_VOL_MAX_STR "100"
#define ANA_VOL_DEF_STR "50"
#define BAD_SENDER_STR  "BAD_SENDER_ID"
#define BAD_VALUE_STR   "BAD_VALUE"
#define COLOR_GRN_STR   "#66ff33"
#define COLOR_RED_STR   "#FF3500"
#define COLOR_BLK_STR   "#000000"
#define COLOR_WHT_STR   "#FFFFFF"
#define COLOR_GRY_STR   "#bfbfbf"
#define CSS_LABEL_STYLE_RED    "background-color: unset; color: red; margin-top: .1rem; margin-bottom: .1rem;"
#define CSS_LABEL_STYLE_BLACK  "background-color: unset; color: black; margin-top: .1rem; margin-bottom: .1rem;"
#define CSS_LABEL_STYLE_WHITE  "background-color: unset; color: white; margin-top: .1rem; margin-bottom: .1rem;"
#define CSS_LABEL_STYLE_MAROON "background-color: unset; color: Maroon; margin-top: .1rem; margin-bottom: .1rem;"

// #define CSS_LABEL_STYLE_RED   "background-color: unset; color: red; margin-top: -2px; margin-bottom: -3px;"
// #define CSS_LABEL_STYLE_BLACK "background-color: unset; color: black; margin-top: -2px; margin-bottom: -3px;"
// #define CSS_LABEL_STYLE_WHITE "background-color: unset; color: white; margin-top: -2px; margin-bottom: -3px;"
#define USB_VOL_MIN_STR "0"
#define USB_VOL_MAX_STR "30"
#define USB_VOL_DEF_STR USB_VOL_MAX_STR

// File System
#define  BACKUP_FILE_NAME "/backup.cfg"
#define  CRED_FILE_NAME   "/credentials.txt"
#define  LOGO_GIF_NAME    "/RadioLogo225x75_base64.gif" // Base64 gif file, 225 pixel W x 75 pixel H.
const uint8_t LITTLEFS_MODE = 1;
const uint8_t SD_CARD_MODE  = 2;

// FM Radio: QN8027 Test Codes
const uint8_t FM_TEST_OK   = 0;          // QN8027 Is Ok.
const uint8_t FM_TEST_VSWR = 1;          // QN8027 RF Out has Bad VSWR.
const uint8_t FM_TEST_FAIL = 2;          // QN8027 Chip Bad or missing.

// FM Radio: All FM Frequencies are X10.
const uint16_t FM_FREQ_DEF_X10 = 887;    // 88.7MHz FM.
const uint16_t FM_FREQ_MAX_X10 = 1079;   // 107.9MHz FM.
const uint16_t FM_FREQ_MIN_X10 = 881;    // 88.1MHz FM.
const uint16_t FM_FREQ_SKP_KHZ = 1;      // 100Khz.
const uint16_t FM_FREQ_SKP_MHZ = 10;     // 1MHz.

// FM Radio RF
const float PA_VOLT_MIN = 8.1f;          // Minimum allowed voltage for Power Amp, 9V -10%.
const float PA_VOLT_MAX = 9.9f;          // Maximum allowed voltage for Power Amp, 9V + 10%.
// Free Memory
const uint32_t FREE_MEM_UPD_TIME = 1750; // Update time for Free Memory (on diagTab), in mS.

// GPIO Pins:
// Note: GPIOs 34-39 do not support internal pullups or pulldowns.
const int GPIO19_PIN = 19;
const int GPIO23_PIN = 23;
const int GPIO33_PIN = 33;
const int MISO_PIN   = 2;  // SD Card D0, requires INPUT_PULLUP During SD Card Use.
const int MOSI_PIN   = 15; // SD Card CMD.
const int ON_AIR_PIN = 12;
const int SCL_PIN    = 22; // I2C Clock Pin, Output.
const int SDA_PIN    = 18; // I2C Data Pin, I/O.
const int SD_CLK_PIN = 14; // SD Card CLK.
const int SD_CS_PIN  = 13; // SD Card CS.
const int SER1_RXD   = 34; // Serial1 RxD Input, Optional 3.3V TTL, 5V tolerant.
const int SER1_TXD   = 32; // Serial1 TxD Output, 3.3V TTL.
const int MUX_PIN    = 21; // Audio Signal MUX Control, Output.
const int TONE_PIN   = 25; // PWM Test Tone Pin, Output.

// GPIO Pin States
const int SIGN_OFF = 0;
const int SIGN_ON  = 1;
const int TONE_OFF = 1;
const int TONE_ON  = 0;

// HTTP Controller
#define  HTTP_CMD_STR         "/cmd?"             // Command preamble.
#define  HTTP_EMPTY_RESP_STR  "get /favicon.ico"  // Empty Reply, ignore this payload.
#define  HTTP_POST_STR        "content-length:"   // Post data length Keyword.
#define  HTTP_CMD_END_STR     "http/"             // This string marks the End of HTTP Controller Command.


const uint16_t HTTP_RESPONSE_MAX_SZ = 225;        // Maximum Chars Allowed in HTTP client response.

// I2C:
const uint8_t  I2C_QN8027_ADDR = 0x2c;            // I2C Address of QN8027 FM Radio Chip.
const uint8_t  I2C_DEV_CNT     = 1;               // Number of expected i2c devices on bus.
const uint32_t I2C_FREQ_HZ     = 100000;          // I2C master clock frequency

// Measurement:
const int32_t MEAS_TIME = 50;                     // Measurement Refresh Time, in mS.

// MDNS & OTA
#define MDNS_NAME_DEF_STR "PixelRadio"            // Default MDNS and OTA WiFi Name.

// MQTT:
const uint8_t  MQTT_FAIL_CNT       = 10;          // Maximum failed MQTT reconnects before disabling MQTT.
const uint16_t MQTT_KEEP_ALIVE     = 90;          // MQTT Keep Alive Time, in Secs.
const int32_t  MQTT_MSG_TIME       = 30000;       // MQTT Periodic Message Broadcast Time, in mS.
const uint8_t  MQTT_NAME_MAX_SZ    = 18;
const uint8_t  MQTT_PAYLD_MAX_SZ   = 100;         // Must be larger than RDS_TEXT_MAX_SZ.
const uint8_t  MQTT_PW_MAX_SZ      = 48;
const int32_t  MQTT_RECONNECT_TIME = 30000;       // MQTT Reconnect Delay Time, in mS;
const uint8_t  MQTT_RETRY_CNT      = 6;           // MQTT Reconnection Count (max attempts).
const uint8_t  MQTT_TOPIC_MAX_SZ   = 45;
const uint8_t  MQTT_USER_MAX_SZ    = 48;
#define MQTT_NAME_DEF_STR "pixelradio"            // Default MQTT Topic / Subscription Name.

// MQTT Command
#define MQTT_CMD_STR       "/cmd/"                // Publish topic preamble, Client MQTT Command Preampble.

// MQTT Subscriptions
#define MQTT_CMD_SUB_STR   "/cmd/#"               // MQTT wildcard Subscription, receive all /cmd messages.

// MQTT Publish Topics
#define MQTT_CONNECT_STR "/connect"               // Publish topic, Client MQTT Subscription.
#define MQTT_GPIO_STR    "/gpio"                  // Publish topic, Client MQTT Subscription.
#define MQTT_INFORM_STR  "/info"                  // Publish topic, Client MQTT Subscription.
#define MQTT_VOLTS_STR   "/volts"                 // Publish topic, Client MQTT Subscription.

// Radio
const uint16_t AUDIO_LEVEL_MAX = 675;             // Maxium Audio Level (peak mV) Measurement by QN8027.
const uint32_t AUDIO_MEAS_TIME = 2000;            // Sample period for Audio Level Meas.
const uint8_t  RADIO_CAL_RETRY = 3;               // RF Port Calibration Retry Count (Maximum Retry Count).
const uint8_t  RF_LOW_POWER    = 27;              // 88dBuv.
const uint8_t  RF_MED_POWER    = 40;              // 96dBuV.
const uint8_t  RF_HIGH_POWER   = 78;              // 119dBuV. Do Not change.

// RDS:
const uint16_t RDS_DSP_TM_MAX   = 900;            // Maximum RadioText Display Period, in secs.
const uint16_t RDS_DSP_TM_MIN   = 5;              // Minimum RadioText Display Period, in secs.
const uint32_t RDS_DSP_TM_DEF   = 15000;          // Default Radio Display Period, in mS.
const uint16_t RDS_MSG_UPD_TIME = 1000;           // RadioText UI Update Time, im mS.
const uint16_t RDS_PI_CODE_DEF  = 0x6400;         // Default RDS PI Code, 16-bit hex value, 0x00ff - 0xffff.
const uint32_t RDS_PI_CODE_MAX  = 0xffff;         // Maximum PI Code Value (hex).
const uint32_t RDS_PI_CODE_MIN  = 0x00ff;         // Minumum PI Code Value (hex).
const uint8_t  RDS_PSN_MAX_SZ   = CMD_PSN_MAX_SZ; // RDS Program Service Name, Max Allowed Length.
const uint8_t  RDS_PTY_CODE_DEF = 9;              // Default RDS PTY Code "Top 40", 0-29 allowed.
const uint8_t  RDS_PTY_CODE_MIN = 0;              // Min RDS PTY Code "None", See https://en.wikipedia.org/wiki/Radio_Data_System
const uint8_t  RDS_PTY_CODE_MAX = 29;             // Max RDS PTY Code "Weather".
const uint8_t  RDS_REFRESH_TM   = 5;              // RadioText Refresh Time, in Seconds.
const uint8_t  RDS_TEXT_MAX_SZ  = CMD_RT_MAX_SZ;  // RDS RadioText Message, Max Allowed Length.

// RSSI:
const uint16_t RSSI_UPD_TIME = 2500;              // RSSI GUI Update time (on homeTab), in mS.

// Serial Controller
#define SERIAL_LOG_STR       "log"                // Serial Log Command Keyword.

// Test Tone
const uint8_t  TEST_TONE_CHNL = 0;                // Test Tone PWM Channel.
const uint32_t TEST_TONE_TIME = 300;              // Test Tone Sequence Time, in mS.
const uint16_t TONE_A3        = 220;              // 220Hz Audio Tone.
const uint16_t TONE_A4        = 440;
const uint16_t TONE_C4        = 262;
const uint16_t TONE_C5        = 523;
const uint16_t TONE_E4        = 330;
const uint16_t TONE_F4        = 349;
const uint16_t TONE_NONE      = 0;

// Time Conversion
const uint32_t MSECS_PER_SEC = 1000UL;
const uint32_t SECS_PER_HOUR = 3600UL;
const uint32_t SECS_PER_DAY  = SECS_PER_HOUR * 24UL;
const uint32_t SECS_PER_MIN  = 60UL;

// OTA:
const uint16_t OTA_PORT    = 3232;     // Port for OTA.
const uint16_t OTA_TIMEOUT = 3000;     // Max allowed time to receive OTA data.

// Volts:
const float MIN_VOLTS         = 4.5f;  // Minimum Power Supply volts.
const float VOLTS_HYSTERESIS  = 0.15f; // Voltage Hysterisis.
const uint16_t VOLTS_UPD_TIME = 3750;  // Power Supply Volts GUI Update time (on diagTab), in mS.
const uint32_t CLIENT_TIMEOUT = 500;   // Webserver Client Timeout, in mS.

// Web Server
#define HTML_HEADER_STR   "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"
#define HTML_DOCTYPE_STR  "<!DOCTYPE HTML>\r\n<html>"
#define HTML_CLOSE_STR    "</html>\r\n\r\n"

// WiFi:
const uint8_t  AP_NAME_MAX_SZ    = 18;
const uint16_t DNS_PORT          = 53;   // Webserver DNS port.
const uint16_t HTTP_PORT         = 8080; // Port for HTTP commands
const uint8_t  MAX_CON_FAIL_CNT  = 10;   // Max Allowed Connection Attempts before reboot (if WiFiRebootFlg=true)
const uint8_t  MDNS_NAME_MAX_SZ  = 18;
const uint8_t  PASSPHRASE_MAX_SZ = 48;
const uint8_t  SSID_MAX_SZ       = 32;   // Maximum permitted SSID Size according to standards.
const uint8_t  STA_NAME_MAX_SZ   = 18;
const uint8_t  USER_NM_MAX_SZ    = 10;
const uint8_t  USER_PW_MAX_SZ    = 10;
const uint8_t  WIFI_RETRY_CNT    = 20;    // Maximum number of WiFi connect attempts before AP mode.
const uint8_t  WL_AP_CONNECTED   = 8;     // Define locally, avoids needing WiFi101 library.
const uint16_t WEBSERVER_PORT    = 80;    // Port for Web Server. Do not change.
const int32_t  RECONNECT_TIME    = 75000; // WiFi Reconnect Time, in mS. Recommended minimum is 1 minute.
#define AP_NAME_DEF_STR  "PixelRadioAP"
#define STA_NAME_DEF_STR "PixelRadio"

/*
   WIFI_POWER_20dBm      // 20dBm NOT DEFINED! (Reset default, highest supply current ~150mA)
   WIFI_POWER_19_5dBm    // 19.5dBm, Highest Power available via API.
   WIFI_POWER_19dBm      // 19dBm
   WIFI_POWER_18_5dBm    // 18.5dBm
   WIFI_POWER_17dBm      // 17dBm
   WIFI_POWER_15dBm      // 15dBm
   WIFI_POWER_13dBm      // 13dBm
   WIFI_POWER_11dBm      // 11dBm
   WIFI_POWER_8_5dBm     // 8dBm
   WIFI_POWER_7dBm       // 7dBm
   WIFI_POWER_5dBm       // 5dBm
   WIFI_POWER_2dBm       // 2dBm
   WIFI_POWER_MINUS_1dBm // -1dBm output, lowest supply current ~120mA
   Power-Up Default Power value is 80 (19.5dBm).
 */
const wifi_power_t MAX_WIFI_PWR = WIFI_POWER_19_5dBm; // Use Max RF Power.
const wifi_power_t RUN_WIFI_PWR = WIFI_POWER_19_5dBm; // Use High RF Power during runtime.

// WiFi IP Addresses:
const IPAddress IP_ADDR_DEF     = { 0u, 0u, 0u, 0u };
const IPAddress WIFI_ADDR_DEF   = { 0u, 0u, 0u, 0u };
const IPAddress HOTSPOT_IP_DEF  = { 192u, 168u, 4u, 1u };
const IPAddress SUBNET_MASK_DEF = { 255u, 255u, 255u, 0u };

// *********************************************************************************************

// Controller Command Prototypes
bool    audioModeCmd(String  payloadStr,
                     uint8_t controller);
bool    frequencyCmd(String  payloadStr,
                     uint8_t controller);
bool    infoCmd(String  payloadStr,
                uint8_t controller);
int16_t getCommandArg(String& requestStr,
                      uint8_t maxSize);
String  getControllerName(uint8_t controller);
uint8_t getControllerStatus(void);
bool    gpioCmd(String  payloadStr,
                uint8_t controller,
                uint8_t pin);
bool    logCmd(String  payloadStr,
               uint8_t controller);
bool    muteCmd(String  payloadStr,
                uint8_t controller);
bool    piCodeCmd(String  payloadStr,
                  uint8_t controller);
bool    ptyCodeCmd(String  payloadStr,
                   uint8_t controller);
bool    programServiceNameCmd(String  payloadStr,
                              uint8_t Controller);
bool    radioTextCmd(String  payloadStr,
                     uint8_t controller);
bool    rdsTimePeriodCmd(String  payloadStr,
                         uint8_t controller);
bool    rebootCmd(String  payloadStr,
                  uint8_t controller);
bool    rfCarrierCmd(String  payloadStr,
                     uint8_t controller);
bool    startCmd(String  payloadStr,
                 uint8_t controller);
bool    stopCmd(String  payloadStr,
                uint8_t controller);

// ESPUI (WebGUI) Prototypes
void   buildGUI(void);
void   displayActiveController(uint8_t controller);
void   displayRdsText(void);
void   displaySaveWarning(void);
int8_t getAudioGain(void);
void   initCustomCss(void);
void   startGUI(void);
void   updateUiAudioLevel(void);
void   updateUiAudioMode(void);
void   updateUiAudioMute(void);
void   updateUiFreeMemory(void);
void   updateUiFrequency(void);
bool   updateUiGpioMsg(uint8_t pin,
                       uint8_t controller);
void   updateUiIpaddress(String ipStr);
void   updateUiLocalMsgTime(void);
void   updateUiLocalPiCode(void);
void   updateUiLocalPtyCode(void);
void   updateUiRdsText(String textStr);
void   updateUiRDSTmr(uint32_t rdsMillis);
void   updateUiRfCarrier(void);
void   updateUiRSSI(void);
void   updateUiDiagTimer(void);
void   updateUiVolts(void);
void   updateUiWfiMode(void);

// ESPUI Callbacks
void   adjFmFreqCallback(Control *sender,
                         int      type);
void   apBootCallback(Control *sender,
                      int      type);
void   apFallBkCallback(Control *sender,
                        int      type);
void   audioCallback(Control *sender,
                     int      type);
void   backupCallback(Control *sender,
                      int      type);
void   dhcpCallback(Control *sender,
                    int      type);
void   controllerCallback(Control *sender,
                          int      type);
void   diagBootCallback(Control *sender,
                        int      type);
void   diagLogCallback(Control *sender,
                       int      type);
void   gainAdjustCallback(Control *sender,
                          int      type);
void   gpioCallback(Control *sender,
                    int      type);
void   impAdjustCallback(Control *sender,
                         int      type);
void   muteCallback(Control *sender,
                    int      type);
void   radioEmphasisCallback(Control *sender,
                             int      type);
void   rdsDisplayTimeCallback(Control *sender,
                              int      type);
void   rdsEnbCallback(Control *sender,
                      int      type);
void   rdsRstCallback(Control *sender,
                      int      type);
void   rdsTextCallback(Control *sender,
                       int      type);
void   rfAutoCallback(Control *sender,
                      int      type);
void   rfCarrierCallback(Control *sender,
                         int      type);
void   rfPowerCallback(Control *sender,
                       int      type);
void   saveSettingsCallback(Control *sender,
                            int      type);
void   serialCallback(Control *sender,
                      int      type);
void   setLoginCallback(Control *sender,
                        int      type);
void   setMqttAuthenticationCallback(Control *sender,
                                     int      type);
#ifdef MQTT_ENB
void   setMqttIpAddrCallback(Control *sender,
                             int      type);
void   setMqttNameCallback(Control *sender,
                           int      type);
#endif // ifdef MQTT_ENB

void setPiCodeCallback(Control *sender,
                       int      type);
void setPtyCodeCallback(Control *sender,
                        int      type);
void setWiFiAuthenticationCallback(Control *sender,
                                   int      type);
void setWiFiNamesCallback(Control *sender,
                          int      type);
void setWiFiAddrsCallback(Control *sender,
                          int      type);

void testModeCallback(Control *sender,
                      int      type);

void volAdjustCallback(Control *sender,
                       int      type);

// File System (LITTLEFS) prototypes
void         instalLogoImageFile(void);
const String makeWebGif(String   fileName,
                        uint16_t width,
                        uint16_t height,
                        String   backGroundColorStr);
void littlefsInit(void);


// JSON Prototypes
bool checkEmergencyCredentials(const char *fileName);
bool restoreConfiguration(uint8_t     restoreMode,
                          const char *fileName);
bool saveConfiguration(uint8_t     saveMode,
                       const char *filename);

// Measure Prototypes
void         initVdcAdc(void);
void         initVdcBuffer(void);
void         measureVbatVoltage(void);
void         measureVdcVoltage(void);
void         processMeasurements(void);

// Misc Prototypes
const String addChipID(const char *name);
void         initEprom(void);
uint8_t      i2cScanner(void);
void         rebootSystem(void);
void         setGpioBootPins(void);
void         spiSdCardShutDown(void);
bool         strIsUint(String intStr);
void         toneInit(void);
void         toneOn(uint8_t  pin,
                    uint16_t freq,
                    uint8_t  channel);
void         toneOff(uint8_t pin,
                     uint8_t channel);
void         updateGpioBootPins(void);
void         updateTestTones(bool resetTimerFlg);

// MQTT Prototypes
void         mqttInit(void);
void         mqttReconnect(bool resetFlg);
void         mqttSendMessages(void);
void         mqttService(void);
void         processMQTT(void);
void         updateUiMqttMsg(String msgStr);
const String returnClientCode(int code);

// OTA Prototypes
void         otaInit(void);

// Radio Protypes
bool         calibrateAntenna(void);
bool         checkRadioIsPresent(void);
uint8_t      initRadioChip(void);
uint16_t     measureAudioLevel(void);
void         setAudioImpedance(void);
void         setAudioMute(void);
void         setDigitalGain(void);
void         setFrequency(void);
void         setMonoAudio(void);
void         setPreEmphasis(void);
void         setRfAutoOff(void);
void         setRfCarrier(void);
void         setRfPower(void);
void         setVgaGain(void);
void         updateOnAirSign(void);
void         updateRadioSettings(void);
void         waitForIdle(uint16_t waitMs);

// RDS Prototypes
bool         checkActiveTextAvail(void);
bool         checkControllerIsAvailable(uint8_t controller);
bool         checkLocalControllerAvail(void);
bool         checkLocalRdsAvail(void);
bool         checkControllerRdsAvail(void);
bool         checkRemoteRdsAvail(void);
bool         checkRemoteTextAvail(void);
void         processRDS(void);
void         resetControllerRdsValues(void);

// Serial Controller
bool         ctrlSerialFlg(void);
void         initSerialControl(void);
void         serialCommands(void);

// Serial Log
uint8_t      getLogLevel(void);
void         initSerialLog(bool verbose);

// webServer Prototypes
int8_t       getWifiMode(void);
int8_t       getRSSI(void);
void         processDnsServer(void);
void         processWebClient(void);
void         refresh_mDNS(void);
void         scanmDNS(void);
bool         wifiValidateSettings(void);
bool         wifiConnect(void);
void         wifiReconnect(void);
String       getWifiModeStr(void);
String       IpAddressToString(const IPAddress& ipAddress);
String       urlDecode(String urlStr);
uint8_t      urlDecodeHex(char c);
IPAddress    convertIpString(String ipStr);

// *********************************************************************************************
// EOF
