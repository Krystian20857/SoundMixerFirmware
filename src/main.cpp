#include <Arduino.h>

#ifndef DEVICE_ID_RAW
#error "Cannot find device ID. Check build scripts!"
#endif

const uint8_t DEVICE_ID[6] PROGMEM = DEVICE_ID_RAW;

void setup() {
    
}

void loop() {
   
}