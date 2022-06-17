/*
   File: webserver.cpp
   Project: PixelRadio, an RBDS/RDS FM Transmitter (QN8027 Digital FM IC)
   Version: 1.1.0
   Creation: Dec-16-2021
   Revised:  Jun-13-2022
   Revision History: See PixelRadio.cpp
   Project Leader: T. Black (thomastech)
   Contributors: thomastech, dkulp

   (c) copyright T. Black 2021-2022, Licensed under GNU GPL 3.0 and later, under this license absolutely no warranty is given.
   This Code was formatted with the uncrustify extension.

   Notes:
   1. The HTTP GET server uses port 8080 (default).
   2. If host name is changed then FLASH must be fully erased before loading new code. Platformio command: pio run -t erase
   3. mDNS can be excluded from build, see config.h. Default mDNS access is 'PixelRadio.local", but can be changed in the WebUI.
   4. Use Android "Service Browser" app for mDNS Host name debugging. PixelRadio will be found in the android.tcp section.

   gif to base64 conversion tool (select Plain text -- just the Base64 Value):
   https://base64.guru/converter/encode/image/gif

 */

// *********************************************************************************************

#include <ArduinoLog.h>
#include <DNSServer.h>
#include <ESPmDNS.h>
#include <WiFi.h>
#include "ESPUI.h"
#include "config.h"
#include "PixelRadio.h"
#include "credentials.h"
#include "globals.h"
#include "language.h"

// ************************************************************************************************

DNSServer  dnsServer;
WiFiServer server(HTTP_PORT);      // WiFi WebServer object.
const uint16_t dnsPort = DNS_PORT; // Hot Spot AP DNS port.

// ************************************************************************************************
// convertIpString(): Convert IP String ("192.168.1.50") to IP class array dereference operators (192,168,1,50).
IPAddress convertIpString(String ipStr)
{
    IPAddress apip;

    if (apip.fromString(ipStr) == false) { // IPAddress invalid.
        apip = IPAddress(0, 0, 0, 0);      // Force to unused IP addr (use this as Error flag).
        // Log.infoln("Error: Unparsable IP, forced to ");
        // Log.infolnln(apip);
    }

    return apip;
}

// ************************************************************************************************
// IpAddressToString() Convert IP Address Class Array to String.
String IpAddressToString(const IPAddress& ipAddress) {
    String addrStr;

    addrStr = String(ipAddress[0]) + String(".") + \
              String(ipAddress[1]) + String(".") + \
              String(ipAddress[2]) + String(".") + \
              String(ipAddress[3]);

    return addrStr;
}

// ************************************************************************************************
// getCommandArg(): Get the Webserver's Command Argument. Result is returned as String passed by reference.
// Also returns String Length. If argument missing then returns -1;
#ifdef HTTP_ENB
int16_t getCommandArg(String& requestStr, uint8_t maxSize) {
    int16_t argStart = 0;
    int16_t argStop  = 0;
    String  argStr   = ""; // Copy of Argument String.
    String  argLcStr = ""; // Lowercase version.

    argLcStr = requestStr;
    argLcStr.toLowerCase();
    argStart = requestStr.indexOf("=");

    if (argStart >= 0) {
        argStop = argLcStr.indexOf(HTTP_CMD_END_STR); // Search for end of command data arg.

        if (argStop < 0) {
            argStop = requestStr.length();
        }
        argStr = requestStr.substring(argStart + 1, argStop - 1);
        argStr = urlDecode(argStr); // Convert any URL encoded text to ASCII.
//      argStr.replace("%20", " ");  // Replace the html space with ASCII text.
        argStr.trim();

        if (argStr.length() > maxSize) {
            argStr = argStr.substring(0, maxSize);
        }
        else if (argStr.length() == 0) {
            return -1; // Fail, Argument Missing.
        }

        argStr.trim(); // Trim one more time.
        requestStr = argStr;
    }
    else {             // Fail, Improper Argument Provided.
        requestStr = "";
        return -1;
    }

    return requestStr.length();
}

#endif // ifdef HTTP_ENB

// ************************************************************************************************
// getRSSI(): Get the RSSI value.
//            Note: AP Mode will always return 0 since it doesn't receive a sgnal from a router.
int8_t getRSSI(void)
{
    int16_t rssi;

    rssi = WiFi.RSSI();

    if (rssi < -127) {
        rssi = -127;
    }
    else if (rssi > 20) {
        rssi = 20;
    }

    return int8_t(rssi);
}

// ************************************************************************************************
// getWifiMode(): Return the wifi mode. Encapsulated wifi function for general use.
int8_t getWifiMode(void)
{
    wifi_mode_t mode;

    mode = WiFi.getMode();

    return mode;
}

