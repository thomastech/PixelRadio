# <img style="padding-right: 10px; padding-bottom: 5px;" align="left" src="../Images/RadioLogo300.gif" width="250">

# RDS CONTROLLERS

PixelRadio is a FM Radio Transmitter with RDS (Radio Data System) capabilities.
It was developed for holiday "Pixel" displays (e.g., animated Christmas lights).

To fully utilize the RadioText features it's important to understand PixelRadio's RDS Controllers.
There are four of them, named as follows:
1. Serial Controller
2. MQTT Controller
3. HTTP Controller
4. Local Controller

Each controller can receive RDS (RadioText) and System commands.
They operate independently.
You can use one controller or all of them.

The Controllers are ordered by their priority, as shown in the list above.
For example, the MQTT Controller has higher priority than the HTTP Controller.
A lower priority controller's RadioText message will not start until the higher priority controller has finished displaying their RadioText message.

The Local Controller's RadioText is handled by the [LOCAL RDS](./LocalTab.md) Tab.
The other three controllers are designed to receive commands from external sources.
The command syntax is text based.

For example, the HTTP controller can be commanded to broadcast a RadioText Message by using a URL like this:\
`http://pixelradio.local:8080/cmd?rtm=Welcome to Our Pixel Show. Enjoy!`

The four controllers must be configured before use.
Please see the [CONTROLLERS Tab](./ControlTab.md) document for details.

&nbsp;&nbsp;&nbsp;

---

## COMMAND KEYWORDS

The Serial, MQTT, and HTTP controllers share the same command keywords.
| KEYWORD | PAYLOAD MESSAGE | DESCRIPTION | SERIAL CONTROLLER EXAMPLE |
|----------|---------------|------------------|-----------------------|
| **aud** | *mono : stereo* | Audio Mode | aud=mono |
| **freq** | *881 - 1079* | FM Transmit Frequency X10 | freq=887 |
| **gpio19** | *read : outhigh : outlow* | GPIO Pin-19 Control | gpio19=read |
| **gpio23** | *read : outhigh : outlow* | GPIO Pin-23 Control | gpio23=outlow |
| **gpio33** | *read : outhigh : outlow* | GPIO Pin-33 Control | gpio33=outhigh |
| **mute** |  *on : off* | Audio Mute | mute=off |
| **pic** | *0x00ff - 0xffff* | RDS Program ID Code (hex) | pic=0x6401 |
| **rtper** | *5 - 900* |RDS Time Period (seconds)| rtper=45 |
| **psn** | *<8 characters max>* | RDS Program Service Name | psn=BOO2U |
| **pty** | *0 - 29* | RDS PTY Code | pty=9 |
| **rfc** |  *on : off* | RF Carrier Control | rfc=on |
| **rtm** | *<64 characters max>* | RDS RadioText Message | rtm=We have Ghosts and Halloween Candy! |
| **start** | *rds* | RDS Start / Restart | start=rds |
| **stop** | *rds* | RDS Stop | stop=rds |
| **info** | *system* | Request System Information | info=system |
| **reboot** | *system* | Perform System Reboot | reboot=system |

The RDS related commands are encapsulated.
For example, if a controller changes its RadioText message it will not affect the RadioText created by the other controllers.
This characteristic is shared by the RDS specific commands.

Whenever **any** RDS command is received the controller will be updated with the new value.
It then automatically starts sending the currently loaded RadioText message.
This intentional behavior helps reduce command traffic because it eliminates the need to issue a ``Start RDS`` command.

---

## COMMAND EXAMPLES
Each controller has its own syntax to execute commands.
Below are two examples that demonstrate the command format for the three controllers.

### EXAMPLE 1

These examples set the FM Frequency to 88.7MHz:

**Serial Controller:**\
    `freq=887`

**MQTT Controller:**\
    Publish Topic: `pixelradio/cmd/freq`\
    Message: `887`

**HTTP Controller:**\
    `http://pixelradio.local:8080/cmd?freq=887`


### EXAMPLE 2

Here are examples to change the RadioText Message to "Welcome to the Santa Village Light Show":

**Serial Controller:**\
    `rtm=Welcome to the Santa Village Light Show`

**MQTT Controller:**\
    Publish Topic: `pixelradio/cmd/rtm`\
    Message: `Welcome to the Santa Village Light Show`

**HTTP Controller:**\
    `http://192.168.1.37:8080/cmd?rtm=Welcome to the Santa Village Light Show`

#### -> HELPFUL TIP
Note: Some HTTP clients do not tolerate spaces in the URL.
Changing the spaces to %20 will resolve this issue.\
For example:\
`http://192.168.1.37:8080/cmd?rtm=Welcome%20to%20the%20Santa%20Village%20Light%20Show`

