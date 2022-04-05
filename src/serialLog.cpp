/*
   File: serialLog.cpp
   Project: PixelRadio, an RBDS/RDS FM Transmitter (QN8027 Digital FM IC)
   Version: 1.0
   Creation: Dec-16-2021
   Revised:  Mar-07-2022
   Public Release:
   Project Leader: T. Black (thomastech)
   Contributors: thomastech
   Revision History: See PixelRadio.cpp

   (c) copyright T. Black 2021-2022, Licensed under GNU GPL 3.0 and later, under this license absolutely no warranty is given.
   This Code was formatted with the uncrustify extension.

   Note 1: Library Patch: In file ArduinoLog.h at approx line 371: Must add "va_end(args);" after the :print(msg, args)"" line.
*/

/*
#define LOG_LEVEL_SILENT  0
#define LOG_LEVEL_FATAL   1
#define LOG_LEVEL_ERROR   2
#define LOG_LEVEL_WARNING 3
#define LOG_LEVEL_INFO    4
#define LOG_LEVEL_NOTICE  4
#define LOG_LEVEL_TRACE   5
#define LOG_LEVEL_VERBOSE 6

void fatal   (const char *format, va_list logVariables);
void error   (const char *format, va_list logVariables);
void warning (const char *format, va_list logVariables);
void notice  (const char *format, va_list logVariables);
void trace   (const char *format, va_list logVariables);
void verbose (const char *format, va_list logVariables);
*/

// *************************************************************************************************************************
  #include <ArduinoLog.h>
  #include "PixelRadio.h"
  #include "globals.h"
  #include "language.h"

// *************************************************************************************************************************

void printLogLevel(Print* _logOutput,int logLevel);
void printPrefix(Print* _logOutput,int logLevel);
void printSuffix(Print* _logOutput,int logLevel);
void printTimestamp(Print* _logOutput);

// *************************************************************************************************************************

uint8_t getLogLevel(void)
{
    uint8_t logLevel;

    if (logLevelStr == DIAG_LOG_SILENT_STR) {
        logLevel = LOG_LEVEL_SILENT;
    }
    else if (logLevelStr == DIAG_LOG_FATAL_STR) {
        logLevel = LOG_LEVEL_FATAL;
    }
    else if (logLevelStr == DIAG_LOG_ERROR_STR) {
        logLevel = LOG_LEVEL_ERROR;
    }
    else if (logLevelStr == DIAG_LOG_WARN_STR) {
        logLevel = LOG_LEVEL_WARNING;
    }
    else if (logLevelStr == DIAG_LOG_INFO_STR) {
        logLevel = LOG_LEVEL_INFO;
    }
    else if (logLevelStr == DIAG_LOG_TRACE_STR) {
        logLevel = LOG_LEVEL_TRACE;
    }
    else if (logLevelStr == DIAG_LOG_VERB_STR) {
        logLevel = LOG_LEVEL_VERBOSE;
    }
    else {
        logLevel = LOG_LEVEL_SILENT;
        Log.errorln("getLogLevel: Unknown logLevelStr");
    }
    return logLevel;
}

// *************************************************************************************************************************
// initSerialLog(): Initialize the Serial Log. If arg = true, then set to Verbose. Else, set to User Configuration.
void initSerialLog(bool verbose)
{
    Log.setPrefix(printPrefix); // set prefix similar to NLog
    Log.setSuffix(printSuffix); // set suffix
    if(verbose) {
        Log.begin(LOG_LEVEL_VERBOSE, &Serial);
    }
    else {
        Log.begin(getLogLevel(), &Serial);
    }
    Log.setShowLevel(false);    // Do not show loglevel, we will do this in the prefix
}

// *************************************************************************************************************************
void printLogLevel(Print* _logOutput, int logLevel)
{
    /// Show log description based on log level
    switch (logLevel)
    {
        default:
        case 0:_logOutput->print("{SILENT } "); break;
        case 1:_logOutput->print("{FATAL  } "); break;
        case 2:_logOutput->print("{ERROR  } "); break;
        case 3:_logOutput->print("{WARNING} "); break;
        case 4:_logOutput->print("{INFORM } "); break;
        case 5:_logOutput->print("{TRACE  } "); break;
        case 6:_logOutput->print("{VERBOSE} "); break;
    }
}

// *************************************************************************************************************************

  void printPrefix(Print* _logOutput, int logLevel)
  {
    printTimestamp(_logOutput);
    printLogLevel (_logOutput, logLevel);
}

// *************************************************************************************************************************
void printSuffix(Print* _logOutput, int logLevel)
{
  _logOutput->print("");
}

// *************************************************************************************************************************

void printTimestamp(Print* _logOutput)
{
  // Division constants
  const unsigned long MSECS_IN_SEC       = 1000;
  const unsigned long SECS_IN_MIN        = 60;
  const unsigned long SECS_IN_HOUR       = 3600;
  const unsigned long SECS_IN_DAY        = 86400;

  // Total time
  const unsigned long msecs               =  millis() ;
  const unsigned long secs                =  msecs / MSECS_IN_SEC;

  // Time in components
  const unsigned long MiliSeconds         =  msecs % MSECS_IN_SEC;
  const unsigned long Seconds             =  secs  % SECS_IN_MIN ;
  const unsigned long Minutes             = (secs  / SECS_IN_MIN) % SECS_IN_MIN;
  const unsigned long Hours               = (secs  % SECS_IN_DAY) / SECS_IN_HOUR;

  // Time as string
  char timestamp[20];
  sprintf(timestamp, "%02lu:%02lu:%02lu.%03lu ", Hours, Minutes, Seconds, MiliSeconds);
  _logOutput->print(timestamp);
}