// ************************************************************************************************
// getWifiModeStr(): Get the WiFi Mode. Returns String.
String getWifiModeStr(void)
{
    String wifiModeStr;
    wifi_mode_t mode;

    mode = WiFi.getMode();

    if (mode == WIFI_AP) {
        wifiModeStr = WIFI_AP_MODE_STR;
    }
    else if (mode == WIFI_STA) {
        wifiModeStr = WIFI_STA_MODE_STR;

        if (wifiDhcpFlg) {
            wifiModeStr +=  WIFI_DHCP_LBL_STR;
        }
        else {
            wifiModeStr +=  WIFI_STATIC_LBL_STR;
        }
    }
    else if (mode == WIFI_OFF) {
        wifiModeStr = WIFI_DISABLE_STR;
    }
    else if (mode == WIFI_AP_STA) {
        wifiModeStr = WIFI_AP_STA_STR;
    }
    else {
        wifiModeStr = WIFI_UNKNOWN_STR;
    }

    return wifiModeStr;
}

// *************************************************************************************************************************
// gpioHttpControl(): HTTP handler for GPIO Commands. This is a companion to processWebClient().
#ifdef HTTP_ENB
void gpioHttpControl(WiFiClient client, String requestStr, uint8_t pin)
{
    bool successFlg = true;
    char charBuff[60];

    sprintf(charBuff, "-> HTTP Controller: Received GPIO Pin-%d Command", pin);
    Log.infoln(charBuff);

    successFlg = gpioCmd(requestStr, HTTP_CNTRL, pin);

    client.print(HTML_HEADER_STR);
    client.print(HTML_DOCTYPE_STR);

    if (!successFlg) {
        sprintf(charBuff, "{\"%s%d\": \"fail\"}", CMD_GPIO_STR, pin);
    }
    else if (requestStr == CMD_GPIO_READ_STR) {
        sprintf(charBuff, "{\"%s%d\": \"%d\"}", CMD_GPIO_STR, pin, digitalRead(pin));
    }
    else {
        sprintf(charBuff, "{\"%s%d\": \"ok\"}", CMD_GPIO_STR, pin);
    }

    client.print(charBuff);
    client.println(HTML_CLOSE_STR);
}

#endif // ifdef HTTP_ENB

// ************************************************************************************************
// makeHttpCmdStr(): Return the HTTP command string. On entry cmdStr has command keyword.
//                   This is a companion to processWebClient().
#ifdef HTTP_ENB
String makeHttpCmdStr(String cmdStr) {
    cmdStr = String(HTTP_CMD_STR) + cmdStr;

    return cmdStr;
}

#endif // ifdef HTTP_ENB

