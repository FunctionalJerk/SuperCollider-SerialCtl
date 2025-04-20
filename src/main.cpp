#include <Arduino.h>
#include <usb_dev.h>      // Teensy's core USB functions
// #include "custom_usb_desc.h"     // Your custom descriptor (quotes = local first)
// #include "custom_usb_names.h"    // Your custom names
#include <usb_rawhid.h>
// #include "usb_custom.h"

#define numSwitches 3 // overall input switchPins connected
#define numPots 4     // overall input potPins connected

// usb_custom_class CustomHID;

const uint8_t switchPins[numSwitches] = {19, 18, 17};
const uint8_t potPins[numPots] = {23, 22, 21, 20};
const uint8_t bitdepth = 16;
const uint8_t numSwitchBytes = numSwitches / 8 + 1;
const uint8_t numPaddingBytes = RAWHID_TX_SIZE - (sizeof(uint16_t) * numPots) - (sizeof(uint8_t) * numSwitchBytes);

// this should not exceed 64 bytes (RAWHID_TX_SIZE):
#pragma pack(push, 1)
struct HIDData
{
    uint16_t pots[numPots];                                                                              // 16 bytes
    uint8_t buttons[numSwitchBytes];                                                                     // 1 byte (will later be more)
    uint8_t padding[numPaddingBytes]; 
} static report;
#pragma pack(pop)

// if struct size != 64, compilation fails with this Error:
static_assert(sizeof(report) == RAWHID_TX_SIZE, "HIDPacket must be exactly 64 bytes");

void setup()
{
    Serial.begin(9600);
    analogReadAveraging(1);
    analogReadRes(bitdepth);

    memset(&report, 0, sizeof(report)); // zero out the struct, so padding doesn't contain garbage

    for (auto pin : switchPins) pinMode(pin, INPUT_PULLDOWN);
    for (auto pin : potPins) pinMode(pin, INPUT);
}

void readData()
{
    memset(&report, 0, sizeof(report)); // set all to 0

    for (int i = 0; i < numSwitches; i++)
    {
        report.buttons[i / 8] |= digitalRead(switchPins[i]) << (i % 8);
    }

    for (int i = 0; i < numPots; i++)
    {
        report.pots[i] = analogRead(potPins[i]);
    }
}

void loop()
{
    // uint32_t now = millis();
    // report[0] = now / 100;
    // uint8_t size = report[1];
    // Serial.println(sizeof(report));
    readData();

    // Serial.write((uint8_t*)&report, sizeof(report));
    // report[1] = size;
    // report[0] = 10;
    //CustomHID.send((uint8_t *)&report, 1000);
    RawHID.send((uint8_t *)&report, 1000);
    delay(1);
}