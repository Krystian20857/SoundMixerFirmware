# SoundMixerFirmware
SoundMixerFirmware is sub-repository of [Sound Mixer](https://github.com/Krystian20857/SoundMixer) project. Firmware is compatible with arduino-based devices. 
Version of firmware in this repository is intended for [Sound Mixer Hardware](https://github.com/Krystian20857/SoundMixerHardware) hardware configuration. It can be reconfigured to any other arduino device and be 100% compatible with [Sound Mixer Software](https://github.com/Krystian20857/SoundMixerSoftware).

## Content
* [Description](#sound-mixer-firmware)
* [Settings Overall](#things-which-can-be-set)
* [Setting Up Firmware](#setting-up-firmware)
* [Communication](#communication)
* [Debug Mode](#debug-mode)
* [Miscellaneous](#miscellanous)

## Things which can be set:
* Count of sliders, buttons and LEDs.
* Pins assigned to sliders, buttons and LEDs.
* Device name and ID.
* Serial connection settings like: Buffer size and data transmit termination symbol.
* Buttons debounce time.
* ADC settings: sampling rate, ADC range.

## Setting up firmware
Create file with name of your device in config folder. Then go to SoundMixer.ino file and replace first line with your config.
```C
#include "config/SoundMixerV2.h"              //including configuration
#include "config/MyDIYSoundMixerDevice.h"     //<--- your configuration
```

First you need to change IO and sliders, buttons and LEDs count
```C
#define SLIDER_COUNT 6        //Number of sliders connected to device.     
#define BUTTON_COUNT 5        //Number of buttons connected to device. 
#define LED_COUNT 2           //Number of LEDs connected to device. 

...

const volatile int sliders[SLIDER_COUNT] = {PA5, PA4, PA3, PA2, PA1, PA0};    //Pins to which sliders are connected.    <-- array length must match SLIDER_COUNT directive
const volatile int buttons[BUTTON_COUNT] = {PB12, PB13, PB14, PB15, PA8};     //Pins to which buttons are connected.    <-- array length must match BUTTON_COUNT directive
const volatile int leds[LED_COUNT] = {PB8, PB9};                              //Pins to which LEDs are connected.       <-- array length must match LED_COUNT directive
```

Then you need to set ADC range by changing *MAX_ADC_VAL* directive and calculate *DIFF* value;
```C
#define MAX_ADC_VAL 4095 //for 12-bit ADC
#define MAX_ADC_VAL 1023 //for 8-bit ADC

...

DIFF = (MAX_ADC_VAL + 1) / MAX_MAP_VAL     MAX_MAP_VAL = 100
#define DIFF 40   // for 12-bit ADC ->  (4095 + 1) / 100 = 40.96 <- round down for more reliable readings
#define DIFF 10   // for 8-bit ADC ->  (1023 + 1) / 100 = 10.24 <- round down for more reliable readings

```
In case readings are unstable, you have to increase sample count.
```C
#define SAMPLES 12 //12 <- default
#define SAMPLES 24 //provide more stable readings but take more time to probe.
```

Last thing you have to change is *DEVICE_NAME* and *DEVICE_ID*. These directives make your device unique. REMEMBER not to place 0xFF(255) byte inside *DEVICE_ID* more about it: [Communication](#communication). To generate deviceID use python script located in _/script/gen-deviceid.py_.
```C
const char DEVICE_NAME[30] PROGMEM = "My Unique Sound Mixer";                   //Max 30 characters
const uint8_t DEVICE_ID[6] PROGMEM = {0x36, 0x3F, 0x56, 0x45, 0x12, 0x67};      //FF(255) byte is forbidden
```
## Communication
Communication between device and pc take place by serial tunnel. Data from c-struct is serialized to byte stream and send through serial then termination byte is send. Termination byte is defined by *TERMINATOR* directive. 
It is very important not to send termination byte as struct data(it may cause multiple issues with data transmission).

## Debug Mode
All data generated by sliders and buttons wil be displayed as human readable string to serial output. To enable debug mode define *DEBUG* directive(uncomment 1st line)
```C
#define DEBUG     // <-- Enable Debug Mode
//#define DEBUG   // <-- Disable Debug Mode(Default)
```

## Miscellanous
Why *DEVICE_ID* and *DEVICE_NAME* are not store in External EEPROM?
- Base hardware use STM32F104 µc which does not have build-in EEPROM(emulating EEPROM will decrease flash memory write cycles count).
