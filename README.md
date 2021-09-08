# JVSy-maimai

### Description
JVSy lets you connect your arcade JVS I/O board to your PC and use it as either a keyboard or joystick, for MAME or other purposes.

**JVSy-maimai** is a fork intended **specifically for use with original Maimai cabs!**

This is due to the keybinds and reverse logic for the photoswitches used in these cabs.

**If you intend to use this for other general applications**, I would go to [gtranche's repo](https://github.com/gtranche/JVSy).

All hardware buttons including 1P, 2P, Test, Service, and Coin are mapped to keyboard keys based on the diagram below:

![jvsy-maimai-keybinds](https://i.imgur.com/umIxTio.png)

### Hardware

This solution is based on a Teensy V2 development board and a SN65176B chip allowing communication over the RS485 protocol used by the JVS bus.

Practically any standard RS485 transceiver module will work. This was tested with a MAX485 (easily found as breakout boards on Amazon) without issue.

You will need: 
- Teensy 2.0
- RS485 to RS232/TTL transceiver module (SN65176B, SN75176B, MAX485, etc.)
- USB-A full size female header (can harvest from a USB extension cable)

### Connections

**MAX485 / SN65176B / SN75176B:**
- Pin 1 (RO): Teensy D2
- Pin 2 (RI): Teensy F6
- Pin 3 (DE): Teensy F6
- Pin 4 (DI): Teensy D3
- Pin 5 (GND): Teensy GND
- Pin 6 (A): USB White
- Pin 7 (B): USB Green
- Pin 8 (Vcc): Teensy VCC

**USB (JVS pinout):**
- RED (Sense): Teensy B4
- WHITE (A): SN65176B 6
- GREEN (B): SN65176B 7
- BLACK (GND): Teensy GND

### Setup

1) Grab the latest `.hex` file from Releases and flash using [Teensy Loader](https://www.pjrc.com/teensy/loader.html)

2) Once all your hardware is connected as shown above, disconnect the JVS connector (male USB-A) from your RingEdge and connect it to the female USB-A port you've wired up.

3) Ensure the cab is on and the I/O board is powered, then connect the Teensy via USB to your main PC. It should install drivers automatically and your buttons should function as keyboard keys (see diagram above).

It's worth noting that for Maimai cabinets, if you intend to use the board with another PC, the main cab power will need to be on as the RingEdge 2 provides power to the JVS I/O board (and the photoswitches in the buttons).

This could be handled by powering the board and buttons yourself externally if you wish, but that is beyond the scope of this guide.

The I/O board **must be on** when you connect the Teensy to your PC. If it wasn't, ensure it is then simply disconnect/reconnect the Teensy to your PC.

### Compiling

If you don't need to change the keybinds and are using a Teensy 2.0 like this guide, just follow **Setup** above and use the pre-compiled firmware.

If you want to compile yourself, you will need to replace your Arduino IDE's Teensy hardware folder with gtranche's libraries in order to support NKRO and dual joysticks.

**Do this at your own risk** as this will overwrite the base Teensy libraries. You can always replace these with the original ones yourself later if you wish.

The libraries pulled for this fork are included as `teensyjvsy-master.zip` in this repo as a backup, but you can also grab the latest from [gtranche's separate repo](https://github.com/gtranche/teensyjvsy).

### Thanks

Many thanks to **k4roshi** and **gtranche** for their work on this project.
Practically all I had to do was change keybinds.