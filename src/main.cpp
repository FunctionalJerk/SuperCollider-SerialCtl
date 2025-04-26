#include <Arduino.h>
#include <usb_dev.h> // Teensy's core USB functions
#include <usb_rawhid.h>

#define numSwitches 3 // overall input switchPins connected
#define numPots 4     // overall input potPins connected

// if -DUSB_RAWHID is commented out in .ini file
#ifndef RAWHID_TX_SIZE
#define RAWHID_TX_SIZE 64
#endif

// usb_custom_class CustomHID;

const uint8_t switchPins[numSwitches] = {19, 18, 17};
const uint8_t potPins[numPots] = {23, 22, 21, 20};
const uint8_t bitdepth = 16;
const uint8_t numSwitchBytes = numSwitches / 8 + 1;
const uint8_t numPaddingBytes = RAWHID_TX_SIZE - (sizeof(uint16_t) * numPots) - (sizeof(uint8_t) * numSwitchBytes);

uint8_t checksum;
uint8_t *bytes;

// this should not exceed 64 bytes (RAWHID_TX_SIZE):
#pragma pack(push, 1)
struct HIDData
{
    uint16_t pots[numPots]; // 16 bytes
    uint8_t buttons[numSwitchBytes];
    uint8_t padding[numPaddingBytes];
} static report;
#pragma pack(pop)

// if struct size != 64, compilation fails with this Error:
static_assert(sizeof(report) == RAWHID_TX_SIZE, "HIDData must be exactly 64 bytes");

void setup()
{
    Serial.begin(115200);
    while (!Serial)
    { // wait for Serial...
        delay(500);
    }

    analogReadAveraging(16); // average over 16 values
    analogReadRes(15);

    for (auto pin : switchPins)
        pinMode(pin, INPUT_PULLDOWN);
    for (auto pin : potPins)
        pinMode(pin, INPUT);
}

void updateData()
{
    memset(&report, 0, sizeof(report)); // set all to 0

    for (int i = 0; i < numSwitches; i++)
    {
        report.buttons[i >> 3] |= digitalRead(switchPins[i]) << (i % 8);
    }

    for (int i = 0; i < numPots; i++)
    {
        report.pots[i] = analogRead(potPins[i]);
    }
}

void loop()
{
    uint16_t testing = analogRead(potPins[0]);

    updateData();

    // Serial.println(report.pots[1]);
    Serial.write((uint8_t *)&testing, 2);
    delay(10);
}