&nbsp;&nbsp;&nbsp;
&nbsp;&nbsp;&nbsp;

---

## CONTROLLER RESPONSES

Both the `Serial` and `HTTP` Controllers provide responses to every command.
The responses are JSON formatted.

For example, if the `aud` command is successfully executed the `Serial` and `HTTP` Controllers will respond as follows:\
`{"aud": "ok"}`

However, if an invalid payload message was received the controllers will reply with:\
`{"aud": "fail"}`

---

## INFO COMMAND

All three Controllers accept the info command.
This command is a request for information on PixelRadio's status.

An example of the `info` command's response is as follows:\
`{"info": "ok", "version": "1.0B", "hostName": "PixelRadio", "ip": "192.168.1.32", "rssi": -53, "status": "0xF1"}`

The payload in the `info` response contains a special `status` byte.
Each bit in this Hex formatted byte value is encoded with information about the four Controllers.
The bit descriptions are as follows:

| BIT | DESCRIPTION |
|----|---------------------------|
| d7 | Serial Controller Enabled |
| d6 | MQTT Controller Enabled |
| d5 | HTTP Controller Enabled |
| d4 | Local Controller Enabled |
|   |      |
| d3 | Serial RadioText Active |
| d2 | MQTT RadioText Active |
| d1 | HTTP RadioText Active |
| d0 | Local RadioText Active |

Unlike the other two controllers, the `MQTT` Controller only replies to the `info` (and `gpio`) command.
An MQTT client can subscribed to `pixelradio/info` to receive the `info` command's reply.

&nbsp;&nbsp;&nbsp;

---

# USB SERIAL CONTROLLER

The Serial Controller uses PixelRadio's USB Serial port.
It has Priority #1 (highest priority).

The USB port is compatible with most computers.
Windows 10 is plug-n-play, however other PC's may need to manually install a USB Serial driver.
The voltage provided by the USB connection is insufficient to run PixelRadio; the external 12V power supply must still be used.

The Serial Baud Rate is set by the `SERIAL CONTROL` panel in the [CONTROLLERS Tab](./ControlTab.md).
The data format is 8-bits, No Parity, 1 Stop Bit (8N1).
All Serial commands must be terminated with a carriage return character (\r).

Normally the serial commands would be sent by a USB based controller, such as PC or RaspberryPi.
However, a terminal emulation program can be used too.
A popular terminal emulator is <a href="https://www.putty.org/" target="_blank">PuTTY</a>


## OPTIONAL GPIO SERIAL CONTROLLER

Normally the Serial Controller uses the USB Serial port.
However, some installations may require a direct GPIO (logic level) connection.
Such as when interfacing to microcontroller based hosts that will provide the serial commands.
No worries, PixelRadio supports this.

Switching to Logic Level Serial requires recompiling with the alternate RBD_SerialManager library files stored in the /extras folder.
For more information please review the readme.txt file found there.

GPIO Serial's TxD and RxD Pins are located on the circuit board at the J4 area.
They utilize 3.3V Logic Levels and are 5V tolerant.
The interface is a three wire connection (TxD, RxD, GND).

GPIO Serial Pinout is as Follows:

| J4 PAD | SIGNAL | GPIO PIN |
| :---- | :---: | :------: |
| J4-2 | TxD | GPIO32 |
| J4-3 | RxD | GPIO34 |
| J4-5 | GND | GND |

`Note: J4-4 (5V Pin) may be used to power a low current microcontroller based serial host.
Maximum permitted draw is 50mA.`

&nbsp;&nbsp;&nbsp;

### DISABLE SERIAL LOG
It is important to disable the Diagnostic `SERIAL LOG` before using the USB based Serial Controller.
Simply use the Web interface and select the Diagnostics Tab.
On the SERIAL LOG LEVEL panel, change the entry to `LOG_LEVEL_SILENT.`
This suppresses the LOG messages.

As an alternative, there is a special Serial Command that can be sent to suppress the Diagnostic LOG messages.
It can be used instead of changing the SERIAL CONTROL setting in the web interface.
To disable the Serial LOG simply send this command:\
`log=silent`

The original LOG setting can be restored by sending this command:\
`log=restore`

>Note: The Serial LOG can be enabled if you need diagnostic messages while using the Serial Controller.
>If the LOG messages interfere with your serial host then try changing the LOG level to further filter its output.

### SERIAL HELP

When using a terminal emulator you can type `h` or `help` to see a summary of the Serial Controller's commands.

&nbsp;&nbsp;&nbsp;

