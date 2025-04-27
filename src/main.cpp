#include <Arduino.h>
#include <ADC.h>
#include <usb_dev.h>      // Teensy's core USB functions
#include <usb_rawhid.h>

#define numSwitches 3 // overall input switchPins connected
#define numPots 4     // overall input potPins connected

// usb_custom_class CustomHID;

const uint8_t switchPins[numSwitches] = {19, 18, 17};
const uint8_t potPins[numPots] = {23, 22, 21, 20};
const uint8_t bitdepth = 14;
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
    // Serial.begin(115200);
    analogReadAveraging(8); // average over 16 values
    analogReadRes(bitdepth);

    memset(&report, 0, sizeof(report)); // zero out the struct, so padding doesn't contain garbage

    for (auto pin : switchPins) pinMode(pin, INPUT_PULLDOWN);
    for (auto pin : potPins) pinMode(pin, INPUT);
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

bool receive() {
    uint8_t *buffer[RAWHID_TX_SIZE];
    RawHID.recv(&buffer, 1000);
    memcpy(&report, *buffer, RAWHID_TX_SIZE);
    return true;
}

void loop()
{
    // if(RawHID.available()) {
    //     // receive();
    // }
    updateData();

    RawHID.send((uint8_t *)&report, 1000);
    delay(1);
}