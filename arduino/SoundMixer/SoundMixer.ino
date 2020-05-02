//#define DEBUG                                                               //enable debug mode
#define SLIDER_COUNT 6                                                        //define number of sliders
#define BUTTON_COUNT 5                                                        //define number of buttons
#define SAMPLES 12                                                            //define number of samples needed to averaging
#define DIFF 40                                                               //define input diff
#define MAP_DIFF 1                                                            //define number of out diff
#define MAX_ADC_VAL 4095                                                      //define in value range
#define MAX_MAP_VAL 100                                                       //define out value range
#define DEBOUNCE_TIME 150

const volatile int sliders[SLIDER_COUNT] = {PA5, PA4, PA3, PA2, PA1, PA0};    //sets sliders inputs
const volatile int buttons[BUTTON_COUNT] = {PB12, PB13, PB14, PB15, PA8};     //sets buttons input
volatile int lastSliderState[SLIDER_COUNT];                                   //constains last values of sliders
volatile int lastMapValue[SLIDER_COUNT];                                      //contains last map value of sliders

volatile int lastButtonReading[BUTTON_COUNT];                                 //contains last buttons states
volatile int lastDebounceTime[BUTTON_COUNT];                                  //contains last buttons debounce timings
volatile int lastButtonState[BUTTON_COUNT];                                   //contains last accepted button states

int slider_index;
int button_index;

//using in serial communication
struct SliderStruct{
  uint8_t command;
  uint8_t slider;
  uint16_t value;
};

struct ButtonStruct{
  uint8_t command;
  uint8_t button;
  uint8_t state;
};

void setup() {
  Serial.begin(115200);                                                       //serial setup(baudrate is ignored my mapple)

  for(int n = 0; n < SLIDER_COUNT; n++)                                       //sets pinMode for all sliders
    pinMode(sliders[n],INPUT_ANALOG);

  for(int n = 0; n < BUTTON_COUNT; n++)                                       //sets pinMode for all buttons
    pinMode(buttons[n], INPUT_PULLUP);
}

void loop() {

  int currentReading = 0;                                                     
  for(int n = 0;n < SAMPLES; n++)                                             //sampling current slider value
    currentReading += analogRead(sliders[slider_index]);

  currentReading /= SAMPLES;                                                  //averaging current slider value
  if(abs(currentReading - lastSliderState[slider_index]) >= DIFF){            //checking if value change is bigger than diff
    lastSliderState[slider_index] = currentReading;
    int mapValue = 0;
    if(currentReading + DIFF > MAX_ADC_VAL)                                   //used in case when value is close to end of the range
      mapValue = MAX_MAP_VAL;
    else
      mapValue = map(currentReading,0,MAX_ADC_VAL,0,MAX_MAP_VAL);             //in other case map value
    if(abs(mapValue - lastMapValue[slider_index]) >= MAP_DIFF){               //checking if map diff is bigger than value change
      lastMapValue[slider_index] = mapValue;
      struct SliderStruct slider = {.command = 0x01, .slider = slider_index, .value = mapValue}; //assign values to data struct
      Serial.write((byte *)&slider, sizeof(slider));                         //send data over the serial
      Serial.write(0xFF);                                                   //send termination byte help parsing data in other side
      #ifdef DEBUG
        Serial.print("Slider: ");
        Serial.print(slider.slider);
        Serial.print(" = ");
        Serial.println(slider.value);
      #endif
                                                  
    } 
  }

  int buttonReading = digitalRead(buttons[button_index]);
  if(buttonReading != lastButtonReading[button_index])
    lastDebounceTime[button_index] = buttonReading;

  if(millis() - lastDebounceTime[button_index] > DEBOUNCE_TIME)
  {
    if(buttonReading != lastButtonState[button_index])
    {
      lastButtonState[button_index] = buttonReading;
      struct ButtonStruct button = {.command = 0x02, .button = button_index, .state = buttonReading};
      Serial.write((byte *)&button, sizeof(button));
      Serial.write(0xFF);
      #ifdef DEBUG
        Serial.print("Button: ");
        Serial.print(button.button);
        Serial.print(" = ");
        Serial.println(button.state);
      #endif
    }
  }
  
  slider_index++;
  if(slider_index == SLIDER_COUNT)
    slider_index = 0;

  button_index++;
  if(button_index == BUTTON_COUNT)
    button_index = 0;
}