---

# MQTT CONTROLLER

The MQTT Controller will process any of the command keywords that are published by your MQTT broker.
It has Priority #2.

All published messages to PixelRadio must use the following topic:
`pixelradio/cmd/<keyword>`\
Note: Replace `<keyword>` with the command keyword shown in the table above.

A broker client can receive responses by subscribing to PixelRadio's MQTT topics.
There are four subscription topics in total. The messages are JSON formatted.

| TOPIC NAME | RESPONSE MESSAGE EXAMPLE | DESCRIPTION |
| ---------- | ---------------- | ----------- |
| **/connect** | `{"boot": 0}` | Reports Device Boot / Reconnect |
| **/gpio** | `{"gpio19": 1"}` | Reports GPIO Read Pin Value |
| **/info** | `{"info": "ok", "version": "1.0B", "hostName": "PixelRadio", "ip": "192.168.1.32", "rssi": -53, "status": "0xF1"}` | Reports System Information |
| **/volts** | `{"vbat: 5.0, "pa": 9.0}` | Reports System and RF PA Voltages |

### SPECIAL MQTT RESPONSES

MQTT's `connect` and `volts` topics are sent periodically.
For example, every 30 seconds the `volts` topic is published.
It will also publish anytime a voltage measurement has changed.

When the MQTT Controller connects to the Local Network the `connect` topic is published.
It is also sent if a reconnect occurs.

&nbsp;&nbsp;&nbsp;

---

# HTTP CONTROLLER
The HTTP Controller will respond to web browsers and HTTP clients.
It has Priority #3.

All HTTP Controller traffic must use port 8080.
The port number MUST be included in the URL.
For example, to set the `Audio` mode use this URL: \
`http://pixelradio.local:8080/cmd?aud=stereo`
>NOTE: It may be necessary to use the device's IP address instead of the mDNS name. Example:\
>`http://192.168.1.37:8080.cmd?rtper=30`

## HTTP PERCENT ENCODING
It will be necessary to use HTTP *Percent Encoding* formatting on some characters sent by the Program Service Name (`psn`) and RadioText Message (`rtm`) commands.
For example, in order to display the plus sign (+) character it must be sent as ``%2B``.
Otherwise this character will be a blank space on the RDS receiver's display.

There are other non-alphanumeric characters that need this treatment too.
When necessary, change them to their Percent Encoded value listed in <a href="https://developer.mozilla.org/en-US/docs/Glossary/percent-encoding" target="_blank">this table</a>.
The allowable values are hex based and must be in the range of %20 and %7E.

#### -> HELPFUL TIPS
Use ``%3F`` to display a question mark (?).\
Text spaces can be replaced with ``%20`` or a plus (``+``) sign.

### HTTP CLEAR TEXT
Clearing the *RadioText Message* (rtm) or *Program Service Name* (psn) requires the Percent Encoding feature.
Either the %20 or %7f hex values (your choice) can be used to erase the text.
Send only the Percent Encoded value;
Do NOT include any other characters when text clearing is required.\
For example, to erase the RadioText message use URL:
`http://pixelradio.local:8080/cmd?rtm=%20` or `http://pixelradio.local:8080/cmd?rtm=%7f`

## HTTP CONTROLLER EXAMPLES
The HTTP Controller can be used with some popular Pixel Animation Sequencers.
This section provides examples to two different solutions that may be of interest to you.

### XLIGHTS / XSCHEDULE
*xLights* is a pixel animation software program that runs on Windows, OSX, and Linux.
It includes a sequence player that can send HTTP commands to control PixelRadio.
For basic instructions on how to configure *xLights* software to do this: [Click Here](./xLights.md).

### FALCON PLAYER (FPP) PLUGIN
*Falcon Player* is a pixel sequencer that runs on low cost single board computers such as the Raspberry Pi.
It has a software plugin that is compatible with PixelRadio's HTTP controller.\
FPP's PixelRadio plugin can be downloaded from the <a href="https://github.com/FalconChristmas" target="_blank">Falcon Christmas Repository</a>.

&nbsp;&nbsp;&nbsp;

---

# LOCAL CONTROLLER

The Local Controller has Priority #4 (lowest priority).
Its RDS functions are setup in the [LOCAL RDS](./LocalTab.md) Tab.

Up to three different RadioText messages can be created.
RadioText entries that are enabled will be sent in a round-robin order.
Each message will be shown for the time period that is configured by the `RDS DISPLAY TIME` entry.

&nbsp;&nbsp;&nbsp;

---

# RETURN TO OVERVIEW

<span>Return to the Menu Overview page: [Click Here](./Overview.md).</span>
