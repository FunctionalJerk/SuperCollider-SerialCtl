#include <Arduino.h>

#define numPins 7 // overall input-pins connected
#define numPots 4 

const int pins[numPins] = {23,22,21,20, 19,18,17};

uint8_t thresh = 2;

uint16_t potVal, preVal;

uint16_t preVals[numPins] = {};
uint16_t potVals[numPins] = {};

void setup() {
  Serial.begin(9600);
  while(!Serial) { delay(500); };

  for(int i = 0; i < numPins; i++) {
    if(i < numPots) {  // for the pots
      pinMode(pins[i], INPUT);
    } else { // pulldown for stitches & buttons
      pinMode(pins[i], INPUT_PULLDOWN);
    }
  }
}

void loop() {

  if (Serial) {

    for(int i = 0; i < numPins; i++) {
      
      preVal = preVals[i];

      if (i < numPots) { // the pots first
        // potVals[i] = potVal;
        potVal = analogRead(pins[i]);
      } else { // then the switches
        potVal = analogRead(pins[i]) > 512;
      }
      potVals[i] = potVal;

      if(potVal != preVal) { 
        
        //Serial.clear(); 
        Serial.print(i); 
        Serial.print(";");
        Serial.println(potVal);

        preVals[i] = potVal;
      }
    }
    delay(20);
  }
}