// ************************************************************************************************
// processWebClient(): Web Client Handler. Must be installed in main loop.
// URL Example: http://pixelradio.local:8080/cmd?aud=mono
#ifdef HTTP_ENB
void processWebClient(void)
{
    static bool connectFlg = false;
    bool successFlg        = true;
    uint16_t charCnt       = 0;
    uint32_t previousTime  = millis();

    String argStr       = "";
    String currentLine  = "";
    String requestStr   = "";               // Var to capture the HTTP request reply.
    String requestLcStr = "";               // Var to store the lower case HTTP request reply.

    WiFiClient client = server.available(); // Listen for incoming web clients.

    if (client) {                           // New client connection (web page access).
        requestStr.reserve(HTTP_RESPONSE_MAX_SZ + 10);
        requestLcStr.reserve(HTTP_RESPONSE_MAX_SZ + 10);

        if (!connectFlg) {
            Log.infoln("HTTP Controller: New Client");
            previousTime = millis();
        }

        if (!client.available()) { // If no bytes available then this is a Browser Prefetch.
            Log.infoln("-> HTTP Controller: Empty Prefetch, Close Connection.");
            client.stop();
            return;
        }

        connectFlg  = true;
        currentLine = ""; // Init string to hold incoming data from the client,

        // while (client.connected()) { // loop while the client is connected.
        while (client.connected() && millis() - previousTime <= CLIENT_TIMEOUT) {
            if (client.available()) {
                char c = client.read();

                if (charCnt < HTTP_RESPONSE_MAX_SZ) {
                    // Serial.write(c); // DEBUG ONLY: Log saved URI chars to serial monitor.
                    requestStr += c; // Build URI string.
                    charCnt++;
                }

                if (c == '\n') {
                    // if the current line is zero, you got two newline characters in a row.
                    // that's the end of the client HTTP request body, so check for the
                    // optional post data. Then process the command.
                    if (currentLine.length() == 0) {
                        // Log.verboseln(requestStr.c_str());
                        requestLcStr = requestStr;
                        requestLcStr.toLowerCase();

                        // ************ PROCESS POST DATA (if any) ***************
                        uint16_t contentLenIndex = requestLcStr.indexOf(HTTP_POST_STR);

                        if (contentLenIndex > 0) {
                            String lenStr = requestLcStr.substring(contentLenIndex + sizeof(HTTP_POST_STR));

                            if (lenStr.toInt() > 0) {
                                Log.verboseln("-> HTTP CMD: Found Post Data");

                                uint16_t endCmdIndex = requestLcStr.indexOf(HTTP_CMD_END_STR);

                                if (endCmdIndex > 0) {
                                    requestStr =  requestStr.substring(0, endCmdIndex);
                                }

                                while (client.available() && charCnt < HTTP_RESPONSE_MAX_SZ) {
                                    char c = client.read();

                                    if ((c == '\r') || (c == '\n')) {
                                        break;
                                    }

                                    // Serial.write(c); // DEBUG ONLY: Log saved URI chars to serial monitor.
                                    requestStr += c; // Build URI string.
                                    charCnt++;
                                }
                                requestStr  += ' ';  // Must pad end of Post data with Space (needed by cmd processor).
                                requestLcStr = requestStr;
                                requestLcStr.toLowerCase();
                            }
                        }

                        // ************ NO COMMAND, EMPTY PAYLOAD ***************
                        if (requestLcStr.indexOf(HTTP_EMPTY_RESP_STR) == 0) {
                            Log.verboseln("-> HTTP CMD: Empty Payload, Ignored");
                            break;
                        }

                        // ************ AUDIO MODE COMMAND ***************
                        else if (requestLcStr.indexOf(makeHttpCmdStr(CMD_AUDMODE_STR)) > 0) {
                            Log.infoln("-> HTTP CMD: Audio Mode");

                            if (getCommandArg(requestStr, CMD_AUD_MAX_SZ) == -1) {
                                successFlg = false;
                                Log.errorln("-> HTTP CMD: Audio Mode Missing Value (abort).");
                            }
                            else {
                                if (audioModeCmd(requestStr, HTTP_CNTRL) == false) {
                                    successFlg = false;
                                }
                            }

                            client.print(HTML_HEADER_STR);
                            client.print(HTML_DOCTYPE_STR);

                            if (successFlg) {
                                client.printf("{\"%s\": \"ok\"}\r\n", CMD_AUDMODE_STR);
                            }
                            else {
                                client.printf("{\"%s\": \"fail\"}\r\n", CMD_AUDMODE_STR);
                            }
                            client.println(HTML_CLOSE_STR);
                        }

                        // ************ GPIO19 COMMAND ***************
                        else if (requestLcStr.indexOf(makeHttpCmdStr(CMD_GPIO19_STR)) > 0) {
                            Log.infoln("-> HTTP CMD: GPIO19");

                            if (getCommandArg(requestStr, CMD_GPIO_MAX_SZ) == -1) {
                                Log.errorln("-> HTTP CMD: GPIO Control Missing Value (abort).");
                            }
                            else {
                                gpioHttpControl(client, requestStr, GPIO19_PIN);
                            }
                        }

                        // ************ GPIO23 COMMAND ***************
                        else if (requestLcStr.indexOf(makeHttpCmdStr(CMD_GPIO23_STR)) > 0) {
                            Log.infoln("-> HTTP CMD: GPIO23");

                            if (getCommandArg(requestStr, CMD_GPIO_MAX_SZ) == -1) {
                                Log.errorln("-> HTTP CMD: GPIO Control Missing Value (abort).");
                            }
                            else {
                                gpioHttpControl(client, requestStr, GPIO23_PIN);
                            }
                        }

                        // ************ GPIO33 COMMAND ***************
                        else if (requestLcStr.indexOf(makeHttpCmdStr(CMD_GPIO33_STR)) > 0) {
                            Log.infoln("-> HTTP CMD: GPIO33");

                            if (getCommandArg(requestStr, CMD_GPIO_MAX_SZ) == -1) {
                                Log.errorln("-> HTTP CMD: GPIO Control Missing Value (abort).");
                            }
                            else {
                                gpioHttpControl(client, requestStr, GPIO33_PIN);
                            }
                        }

                        // ************ INFO COMMAND ***************
                        else if (requestLcStr.indexOf(makeHttpCmdStr(CMD_INFO_STR)) >= 0) {
                            Log.infoln("-> HTTP CMD: INFO");

                            if (getCommandArg(requestStr, CMD_SYS_MAX_SZ) == -1) {
                                successFlg = false;
                                Log.errorln("-> HTTP CMD: Info Missing Value (abort).");
                            }
                            else {
                                if (infoCmd(requestStr, HTTP_CNTRL) == false) {
                                    successFlg = false;
                                }
                            }

                            client.print(HTML_HEADER_STR);
                            client.print(HTML_DOCTYPE_STR);

                            if (successFlg) {
                                client.printf(
                                    "{\"%s\": \"ok\", \"version\": \"%s\", \"hostName\": \"%s\", \"ip\": \"%s\", \"rssi\": %d, \"status\": \"0x%02X\"}\r\n",
                                    CMD_INFO_STR,
                                    VERSION_STR,
                                    staNameStr.c_str(),
                                    WiFi.localIP().toString().c_str(),
                                    WiFi.RSSI(),
                                    getControllerStatus());
                            }
                            else {
                                client.printf("{\"%s\": \"fail\"}\r\n", CMD_INFO_STR);
                            }
                            client.println(HTML_CLOSE_STR);
                        }

                        // ************ FREQUENCY COMMAND ***************
                        else if (requestLcStr.indexOf(makeHttpCmdStr(CMD_FREQ_STR)) >= 0) {
                            Log.infoln("-> HTTP CMD: FREQUENCY");

                            if (getCommandArg(requestStr, CMD_FREQ_MAX_SZ) == -1) {
                                successFlg = false;
                                Log.errorln("-> HTTP CMD: Frequency Missing Value (abort).");
                            }
                            else {
                                if (frequencyCmd(requestStr, HTTP_CNTRL) == false) {
                                    successFlg = false;
                                }
                            }
                            client.print(HTML_HEADER_STR);
                            client.print(HTML_DOCTYPE_STR);

                            if (successFlg) {
                                client.printf("{\"%s\": \"ok\"}\r\n", CMD_FREQ_STR);
                            }
                            else {
                                client.printf("{\"%s\": \"fail\"}\r\n", CMD_FREQ_STR);
                            }
                            client.println(HTML_CLOSE_STR);
                        }

                        // ************ MUTE COMMAND ***************
                        else if (requestLcStr.indexOf(makeHttpCmdStr(CMD_MUTE_STR)) >= 0) {
                            Log.infoln("-> HTTP CMD: MUTE");

                            if (getCommandArg(requestStr, CMD_MUTE_MAX_SZ) == -1) {
                                successFlg = false;
                                Log.errorln("-> HTTP CMD: Mute Missing Value (abort).");
                            }
                            else {
                                if (muteCmd(requestStr, HTTP_CNTRL) == false) {
                                    successFlg = false;
                                }
                            }
                            client.print(HTML_HEADER_STR);
                            client.print(HTML_DOCTYPE_STR);

                            if (successFlg) {
                                client.printf("{\"%s\": \"ok\"}\r\n", CMD_MUTE_STR);
                            }
                            else {
                                client.printf("{\"%s\": \"fail\"}\r\n", CMD_MUTE_STR);
                            }
                            client.println(HTML_CLOSE_STR);
                        }

                        // ************* PI CODE COMMAND ****************
                        else if (requestLcStr.indexOf(makeHttpCmdStr(CMD_PICODE_STR)) >= 0) {
                            Log.infoln("-> HTTP CMD: PI (Program ID) CODE");

                            if (getCommandArg(requestStr, CMD_PI_MAX_SZ) == -1) {
                                successFlg = false;
                                Log.errorln("-> HTTP CMD: PI Code Missing Value (abort).");
                            }
                            else {
                                if (piCodeCmd(requestStr, HTTP_CNTRL) == false) {
                                    successFlg = false;
                                }
                            }
                            client.print(HTML_HEADER_STR);
                            client.print(HTML_DOCTYPE_STR);

                            if (successFlg) {
                                client.printf("{\"%s\": \"ok\"}\r\n", CMD_PICODE_STR);
                            }
                            else {
                                client.printf("{\"%s\": \"fail\"}\r\n", CMD_PICODE_STR);
                            }
                            client.println(HTML_CLOSE_STR);
                        }

                        // ************* PTY CODE COMMAND ****************
                        else if (requestLcStr.indexOf(makeHttpCmdStr(CMD_PTYCODE_STR)) >= 0) {
                            Log.infoln("-> HTTP CMD: PTY CODE");

                            if (getCommandArg(requestStr, CMD_PTY_MAX_SZ) == -1) {
                                successFlg = false;
                                Log.errorln("-> HTTP CMD: PTY Code Missing Value (abort).");
                            }
                            else {
                                if (ptyCodeCmd(requestStr, HTTP_CNTRL) == false) {
                                    successFlg = false;
                                }
                            }
                            client.print(HTML_HEADER_STR);
                            client.print(HTML_DOCTYPE_STR);

                            if (successFlg) {
                                client.printf("{\"%s\": \"ok\"}\r\n", CMD_PTYCODE_STR);
                            }
                            else {
                                client.printf("{\"%s\": \"fail\"}\r\n", CMD_PTYCODE_STR);
                            }
                            client.println(HTML_CLOSE_STR);
                        }

                        // ********PROGRAM SERVICE NAME COMMAND *********
                        else if (requestLcStr.indexOf(makeHttpCmdStr(CMD_PSN_STR)) >= 0) {
                            Log.infoln("-> HTTP CMD: PSN (Program Service Name)");

                            if (getCommandArg(requestStr, RDS_PSN_MAX_SZ) == -1) {
                                successFlg = false;
                                Log.errorln("-> HTTP CMD: PSN Missing Value, (abort).");
                            }
                            else {
                                if (programServiceNameCmd(requestStr, HTTP_CNTRL) == false) {
                                    successFlg = false;
                                }
                            }
                            client.print(HTML_HEADER_STR);
                            client.print(HTML_DOCTYPE_STR);

                            if (successFlg) {
                                client.printf("{\"%s\": \"ok\"}\r\n", CMD_PSN_STR);
                            }
                            else {
                                client.printf("{\"%s\": \"fail\"}\r\n", CMD_PSN_STR);
                            }
                            client.println(HTML_CLOSE_STR);
                        }

                        // ************ RT COMMAND ***************
                        // To clear RadioText display send %20 as payload.
                        else if (requestLcStr.indexOf(makeHttpCmdStr(CMD_RADIOTEXT_STR)) >= 0) {
                            Log.infoln("-> HTTP CMD: RTM (RadioText Message)");

                            if (getCommandArg(requestStr, RDS_TEXT_MAX_SZ) == -1) {
                                successFlg = false;
                                Log.errorln("-> HTTP CMD: RTM, Missing RadioText Message, Ignored.");
                            }
                            else {
                                if (radioTextCmd(requestStr, HTTP_CNTRL) == false) {
                                    successFlg = false;
                                }
                            }
                            client.print(HTML_HEADER_STR);
                            client.print(HTML_DOCTYPE_STR);

                            if (successFlg) {
                                client.printf("{\"%s\": \"ok\"}\r\n", CMD_RADIOTEXT_STR);   // JSON Fmt.
                            }
                            else {
                                client.printf("{\"%s\": \"fail\"}\r\n", CMD_RADIOTEXT_STR); // JSON Fmt.
                            }
                            client.println(HTML_CLOSE_STR);
                        }

                        // ************ RFC COMMAND ***************
                        else if (requestLcStr.indexOf(makeHttpCmdStr(CMD_RF_CARRIER_STR)) >= 0) {
                            Log.infoln("-> HTTP CMD: RFC (RF Carrier Control)");

                            if (getCommandArg(requestStr, CMD_RF_MAX_SZ) == -1) {
                                successFlg = false;
                                Log.errorln("-> HTTP CMD: RFC, Missing Value (abort).");
                            }
                            else {
                                if (rfCarrierCmd(requestStr, HTTP_CNTRL) == false) {
                                    successFlg = false;
                                }
                            }
                            client.print(HTML_HEADER_STR);
                            client.print(HTML_DOCTYPE_STR);

                            if (successFlg) {
                                client.printf("{\"%s\": \"ok\"}\r\n", CMD_RF_CARRIER_STR);   // JSON Fmt.
                            }
                            else {
                                client.printf("{\"%s\": \"fail\"}\r\n", CMD_RF_CARRIER_STR); // JSON Fmt.
                            }
                            client.println(HTML_CLOSE_STR);
                        }

                        // ************ REBOOT COMMAND ***************
                        else if (requestLcStr.indexOf(makeHttpCmdStr(CMD_REBOOT_STR)) >= 0) {
                            Log.infoln("-> HTTP CMD: REBOOT");

                            if (getCommandArg(requestStr, CMD_SYS_MAX_SZ) == -1) {
                                successFlg = false;
                                Log.errorln("-> HTTP CMD: REBOOT, Missing Value (abort).");
                            }
                            else {
                                if (rebootCmd(requestStr, HTTP_CNTRL) == false) {
                                    successFlg = false;
                                }
                            }

                            client.print(HTML_HEADER_STR);
                            client.print(HTML_DOCTYPE_STR);

                            if (successFlg) {
                                client.printf("{\"%s\": \"ok\"}\r\n", CMD_REBOOT_STR);   // JSON Fmt.
                            }
                            else {
                                client.printf("{\"%s\": \"fail\"}\r\n", CMD_REBOOT_STR); // JSON Fmt.
                            }

                            client.println(HTML_CLOSE_STR);
                        }

                        // ************** START COMMAND ***************
                        else if (requestLcStr.indexOf(makeHttpCmdStr(CMD_START_STR)) >= 0) {
                            Log.infoln("-> HTTP CMD: START");

                            if (getCommandArg(requestStr, CMD_RDS_MAX_SZ) == -1) {
                                successFlg = false;
                                Log.errorln("-> HTTP CMD: START, Missing Value (abort).");
                            }
                            else {
                                if (startCmd(requestStr, HTTP_CNTRL) == false) {
                                    successFlg = false;
                                }
                            }

                            client.print(HTML_HEADER_STR);
                            client.print(HTML_DOCTYPE_STR);

                            if (successFlg) {
                                client.printf("{\"%s\": \"ok\"}\r\n", CMD_START_STR);   // JSON Fmt.
                            }
                            else {
                                client.printf("{\"%s\": \"fail\"}\r\n", CMD_START_STR); // JSON Fmt.
                            }

                            client.println(HTML_CLOSE_STR);
                        }

                        // **************** STOP COMMAND ****************
                        else if (requestLcStr.indexOf(makeHttpCmdStr(CMD_STOP_STR)) >= 0) {
                            Log.infoln("-> HTTP CMD: STOP");

                            if (getCommandArg(requestStr, CMD_RDS_MAX_SZ) == -1) {
                                successFlg = false;
                                Log.errorln("-> HTTP CMD: STOP, Missing Value (abort).");
                            }
                            else {
                                if (stopCmd(requestStr, HTTP_CNTRL) == false) {
                                    successFlg = false;
                                }
                            }

                            client.print(HTML_HEADER_STR);
                            client.print(HTML_DOCTYPE_STR);

                            if (successFlg) {
                                client.printf("{\"%s\": \"ok\"}\r\n", CMD_STOP_STR);   // JSON Fmt.
                            }
                            else {
                                client.printf("{\"%s\": \"fail\"}\r\n", CMD_STOP_STR); // JSON Fmt.
                            }

                            client.println(HTML_CLOSE_STR);
                        }

                        // ************* TIME PERIOID COMMAND ************
                        else if (requestLcStr.indexOf(makeHttpCmdStr(CMD_PERIOD_STR)) >= 0) {
                            Log.infoln("-> HTTP CMD: RTPER (RadioText Time Period)");

                            if (getCommandArg(requestStr, CMD_TIME_MAX_SZ) == -1) {
                                successFlg = false;
                                Log.errorln("-> HTTP CMD: RTPER, Missing Value (abort).");
                            }
                            else {
                                if (rdsTimePeriodCmd(requestStr, HTTP_CNTRL) == false) {
                                    successFlg = false;
                                }
                            }

                            client.print(HTML_HEADER_STR);
                            client.print(HTML_DOCTYPE_STR);

                            if (successFlg) {
                                client.printf("{\"%s\": \"ok\"}\r\n", CMD_PERIOD_STR);   // JSON Fmt.
                            }
                            else {
                                client.printf("{\"%s\": \"fail\"}\r\n", CMD_PERIOD_STR); // JSON Fmt.
                            }

                            client.println(HTML_CLOSE_STR);
                        }

                        // ************ UNKNOWN COMMAND ***************
                        else {
                            Log.errorln("-> HTTP CMD: COMMAND IS UNDEFINED");
                            client.print(HTML_HEADER_STR);
                            client.print(HTML_DOCTYPE_STR);
                            client.println("{\"cmd\": \"undefined\"}\r\n"); // JSON Fmt.
                            client.println(HTML_CLOSE_STR);
                        }

                        // Break out of the while loop
                        break;
                    }
                    else { // if you got a newline, then clear currentLine
                        currentLine = "";
                    }
                }
                else if (c != '\r') { // if you got anything else but a carriage return character,
                    currentLine += c; // add it to the end of the currentLine
                }
            }
        }
        requestStr   = ""; // Clear the reply request variable.
        requestLcStr = "";

        connectFlg = false;
        client.stop();     // Close the GET HTTP connection.
        Log.infoln("-> HTTP Controller: Client Disconnected.");
    }
    else if (connectFlg) { // Client was connected, but now nothing to do.
        connectFlg = false;
        client.stop();     // Close the GET HTTP connection.
        Log.infoln("-> HTTP Controller: Connected Client Now Idle, Disconnected).");
    }
}

