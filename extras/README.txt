Readme.txt

Arduino RBD Serial Manager Library v1.0.0 - A simple interface for serial communication.
https://github.com/alextaujenis/RBD_SerialManager
Copyright (c) 2015 Alex Taujenis - MIT License
-----------------------------------------------------------------------------------------------------------

Modified Apr-13-2022 by ThomasTech / TEB.

PixelRadio's default Serial Controller uses USB Serial (Serial-0).
If TTL Serial (Serial-1) is needed then two updated files must be installed in
the /RBD_SerialManager.cpp/src folder.

Basic Instructions:
Replace the RBD_SerialManager.cpp and RBD_SerialManager.h files in the library if Serial-1
port is needed.
Be sure to change the baud rate (RBD_SerialManager.cpp line 15) to match your host's
requirements.

Serial1 Wiring:
Serial1 TxD = GPIO32. 3.3V TTL. Available on Header J4, Pin 2.
Serial1 RxD = GPIO34. 3.3V TTL, 5V Tolerant. Available on Header J4, Pin 3.
