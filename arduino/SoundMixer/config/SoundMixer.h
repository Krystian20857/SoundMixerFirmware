//#define DEBUG            //enable debug mode
#define SLIDER_COUNT 6   //define number of sliders
#define BUTTON_COUNT 5   //define number of buttons
#define LED_COUNT 3      //define number of leds
#define SAMPLES 12       //define number of samples needed to averaging
#define DIFF 40          //define input diff
#define MAP_DIFF 1       //define number of out diff
#define MAX_ADC_VAL 4095 //define in value range
#define MAX_MAP_VAL 100  //define out value range
#define DEBOUNCE_TIME 50 //define buttons debounce time
#define TERMINATOR 0xFF  //define termination char use in serial communication
#define BUFFER_SIZE 200  //define serial buffer size
#define SLIDER_THROTTLE 10
const char DEVICE_NAME[30] PROGMEM = "My Sound Mixer";
const uint8_t DEVICE_ID[6] PROGMEM = {0x23, 0xDD, 0x34, 0x93, 0x65, 0x15};

const volatile int sliders[SLIDER_COUNT] = {PA5, PA4, PA3, PA2, PA1, PA0}; //sets sliders inputs
const volatile int buttons[BUTTON_COUNT] = {PB12, PB13, PB14, PB15, PA8};  //sets buttons input
const volatile int leds[LED_COUNT] = {PB8, PB9, PC13};                     //sets leds outputs