#endif // ifdef HTTP_ENB

// ************************************************************************************************
// processDnsServer(): DNS Server for AP hot spot. Must be called in main loop.
void processDnsServer(void)
{
    dnsServer.processNextRequest();
}

// *********************************************************************************************
// urlDecode(): Convert URL encoding into ASII.
String urlDecode(String urlStr)
{
    String encodeStr = "";
    char   c;
    char   code0;
    char   code1;

    for (int i = 0; i < urlStr.length(); i++) {
        c = urlStr.charAt(i);

        if (c == '%') {
            code0 = urlStr.charAt(++i);
            code1 = urlStr.charAt(++i);
            c = (urlDecodeHex(code0) << 4) | urlDecodeHex(code1);
            encodeStr += c;
        }
        else if (c == '+') {
            encodeStr += ' ';
        }
        else {
            encodeStr += c;
        }
    }

    return encodeStr;
}

// *********************************************************************************************
// urlDecodeHex(): convert hex to integer base. This is companion function for urlDecode().
unsigned char urlDecodeHex(char c)
{
    if (c >= '0' && c <='9'){
        return((unsigned char)c - '0');
    }
    if (c >= 'a' && c <='f'){
        return((unsigned char)c - 'a' + 10);
    }
    if (c >= 'A' && c <='F'){
        return((unsigned char)c - 'A' + 10);
    }

    return(0);
}

