  #include <Arduino.h>

  #define numSwitches 3 // overall input switchPins connected
  #define numPots 4 // overall input potPins connected

  const int switchPins[numSwitches] = {19, 18, 17};
  const int potPins[numPots] = {23, 22, 21, 20};
  const int bitdepth = 14;
  const int tickrate = 2;

  // By default, compilers add padding bytes between struct members to optimize memory access speed.
  #pragma pack(push, 1) // Save the current alignment setting, then set alignment to 1 byte (no padding)
  struct PotSwitchValues 
  {
    uint16_t pot1:bitdepth;
    uint16_t pot2:bitdepth;
    uint16_t pot3:bitdepth;
    uint16_t pot4:bitdepth;
    bool switch1:1;
    bool switch2:1;
    bool switch3:1; 
  };
  #pragma pack(pop) // Restore the previous alignment setting afterward.

  PotSwitchValues Payload;

  uint8_t numBytes = sizeof(Payload);

  void setup()
  {

    Serial.begin(230400);
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

  void loop()
  {
    
    Payload.switch1 = digitalRead(switchPins[0]);
    Payload.switch2 = digitalRead(switchPins[1]);
    Payload.switch3 = digitalRead(switchPins[2]);
    
    Payload.pot1 = analogRead(potPins[0]);
    Payload.pot2 = analogRead(potPins[1]);
    Payload.pot3 = analogRead(potPins[2]);
    Payload.pot4 = analogRead(potPins[3]);
    
    // send a buffer of bytes by giving a pointer and a length
    Serial.write((uint8_t*)&Payload, sizeof(Payload));
    
    // get number of bytes, this is needed in supercollider
    // Serial.println(sizeof(Payload));

    delay(tickrate);
  }