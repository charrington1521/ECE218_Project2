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

#define DAYLIGHT_LEVEL 0
#define DUSK_LEVEL 0

#define HEADLIGHT_OFF_THRESHOLD 0.33
#define HEADLIGHT_ON_THRESHOLD 0.66

#define TIME_INCREMENT_MS                       10
#define DEBOUNCE_BUTTON_TIME_MS                 40

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
buttonState_t ignitionButtonState;

//=====[Declarations (prototypes) of public functions]=========================

/**
 * Returns the mode of the headlight based on potentiometer reading.
 * @Return OFF_MODE, AUTO_MODE, ON_MODE
 */
headlightMode_t getHeadlightMode();

/**
 * Initialize the inputs
 */
void inputsInit();

/**
 * Initialize the outputs
 */ 
void outputsInit();

/**
 * Update the headlights. They should be off when the ignition is off.
 * Otherwise they are set by their mode. In auto uses daylight readings
 * to automatically switch between on and off.
 */
void headlightUpdate();

/**
 * Updates the ignition. The ignition goes on when the driver is seated
 * and the ignition button is released. It goes off when the ignition button
 * is pushed and released again. 
 * If other methods want to know if the ignition is on, they should use
 * ignitionLed.read()
 */
void ignitionUpdate();

/**
 * Initialize the debounce button state. 
 */
void debounceButtonInit();

/**
 * @Return true when a buttonReleasedEvent occurs
 */
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
    while (true) {
        // char str[8];
        // sprintf (str, "%.2f\r\n", f);
        // int len = strlen(str);
        // uartUsb.write(str, len);
        debounceButtonUpdate();
        ignitionUpdate();
        headlightUpdate();
        delay(TIME_INCREMENT_MS);
    }
}

//=====[Implementation of global functions]====================================

headlightMode_t getHeadlightMode() {
    return OFF_MODE;
}

void inputsInit() {
    driverOccupancy.mode(PullDown);
    debounceButtonInit();
}

void outputsInit() {
    lowBeamLampLeft = OFF;
    lowBeamLampRight = OFF;
}

void headlightUpdate() {

}

void ignitionUpdate() {
    bool enterButtonReleasedEvent = debounceButtonUpdate();
    if (ignitionLed.read()) {
        if (enterButtonReleasedEvent) {
            ignitionLed = OFF;
        }
    } else {
        if (driverOccupancy) {
            uartUsb.write("Driver In Seat\r\n", 16);
        }
        if (driverOccupancy && enterButtonReleasedEvent) {
            ignitionLed = ON;
        }
    }
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