// ************************************************************************************************
// wifiReconnect(): If WiFI not connected, or in AP mode, then peridoically atttempt a STA reconnect.
void wifiReconnect(void)
{
    uint8_t apCount = 0;
    char    charBuff[40];
    static uint32_t previousWiFiMillis = 0; // Timer for WiFi services.

    apCount = WiFi.softAPgetStationNum();

    if (previousWiFiMillis == 0) {
        previousWiFiMillis = millis();
    }
    else if (millis() - previousWiFiMillis >= RECONNECT_TIME) {
        if ((WiFi.status() != WL_CONNECTED) && (apCount == 0)) {
            Log.infoln("Attempting to Reconnect WiFi.");
            wifiConnect();
            #ifdef MQTT_ENB
            mqttReconnect(false);
            #endif // ifdef MQTT_ENB
        }
        else {
            sprintf(charBuff, "WiFi Status: %s.", getWifiModeStr().c_str());
            Log.infoln(charBuff);
            updateUiWfiMode(); // Update the User Interface WiFi Mode Status Text.

            if (apCount > 0) {
                sprintf(charBuff, " AP HotSpot Connections: %u", apCount);
                Log.infoln(charBuff);
            }
        }

        if (WiFi.status() == WL_CONNECTED) {
            ipAddrStr = WiFi.localIP().toString().c_str();
            updateUiIpaddress(ipAddrStr);
        }

        previousWiFiMillis = millis(); // Reset Reconnect Period.
    }
}

