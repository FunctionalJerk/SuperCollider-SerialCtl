#include <Arduino.h>

#define numSwitches 3 // overall input switchPins connected
#define numPots 4     // overall input potPins connected

const uint8_t switchPins[numSwitches] = {19, 18, 17};
const uint8_t potPins[numPots] = {23, 22, 21, 20};
const uint8_t bitdepth = 14;
const int tickrate = 6; // this depends on 1 / (baudrate / msg-size) * 1000 = 5.73..

const uint8_t START_MARKER = 0xAA;
const uint8_t END_MARKER = 0x55;

uint8_t checksum;
uint8_t *bytes;

// By default, compilers add padding bytes between struct members to optimize memory access speed.
#pragma pack(push, 1) // Save the current alignment setting, then set alignment to 1 byte (no padding)
struct PotSwitchValues
{
  uint16_t pot1 : bitdepth;
  uint16_t pot2 : bitdepth;
  uint16_t pot3 : bitdepth;
  uint16_t pot4 : bitdepth;
  short switch1 : 1; // :4 for serialplot app
  short switch2 : 1; // :4 for serialplot app
  short switch3 : 1; // :4 for serialplot app // true == 1010
};
#pragma pack(pop) // Restore the previous alignment setting afterward.

PotSwitchValues Payload;

void setup()
{
  Serial.begin(19200);
  while (!Serial)
  { // wait for Serial...
    delay(500);
  }

  // set adc resolution to 14bits (= 16384 values)
  analogReadResolution(bitdepth);

  for (auto pin : switchPins)
  {
    pinMode(pin, INPUT_PULLDOWN);
  }
  for (auto pin : potPins)
  {
    pinMode(pin, INPUT);
  }
}

void readData()
{
  Payload.switch1 = digitalRead(switchPins[0]);
  Payload.switch2 = digitalRead(switchPins[1]);
  Payload.switch3 = digitalRead(switchPins[2]);

  Payload.pot1 = analogRead(potPins[0]);
  Payload.pot2 = analogRead(potPins[1]);
  Payload.pot3 = analogRead(potPins[2]);
  Payload.pot4 = analogRead(potPins[3]);
}

void sendData()
{
  checksum = 0;
  bytes = (uint8_t *)&Payload;
  for (size_t i = 0; i < sizeof(Payload); i++)
  {
    checksum ^= bytes[i]; // XOR checksum
  }

  Serial.write(START_MARKER);
  Serial.write(bytes, sizeof(Payload)); // send a buffer of bytes by giving a pointer (bytes) and a length (sizeof(Payload))
  Serial.write(checksum);
  Serial.write(END_MARKER);
  // ..or get number of bytes, this is needed in supercollider
  //Serial.println(sizeof(Payload));
}

void loop()
{

  readData();

  if (Serial.availableForWrite())
  {
    sendData();
  }
  delay(tickrate);
}