/*
   File: fileSys.cpp
   Project: PixelRadio, an RBDS/RDS FM Transmitter (QN8027 Digital FM IC)
   Version: 1.1.0
   Creation: Dec-16-2021
   Revised:  Jun-13-2022
   Revision History: See PixelRadio.cpp
   Project Leader: T. Black (thomastech)
   Contributors: thomastech

   (c) copyright T. Black 2021-2022, Licensed under GNU GPL 3.0 and later, under this license absolutely no warranty is given.
   This Code was formatted with the uncrustify extension.

   Notes:
   SPIFFS is depreciated and has been replaced by LittleFS.

   Data Files:
   All littleFS data files must be in the PixelRadio/data folder.

   Here's some advice on how to get all the ESPUI data files to fit in min_spiffs flash space.
   ESPUI has 50KB of unneeded files; Delete the following temp files (if present) from the /project data folder:
    data/index.min.htm
    data/css/normalize.min.css
    data/css/style.min.css
    data/js/controls.min.js
    data/js/graph.min.js
    data/js/slider.min.js
    data/js/tabbedcontent.min.js
    NOTE: Do NOT delete /data/js/zepto.min.js

   Base64 GIF Data Files:
   GIF Files can be converted to base64 using this tool: https://codebeautify.org/gif-to-base64-converter

   How to get data files (Filesystem Image) onto ESP32 during ESP32 Flash:
    1. Use tne IDE's Platform->Upload_Filesystem_Image to upload the data directory files.
    2. Or use powershell command To serial upload the LittleFS data directory: platformio run --target uploadfs
    3. Or for OTA upload use: platformio run --target uploadfs --upload-port <IP_ADDR>
       Note: Replace <IP_ADDR> with board's IP (example: 192.168.1.7).
    4. Or to copy the Logo Image File to File System use SD Card method. See Github readme.

 */

// *********************************************************************************************

#include <Arduino.h>
#include <ArduinoLog.h>
#include <LittleFS.h>
#include <SPI.h>
#include <SD.h>
#include "config.h"
#include "PixelRadio.h"

// *********************************************************************************************
// instalLogoImageFile(): Check to see if a logo image file needs to be copied to the ESP32
// file system. Use SD Card as file source.
// Note: This routine can be used anytime AFTER littlefsInit(); Typically called in startup().
//
void instalLogoImageFile(void) {
    char logBuff[60 + sizeof(LOGO_GIF_NAME)];
    int16_t sdcFileSize;
    int16_t lfsFileSize;
    SPIClass SPI2(HSPI);

    if (LittleFS.exists(LOGO_GIF_NAME)) {
        sprintf(logBuff, "Found \"%s\" Logo Image in File System.", LOGO_GIF_NAME);
        Log.verboseln(logBuff);
        return;
    }

    sprintf(logBuff, "Logo Gif File (%s) is Missing. Will Load it From the SD Card.", LOGO_GIF_NAME);
    Log.errorln(logBuff);

    SPI2.begin(SD_CLK_PIN, MISO_PIN, MOSI_PIN, SD_CS_PIN);
    pinMode(MISO_PIN, INPUT_PULLUP); // MISO requires internal pull-up.
    SD.end();                        // Reset interface (in case SD card had been swapped).

    if (!SD.begin(SD_CS_PIN, SPI2)) {
        SD.end();
        spiSdCardShutDown();

        sprintf(logBuff, "-> SD Card Not Installed. Cannot Load Missing Logo Gif File.");
        Log.errorln(logBuff);
        return; // No SD Card, nothing to do, exit.
    }

    File sdcImageFile; // SD Card Image File.
    sdcImageFile = SD.open(LOGO_GIF_NAME, FILE_READ);
    sdcFileSize  = sdcImageFile.size();

    if (sdcImageFile && (sdcFileSize > 0)) {
        sprintf(logBuff, "-> Installing \"%s\" Logo Gif File From SD Card ...", LOGO_GIF_NAME);
        Log.infoln(logBuff);
        sprintf(logBuff, "-> Copying Gif File, Size: %d", sdcImageFile.size());
    }
    else {
        //lfsImageFile.close();
        sdcImageFile.close();
        SD.end();
        spiSdCardShutDown();
        sprintf(logBuff, "-> Bad/Missing SD Card Logo File.");
        Log.errorln(logBuff);
        return;
    }

    sprintf(logBuff, "-> Copying Logo File From SD Card (%d bytes).", sdcFileSize);
    Log.infoln(logBuff);

    //lfsImageFile.close();
    File    lfsImageFile; // LittleFS Image File.
    lfsImageFile = LittleFS.open(LOGO_GIF_NAME, FILE_WRITE);

    while (sdcImageFile.available()) {
        char data = sdcImageFile.read();
        lfsImageFile.print(data);
    }
    lfsImageFile.close();
    lfsImageFile = LittleFS.open(LOGO_GIF_NAME, FILE_READ);
    lfsFileSize  = lfsImageFile.size();

/*  // DEBUG ONLY
    Serial.println("START==>");
    while (lfsImageFile.available()) {
        Serial.write(lfsImageFile.read()); // Send raw data to serial port.
    }
    Serial.println("<==END");
 */
    sprintf(logBuff, "-> Success, Saved Logo Gif to File System (%d bytes).", lfsFileSize);
    Log.infoln(logBuff);

    if (sdcImageFile != lfsImageFile) {
        sprintf(logBuff, "-> Copied File Does NOT match Source Size (diff= %d bytes).", sdcImageFile - lfsImageFile);
        Log.errorln(logBuff);
    }

    lfsImageFile.close();
    sdcImageFile.close();
    SD.end();
    spiSdCardShutDown();

    littlefsInit();
}