// ************************************************************************************************
bool wifiConnect(void)
{
    char logBuff[SSID_MAX_SZ + 60];
    char nameBuff[SSID_MAX_SZ + 1];
    char pskBuff[sizeof(AP_PSK_STR) + 1];
    bool apFlg              = true;
    bool success            = true;
    static bool dnsStartFlg = false;
    uint8_t     retryCnt    = 0;
    static uint8_t failCnt  = 0; // If Failed STA connect count too high then reboot then reboot.
    String tmpNameStr       = "";

    Log.infoln("Starting WiFi (Webserver Initialization):");

    // WiFi.setTxPower(MAX_WIFI_PWR);

    // Configure static IP address if DHCP turned off.
    if (!wifiDhcpFlg) { // Static IP Mode Enabled.
        if (wifiValidateSettings() == true) {
            if (WiFi.config(staticIP, wifiGateway, subNet, wifiDNS)) {
                sprintf(logBuff, "-> Static IP Connection Enabled, IP Address: %s", (IpAddressToString(staticIP)).c_str());
                Log.infoln(logBuff);
            }
            else {
                WiFi.config(0u, 0u, 0u); // Force DHCP mode.
                Log.warningln("-> Static IP Connection Failed, Will Use DHCP");
            }
        }
        else {
            Log.errorln("-> Invalid Static IP Settings, Will Use DHCP");
        }
    }
    else {
        Log.infoln("-> DHCP Connection Enabled");
    }

    // WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE); // Workaround for setting host name. (Dec-22-2021 bug, see
    // https://github.com/espressif/arduino-esp32/issues/4732)
    if (staNameStr.length() == 0) {
        staNameStr = STA_NAME_DEF_STR;
        sprintf(logBuff, "-> STA Name Missing, Using %s", staNameStr.c_str());
        Log.warningln(logBuff);
    }
    WiFi.setHostname(staNameStr.c_str()); // MUST set the host name (for WiFi router) BEFORE wifi.mode().
    WiFi.mode(WIFI_STA);                  // Enable WiFi.
    delay(100);                           // Wait for SYSTEM_EVENT_AP_START to occur.

    // Connect to Wi-Fi network with SSID and password
    sprintf(logBuff, "-> Connecting to %s ...", wifiSSIDStr.c_str());
    Log.infoln(logBuff);

    WiFi.setSleep(false);
    WiFi.begin(wifiSSIDStr.c_str(), wifiWpaKeyStr.c_str());

    while (WiFi.status() != WL_CONNECTED) {
        if (retryCnt++ > WIFI_RETRY_CNT) {
            Log.errorln("-> WiFi Connection Failed.");
            success = false;
            break;
        }
        else {
            sprintf(logBuff, "-> Connection Retry #%u", retryCnt);
            Log.infoln(logBuff);
        }
        delay(1000);
    }

    if (success) {
        failCnt   = 0;
        ipAddrStr = WiFi.localIP().toString().c_str();
        updateUiIpaddress(ipAddrStr);

        WiFi.setAutoReconnect(false);
        WiFi.persistent(false);

        if (dnsStartFlg) { // Previously AP Mode, Now STA. Cancel DNS Service.
            dnsStartFlg = false;
            dnsServer.stop();
            Log.warningln("-> DNS Server Terminated.");
        }

        // The mDNS initialization is also handled by the ArduinoOTA.begin() function. This code block is a duplicate for MDNS only.
        #ifdef MDNS_ENB

        if (mdnsNameStr.length() == 0) {
            mdnsNameStr = MDNS_NAME_DEF_STR;
            sprintf(logBuff, "-> MDNS Name Missing, Using %s", mdnsNameStr.c_str());
            Log.warningln(logBuff);
        }

        if (!MDNS.begin(mdnsNameStr.c_str())) { // ArduinoOTA.setHostname() MUST use the same name!
            Log.errorln("-> Error starting mDNS; Service is disabled.");
        }
        else {
            Log.infoln("-> Server mDNS has started");
            sprintf(logBuff, "-> Open http://%s.local in your browser", mdnsNameStr.c_str());
            Log.infoln(logBuff);
        }
        #endif // ifdef MDNS_ENB

        // Log.traceln("WiFi Transmit Power Value = %d", WiFi.getTxPower());
        // WiFi.setTxPower(RUN_WIFI_PWR);

        // Print local IP address and start web server
        sprintf(logBuff, "-> WiFi connected, IP address: %s, RSSI: %ddBm", WiFi.localIP().toString().c_str(), WiFi.RSSI());
        Log.infoln(logBuff);
        server.begin(); // Start HTTP GET server.

        #ifdef MDNS_ENB
        MDNS.addService("http", "tcp", WEBSERVER_PORT);
        MDNS.addServiceTxt("http", "tcp", "arduino", mdnsNameStr);
        #endif // ifdef MDNS_ENB

        Log.infoln("WiFi Initialization Complete (Online).");
    }
    else {
        Log.warningln("-> WiFi Station Connection Failed");

        if (apFallBackFlg) {
            Log.warningln("Will Attempt to create AP Connection (HotSpot).");
            WiFi.mode(WIFI_AP);
            delay(100); // Not needed if 192.168.4.1 is used. See https://github.com/espressif/arduino-esp32/issues/3906
            WiFi.setSleep(false);
            WiFi.softAPConfig(hotSpotIP, hotSpotIP, subNet);

            // Uncomment ADD_CHIP_ID in config.h to create AP Host name with ESP32's Chip-ID Number (optional).
            if (apNameStr.length() == 0) {
                apNameStr = AP_NAME_DEF_STR;
                sprintf(logBuff, "-> HotSpot Name Missing, Using %s", apNameStr.c_str());
                Log.warningln(logBuff);
            }

            #ifdef ADD_CHIP_ID
            tmpNameStr = addChipID(apNameStr.c_str()); // Append ESP32 Chip ID to Host Name.
            #else // ifdef ADD_CHIP_ID
            tmpNameStr = apNameStr;
            #endif // ifdef ADD_CHIP_ID
            tmpNameStr.toCharArray(nameBuff, tmpNameStr.length() + 1);

            sprintf(logBuff, "-> HotSpot Host Name: %s", nameBuff);
            Log.infoln(logBuff);

            #ifdef REQUIRE_WIFI_AP_PSK
            Log.infoln("-> HotSpot Requires PSK");
            sprintf(pskBuff, "%s", AP_PSK_STR);     // Enable PSK.
            #else // ifdef REQUIRE_WIFI_AP_PSK
            pskBuff[0] = '\0';                      // Disable PSK by using null char arrqy.
            #endif // ifdef REQUIRE_WIFI_AP_PSK

            apFlg = WiFi.softAP(nameBuff, pskBuff); // Connect hotspot.

            if (!apFlg) {
                Log.warningln("-> Hotspot Failed!");
                success = false;
                failCnt++;
            }
            else {
                success = true;
                failCnt = 0;
            }

            if (failCnt++ > MAX_CON_FAIL_CNT) {
                Log.warningln(" -> Too Many Failed WiFi Connect Attempts.");

                if (WiFiRebootFlg) {
                    Log.fatalln("wifiConnect: REBOOTING!\r\n");
                    Serial.flush();
                    delay(250);
                    rebootFlg = true;
                    rebootSystem(); // REBOOT NOW!
                }
                else {
                    Log.infoln("-> WiFi Reboot Disabled by User.");
                }
            }
            else if (!success) { // All connections failed. Give Up for now.
                return success;
            }

            // STA mode failed, but AP Mode is OK. lets create a hot spot for the user.
            ipAddrStr = WiFi.softAPIP().toString().c_str();
            updateUiIpaddress(ipAddrStr);

            if (dnsStartFlg) {
                dnsStartFlg = false;
                dnsServer.stop();
                Log.warningln("-> DNS Server Terminated.");
            }
            dnsStartFlg = dnsServer.start(dnsPort, "*", hotSpotIP);

            if (dnsStartFlg) {
                Log.infoln("-> DNS Server Started.");
            }
            else {
                Log.warningln("-> DNS Server FAILED.");
            }
        }
        else {
            Log.errorln("-> AP Fallback Disabled. Will NOT provide Hotspot access.");
        }
    }

    sprintf(logBuff, "-> WiFi Mode: %s", WiFi.getMode() == WIFI_AP ? "Client" : "Station");
    Log.infoln(logBuff);

    String ipStr = WiFi.getMode() == WIFI_AP ? IpAddressToString(WiFi.softAPIP()) : IpAddressToString(WiFi.localIP());
    sprintf(logBuff, "-> WiFi IP address: %s", ipStr.c_str());
    Log.infoln(logBuff);

    return success;
}

