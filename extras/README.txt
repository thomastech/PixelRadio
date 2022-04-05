Readme.txt

Arduino RBD Serial Manager Library v1.0.0 - A simple interface for serial communication.
https://github.com/alextaujenis/RBD_SerialManager
Copyright (c) 2015 Alex Taujenis - MIT License
-----------------------------------------------------------------------------------------------------------

Modified Mar-10-2022 by TEB. 

PixelRadio's default Serial Controller uses USB Serial (Serial-0).
If TTL Serial is needed then install these two files in the /RBD_SerialManager.cpp/src folder.

Basic Instructions:
Replace the RBD_SerialManager.cpp and RBD_SerialManager.h files in the library if Serial-1 Port is needed.
Be sure to change the baud rate (line 15) to match your host's requirements.

Serial1 Wiring:
Serial1 TxD = GPIO32. 3.3V TTL.
Serial1 RxD = GPIO34. 3.3V TTL, 5V Tolerant.

