#include <Arduino.h>

#define numSwitches 3 // overall input switchPins connected
#define numPots 4 // overall input potPins connected

const int switchPins[numSwitches] = {19, 18, 17};
const int potPins[numPots] = {23, 22, 21, 20};
const int bitdepth = 14;

void setup()
{
  Serial.begin(115200);
  while (!Serial)
  { // wait for Serial...
    delay(500);
  }

  // set adc resolution to 14bits (= 16384 values)
  analogReadResolution(bitdepth);

  for (auto i : switchPins)
  {
    pinMode(i, INPUT_PULLDOWN);
  }
  for (auto i : potPins)
  {
    pinMode(i, INPUT);
  }
}

void loop()
{
  uint8_t switchStates = 0;
  uint16_t potVal;

  for (int i = 0; i < numSwitches; i++)
  {
    if (digitalRead(switchPins[i]) == LOW)
    { // you can swap these to invert switch behaviour
      switchStates |= (0 << i);
    }
    else
    {
      switchStates |= (1 << i);
    }
  }

  // suboptimal: 
  // because we always read 2 bytes in SC, there must be an unused byte here. 
  // In future projects we can easily have upto 16 switches / buttons.
  Serial.write(0); // Send 0 as the first byte
  Serial.write(switchStates); // Send switch states as the second byte

  for (auto i : potPins)
  {
    potVal = analogRead(i);
    Serial.write((potVal >> 8) & 0xFF); // Send upper byte of 12-bit value
    Serial.write(potVal & 0xFF);        // Send lower byte of 12-bit value  }
  }

  // This combination of two bytes will not occur through analogRead values (14bit),
  // so it can be used as EOM.  
  Serial.write(255);
  Serial.write(255);

  // Serial.write(0xFF); // Send the EOM byte
  delay(5);
}