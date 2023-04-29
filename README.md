# SuperCollider-SerialCtl

This is a hardware-controller, designed to interface with SuperCollider through USB.  
Other than (standard) MIDI, tty-serial offers faster baudrates and higher resolution.  

I have only tested it in SuperCollider on Ubuntu GNU/Linux, but I'm sure this device can interface with other applications that are able to read from serial. 

## Prerequisites

### Software

- [SuperCollider](https://github.com/supercollider/supercollider)
- [Visual Studio Code](https://code.visualstudio.com/)
- [PlatformIO for VSCode](https://docs.platformio.org/en/latest/integration/ide/vscode.html)

### Hardware

- [PJRC's Teensy 3.2](https://www.pjrc.com/store/teensy32.html)
- Pots / switches / buttons (go wild! I would recommend linear potentiometers, though)
- Housing 

## Todo

- upload schematics (they're pretty straight-forward)
- remove "flickering"
- find a more elegant way to break out Teensy's micro-USB
- build a more complex version that:
	- reads Serial-data
	- offers more controls 