// *********************************************************************************************
// littlefsInit(): Initialize LittleFS file system.
void littlefsInit(void)
{
    char logBuff[100];

    unsigned long filesz   = 0;
    const char   *content1 = "  This text was written to LittleFS because the Filesystem is missing\r\n";
    const char   *content2 = "  >> If you see this message then please Upload the Filesystem Image <<";

    if (!LittleFS.begin(true)) { // true=Format on fail.
        Log.errorln("LittleFS: An Error has occurred while mounting File System");
    }
    else {
        Log.infoln("LittleFS: Mounted File System, testing ...");

        sprintf(logBuff, "-> Total Size: %u bytes", LittleFS.totalBytes());
        Log.verboseln(logBuff);
        sprintf(logBuff, "-> Total Used: %u bytes", LittleFS.usedBytes());
        Log.verboseln(logBuff);
        sprintf(logBuff, "-> Total Free: %u bytes", LittleFS.totalBytes() - LittleFS.usedBytes());
        Log.verboseln(logBuff);

        File file1 = LittleFS.open("/test.txt", FILE_READ);

        if (!file1) {
            filesz = 0;
            Log.errorln("-> Error opening test.txt file for reading");
        }
        else {
            filesz = file1.size();
            Log.verboseln("-> Successfully Opened test.txt file for reading, Size: %u", filesz);
            file1.close();
        }

        if (filesz == 0) {
            file1 = LittleFS.open("/test.txt", FILE_WRITE);

            if (!file1) {
                Log.errorln("-> There was an error opening the test.txt file for writing");
            }
            else {
                Log.warningln("-> The test.txt file is missing, will create it");

                // Serial.println("  Content to write =");
                // Serial.print(content1);
                // Serial.println(content2);

                if (file1.print(content1)) {
                    file1.print(content2);
                    Log.infoln("-> File was successfully created, written, and closed");
                }
                else {
                    Log.errorln("-> File write failed");
                }
                file1.close();
            }
        }
        else {
            Log.verboseln("-> test.txt file successfully opened, now closed");
            file1.close();
        }

        // Let's read the test.txt file from LittleFS.
        // Data files like this one should be uploaded to the Filesystem Image, see comment section at top of file.
        File file2 = LittleFS.open("/test.txt", FILE_READ);

        if (!file2)
        {
            Log.errorln("-> Failed to open file test.txt for reading");
        }
        else {
            sprintf(logBuff, "-> Reopened test.txt, File size = %u bytes", file2.size());
            Log.verboseln(logBuff);

            /*
                        Serial.println(" Contents of test.txt file = ");
                        while (file2.available())
                        {
                            Serial.write(file2.read()); // Send raw data to serial port.
                        }
                        Serial.println();
             */
            file2.close();
        }

        Log.infoln("-> File System Testing Complete.");
    }
}

// ============================================================================================================================
// makeWebGif(): Get base64 GIF from file system (LittleFS) and save in String encoded for HTML.
const String makeWebGif(String fileName, uint16_t width, uint16_t height, String backGroundColorStr)
{
    uint16_t fileSz = 0;
    char     logBuff[90];
    String   imageStr;

    imageStr.clear();
    File imageFile = LittleFS.open(fileName, FILE_READ);

    if (!imageFile) {
        sprintf(logBuff, "-> makeWebGif: Error opening Image file (%s)", fileName.c_str());
        Log.warningln(logBuff);
        Log.warningln("-> LittleFS Filesystem is Missing, YOU Need to Upload it.");
        imageStr.clear();
        imageStr.reserve(13);
        imageStr = "IMAGE FILE MISSING";
        return imageStr;
    }
    else {
        fileSz = imageFile.size();

        if (fileSz > 4095) {
            sprintf(logBuff, "-> makeWebGif: File %s is too large (%u bytes).", fileName.c_str(), fileSz);
            Log.infoln(logBuff);
            imageStr.clear();
            imageStr.reserve(15);
            imageStr = "FILE TOO LARGE";
            imageFile.close();
            return imageStr;
        }

        imageStr.reserve(fileSz + 200);
        imageStr  = "<p style=";
        imageStr += "\"background-color:";
        imageStr += backGroundColorStr;
        imageStr += ";margin-bottom:-3px;margin-top:-2px;margin-left:-6px;margin-right:-6px;\">";
        imageStr += "<img src=\'data:image/gif;base64,";
        imageStr += imageFile.readString();
        imageStr += "\'; width=\"";
        imageStr += String(width);
        imageStr += "\" height=\"";
        imageStr += String(height);
        imageStr += "\" alt=\"";
        imageStr += fileName;
        imageStr += "\"/></p>";

        imageFile.close();
        sprintf(logBuff, "-> Image File %s Successfully Loaded (%u bytes).", fileName.c_str(), fileSz);
        Log.infoln(logBuff);

        // Serial.println("\r\n makeWebGif File Contents: BEGIN->[ " + imageStr + " ]<-END\r\n"); // DEBUG ONLY
    }
    return imageStr;
}

// *********************************************************************************************
// EOF