// ************************************************************************************************
// wifiValidateSettings(): Return true if StaticIP, Gateway, Subnet, and DNS are valid.
bool wifiValidateSettings(void)
{
    IPAddress tempAddr;
    bool successFlg = true;

    tempAddr = convertIpString(staticIpStr); // Convert to IP Class Array, return error value.

    if ((tempAddr[0] == 0) && (tempAddr[1] == 0) && (tempAddr[2] == 0) && (tempAddr[3] == 0)) {
        Log.verboseln("wifiValidateSettings: IP Address Invalid.");
        successFlg = false;
    }

    tempAddr = convertIpString(wifiGatewayStr); // Convert to IP Class Array.

    if ((tempAddr[0] == 0) && (tempAddr[1] == 0) && (tempAddr[2] == 0) && (tempAddr[3] == 0)) {
        Log.verboseln("wifiValidateSettings: WiFi Gateway Invalid.");
        successFlg = false;
    }

    tempAddr = convertIpString(subNetStr); // Convert to IP Class Array.

    if ((tempAddr[0] == 0) && (tempAddr[1] == 0) && (tempAddr[2] == 0) && (tempAddr[3] == 0)) {
        Log.verboseln("wifiValidateSettings: Subnet Mask Invalid.");
        successFlg = false;
    }

    tempAddr = convertIpString(wifiDnsStr); // Convert to IP Class Array.

    if ((tempAddr[0] == 0) && (tempAddr[1] == 0) && (tempAddr[2] == 0) && (tempAddr[3] == 0)) {
        Log.verboseln("wifiValidateSettings: WiFi DNS Invalid.");
        successFlg = false;
    }

    return successFlg;
}
