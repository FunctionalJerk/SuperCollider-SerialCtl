# SuperCollider-SerialCtl

This is a hardware-controller, designed to interface with [SuperCollider] through USB.  
Other than (standard) MIDI, tty-serial offers faster baudrates and higher resolution.  

I have only tested it in SuperCollider on Ubuntu GNU/Linux, but I'm sure this device can interface with other applications that are able to read from serial aswell. 

## Prerequisites

### Software

- SuperCollider
- Visual Studio Code
- PlatformIO

### Hardware

- Teensy 3.2
- Pots / switches / buttons
- Housing

## Todo

- find a more elegant way to break out Teensy's micro-USB
- build a more complex version that:
	- reads Serial-data
	- offers more controls 