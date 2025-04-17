#include "stateControl.h"

volatile bool led1State = false;
volatile int led1Brightness = 255;
volatile bool led2State = false;
volatile int led2Brightness = 255;
volatile bool led3State = false;
volatile int led3Brightness = 255;
volatile bool ledRGBState = false;
volatile int Red = 254;
volatile int Green = 254;
volatile int Blue = 254;
volatile bool relayState = false;
volatile int LedL = 254;

volatile int OZap;
volatile int Zap;
bool IsConnected = false;