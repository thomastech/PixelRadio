# <img style="padding-right: 10px; padding-bottom: 5px;" align="left" src="../Images/RadioLogo300.gif" width="250">

# HARDWARE INFORMATION

PixelRadio is a FM Radio Transmitter with RDS (Radio Data System) capabilities.
Hardware consists of a ESP32 WiFi equipped Microcontroller and low cost QN8027 Digital FM Transmitter module.
Typical range is about 100 meters with a small vertical whip antenna.

&nbsp;&nbsp;&nbsp;

---

## HARDWARE BUILD SKILL LEVEL

Build documentation is just a one page schematic.
Step-by-step wiring instructions or pictorial wiring diagrams are not provided.
The soldered parts are through-hole type.
No SMD components need to be soldered.

&nbsp;&nbsp;&nbsp;

---

## COMPONENT SOURCES

Any component sources that are mentioned are for your convenience. Please feel free to use your favorite supplier to acquire parts.

Kits and/or ready-to-use products are NOT available.
The project's creator does NOT sell any item used in this project.

&nbsp;&nbsp;&nbsp;

---

## BILL OF MATERIALS

Please visit the [Parts List](./PartsList.md) section for the project's bill-of-materials.

&nbsp;&nbsp;&nbsp;

---

## PRINTED CIRCUIT BOARD

PixelRadio can be built using perfboard construction.
However, a Printed Circuit Board (PCB) is recommended to ensure proper operation of the RF amplifier circuitry.

[Artwork files](../Artwork) are provided for the two-sided PCB.
The PCB files are in the RS274X gerber format required by commerical PC board fabricators.

The PCB is 95mm x 100mm in size.
It is specifically designed to slide into a low cost metal enclosure.
See the [Parts List](./PartsList.md) for details to the enclosure.

&nbsp;&nbsp;&nbsp;

---

## BASIC ASSEMBLY TIPS

The schematic is the main reference for building the circuitry.

### JUMPER WIRES

Solder jumper wires at locations L7 and J9.
Discarded resistor or capacitor wire leads work well for this.
It's also possible to install a 0.1" 2-Pin header at J9 and use a shorting block instead of a soldered wire.

### ANTENNA

The telescoping rod antenna looks nice and works well.
However, a 30" piece of bare or insulated wire can be used in place of the telescoping antenna.
Install the antenna so that it points upwards or downwards (vertical polarlization).

For more range a dipole antenna can be made from easy-to-obtain hardware store items.\
Assembly instructions are published at the doityourselfchristmas.com site:\
[DiY Dipole Antenna](http://www.doityourselfchristmas.com/wiki/images/a/a7/How_to_make_a_dipole_antenna.pdf)

### ON AIR SIGN

A 5V or 12V LED "On Air" sign can be installed at J8. As follows:\
J8-1 SIG: LED Gnd (-)\
J8-2 12V: LED 12V Power (+), max 1A\
J8-3 5V: LED 5V Power (+), max 250mA

&nbsp;&nbsp;&nbsp;

---

## SOFTWARE INSTALLATION

Please see the [Project Software](../../src/README.md) section for details on flashing the PixelRadio firmware.

&nbsp;&nbsp;&nbsp;

---

## RF CALIBRATION

PixelRadio is a digital device and does not have any adjustable components that are typically found on analog designs.
Therefore RF Calibration is not required.

&nbsp;&nbsp;&nbsp;

---

### THE FINE PRINT

*All information is provided as-is.
YOU must accept all responsibility for the construction and use of the project.*
