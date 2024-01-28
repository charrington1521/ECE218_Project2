/*! @mainpage Automatic Headlight Control
* @date Wednesday, January 24, 2024
* @authors Collin Harrington, ***********
* @section genDesc General Description
*
*
* This is the implementation of a system that prototypes
* automatic headlight control for motor vehicles. 
*
* @section changelog Changelog
* | Date | Description |
* |:----------:|:-----------------------------------------------|
* | 01/24/2024 | First version of program |
* | 01/24/2024 | Added objects and defines clearly required by 
*                project directions |
* | 01/24/2024 | Setup for daylight sensor testing |
* | 01/28/2024 | Prototypes and empty implementations added |
*
*/

//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

//=====[Defines]===============================================================

#define DAYLIGHT_LEVEL 0.80
#define DUSK_LEVEL 0.65

#define HEADLIGHT_OFF_THRESHOLD 0.33
#define HEADLIGHT_ON_THRESHOLD 0.66

#define TIME_INCREMENT_MS                       10
#define DEBOUNCE_BUTTON_TIME_MS                 40

#define HEADLIGHT_ON_TIME 1000
#define HEADLIGHT_OFF_TIME 2000

//=====[Declaration of public data types]=================

typedef enum {
    OFF_MODE,
    AUTO_MODE,
    ON_MODE
} headlightMode_t;

typedef enum {
    BUTTON_UP,
    BUTTON_DOWN,
    BUTTON_FALLING,
    BUTTON_RISING
} buttonState_t;

//=====[Declaration and initialization of public global objects]===============

AnalogIn headlightModeSelector(A1);
AnalogIn daylightSensor(A0);

DigitalIn ignitionButton(BUTTON1);
DigitalIn driverOccupancy(D2);

DigitalOut ignitionLed(LED2);
DigitalOut lowBeamLampLeft(D6);
DigitalOut lowBeamLampRight(D7);

UnbufferedSerial uartUsb(USBTX, USBRX, 115200);

//=====[Declaration and initialization of public global variables]=============

int accumulatedHeadlightTime          = 0;

int accumulatedDebounceButtonTime     = 0;
int numberOfIgnitionButtonReleasedEvents = 0;
buttonState_t ignitionButtonState;

//=====[Declarations (prototypes) of public functions]=========================

headlightMode_t getHeadlightMode();

void inputsInit();
void outputsInit();

void headlightUpdate();
void ignitionUpdate();

void debounceButtonInit();
bool debounceButtonUpdate();

//=====[Main function, the program entry point after power on or reset]========

/**
* Calls functions to initialize the declared input and output objects, and to
* implement the system behavior.
* @param none
* @return The returned value represents the success
* of application.
*/
int main()
{
    inputsInit();
    outputsInit();
    debounceButtonInit();
    while (true) {
        debounceButtonUpdate();
        ignitionUpdate();
        headlightUpdate();
        delay(TIME_INCREMENT_MS);
    }
}

//=====[Implementation of global functions]====================================

headlightMode_t getHeadlightMode() {
if(headlightModeSelector <= HEADLIGHT_OFF_THRESHOLD){
    return OFF_MODE;
}
if(headlightModeSelector >= HEADLIGHT_ON_THRESHOLD){
    return ON_MODE;
}
else{
    return AUTO_MODE;
}

}

void inputsInit() {

}

void outputsInit() {

}

void headlightUpdate() {
    if(ignitionLed){
        switch(getHeadlightMode()){
            case OFF_MODE:
            lowBeamLampLeft = 0;
            lowBeamLampRight = 0;
            break;
            case ON_MODE:
            lowBeamLampLeft = 1;
            lowBeamLampRight = 1;
            break;
            case AUTO_MODE:
            
            if(daylightSensor.read() <= DUSK_LEVEL){ 
                accumulatedHeadlightTime = accumulatedHeadlightTime + TIME_INCREMENT_MS;
                if(accumulatedHeadlightTime == HEADLIGHT_OFF_TIME){
                    lowBeamLampLeft = 0;
                    lowBeamLampRight = 0;
                    accumulatedHeadlightTime = 0;
                }
                }
            if(daylightSensor.read() <= DUSK_LEVEL){
                accumulatedHeadlightTime = accumulatedHeadlightTime + TIME_INCREMENT_MS;
                if(accumulatedHeadlightTime == HEADLIGHT_ON_TIME){
                    lowBeamLampLeft = 1;
                    lowBeamLampRight = 1;
                    accumulatedHeadlightTime = 0;
                }
            }
            else{
                lowBeamLampLeft = lowBeamLampLeft;
                lowBeamLampRight = lowBeamLampRight;

            }
            break;
            }
        }
    }



void ignitionUpdate() {

}

void debounceButtonInit()
{
    if( ignitionButton == 1) {
        ignitionButtonState = BUTTON_UP;
    } else {
        ignitionButtonState = BUTTON_DOWN;
    }
}

bool debounceButtonUpdate()
{
    bool ignitionButtonReleasedEvent = false;
    switch( ignitionButtonState ) {

    case BUTTON_UP:
        if( ignitionButton == 0 ) {
            ignitionButtonState = BUTTON_FALLING;
            accumulatedDebounceButtonTime = 0;
        }
        break;

    case BUTTON_FALLING:
        if( accumulatedDebounceButtonTime >= DEBOUNCE_BUTTON_TIME_MS ) {
            if( ignitionButton == 0 ) {
                ignitionButtonState = BUTTON_DOWN;
            } else {
                ignitionButtonState = BUTTON_UP;
            }
        }
        accumulatedDebounceButtonTime = accumulatedDebounceButtonTime +
                                        TIME_INCREMENT_MS;
        break;

    case BUTTON_DOWN:
        if( ignitionButton == 1 ) {
            ignitionButtonState = BUTTON_RISING;
            accumulatedDebounceButtonTime = 0;
        }
        break;

    case BUTTON_RISING:
        if( accumulatedDebounceButtonTime >= DEBOUNCE_BUTTON_TIME_MS ) {
            if( ignitionButton == 1 ) {
                ignitionButtonState = BUTTON_UP;
                ignitionButtonReleasedEvent = true;
            } else {
                ignitionButtonState = BUTTON_DOWN;
            }
        }
        accumulatedDebounceButtonTime = accumulatedDebounceButtonTime +
                                        TIME_INCREMENT_MS;
        break;

    default:
        debounceButtonInit();
        break;
    }
    return ignitionButtonReleasedEvent;
}