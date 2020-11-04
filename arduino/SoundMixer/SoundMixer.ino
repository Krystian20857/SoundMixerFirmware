#include "config/SoundMixerV2.h"              //including configuration

/* ----- Code Section -----  */

volatile int lastSliderState[SLIDER_COUNT]; //constains last values of sliders
volatile int lastMapValue[SLIDER_COUNT];    //contains last map value of sliders
volatile int lastThrottleTime[SLIDER_COUNT];

volatile int lastButtonReading[BUTTON_COUNT]; //contains last buttons states
volatile int lastDebounceTime[BUTTON_COUNT];  //contains last buttons debounce timings
volatile int lastButtonState[BUTTON_COUNT];   //contains last accepted button states

int slider_index;
int button_index;
int serial_index;

char buffer[BUFFER_SIZE];

//using in serial communication
struct SliderStruct
{
  uint8_t command;
  uint8_t slider;
  uint16_t value;
};

struct ButtonStruct
{
  uint8_t command;
  uint8_t button;
  uint8_t state;
};

struct LedStruct
{
  uint8_t command;
  uint8_t led;
  uint8_t state;
};

struct DEVICEIDRequest
{
  uint8_t command;
  uint8_t flag;
};

struct DEVICEIDResponse
{
  uint8_t command;
  uint8_t flag;
  char name[32];
  uint8_t uuid[6];
  uint8_t slider_count;
  uint8_t button_count;
};

void setup()
{
  Serial.begin(115200); //serial setup(baudrate is ignored my mapple)

  for (int n = 0; n < SLIDER_COUNT; n++) //sets pinMode for all sliders
    pinMode(sliders[n], INPUT_ANALOG);

  for (int n = 0; n < BUTTON_COUNT; n++) //sets pinMode for all buttons
    pinMode(buttons[n], INPUT_PULLUP);

  for (int n = 0; n < LED_COUNT; n++)
    pinMode(leds[n], OUTPUT);

  for (int n = 0; n < LED_COUNT; n++)
    digitalWrite(leds[n], LOW);
}

void loop()
{

  int currentReading = 0;
  for (int n = 0; n < SAMPLES; n++) //sampling current slider value
    currentReading += analogRead(sliders[slider_index]);

  currentReading /= SAMPLES; //averaging current slider value

  if (abs(currentReading - lastSliderState[slider_index]) >= DIFF) //checking if value change is bigger than diff
  {
    if (millis() - lastThrottleTime[slider_index] > SLIDER_THROTTLE)
    {
      lastSliderState[slider_index] = currentReading;
      int mapValue = 0;
        mapValue = map(currentReading, 0, MAX_ADC_VAL - (0.5 * DIFF), 0, MAX_MAP_VAL); //in other case map value
      if (abs(mapValue - lastMapValue[slider_index]) >= MAP_DIFF)       //checking if map diff is bigger than value change
      {
        lastMapValue[slider_index] = mapValue;

        struct SliderStruct slider = {
            .command = 0x01,
            .slider = slider_index,
            .value = mapValue};                        //assign values to data struct
#ifndef DEBUG
        Serial.write((byte *)&slider, sizeof(slider)); //send data over the serial
        Serial.write(TERMINATOR);                      //send terminator to end struct parsing
#endif
#ifdef DEBUG
        Serial.print("Slider: ");
        Serial.print(slider.slider);
        Serial.print(" = ");
        Serial.println(slider.value);
#endif
      }
      lastThrottleTime[slider_index] = millis();
    }
  }

  int buttonReading = digitalRead(buttons[button_index]); //reading current button state
  if (buttonReading != lastButtonReading[button_index])   //checking if state changed
    lastDebounceTime[button_index] = buttonReading;

  if (millis() - lastDebounceTime[button_index] > DEBOUNCE_TIME) //checking if the button has been pressed long enough
  {
    if (buttonReading != lastButtonState[button_index]) //checking if debounced button state changed
    {
      lastButtonState[button_index] = buttonReading;
      struct ButtonStruct button = {
          .command = 0x02,
          .button = button_index,
          .state = buttonReading};                   //assign values to data struct
#ifndef DEBUG
      Serial.write((byte *)&button, sizeof(button)); //send data over serial
      Serial.write(TERMINATOR);                      //send terminator to end struct parsing
#endif
#ifdef DEBUG
      Serial.print("Button: ");
      Serial.print(button.button);
      Serial.print(" = ");
      Serial.println(button.state);
#endif
    }
  }

  if (Serial.available() > 0) //checing if are any data to receive
  {
    char serialRead = Serial.read(); //checking if a byte is a termination char
    if (serialRead == TERMINATOR)
    {
      if (buffer[0] == 0x01) //handiling commands...
      {
        struct LedStruct ledStruct;                     //creating struct from buffer
        memcpy(&ledStruct, &buffer, sizeof(ledStruct)); //copying buffer content to local struct variable
        digitalWrite(leds[ledStruct.led], ledStruct.state);
      }
      else if (buffer[0] == 0x02)
      {
        struct DEVICEIDRequest DEVICEIDRequest;                     //creating struct from buffer
        memcpy(&DEVICEIDRequest, &buffer, sizeof(DEVICEIDRequest)); //copying buffer content to local struct variable
        struct DEVICEIDResponse DEVICEIDResponse = {
            .command = 0x03};
        DEVICEIDResponse.flag = DEVICEIDRequest.flag;
        DEVICEIDResponse.slider_count = SLIDER_COUNT;
        DEVICEIDResponse.button_count = BUTTON_COUNT;
        memcpy((byte *)&DEVICEIDResponse.name, &DEVICE_NAME, sizeof(DEVICE_NAME));
        memcpy((byte *)&DEVICEIDResponse.uuid, &DEVICE_ID, sizeof(DEVICE_ID));
        Serial.write((byte *)&DEVICEIDResponse, sizeof(DEVICEIDResponse));
        Serial.write(TERMINATOR);
      }
      memset(&buffer, 0, BUFFER_SIZE); //clearing buffer
      serial_index = 0;
    }
    else
    {
      buffer[serial_index] = serialRead; //filling buffer
      serial_index++;                    //increesing serial index;
    }
  }

  slider_index++;
  if (slider_index == SLIDER_COUNT)
    slider_index = 0;

  button_index++;
  if (button_index == BUTTON_COUNT)
    button_index = 0;
}
