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
*
*/

//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

//=====[Defines]===============================================================

#define DAYLIGHT_LEVEL 0
#define DUSK_LEVEL 0

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

//=====[Declaration and initialization of public global types]=================

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
    while (true) {
        char str[10];
        sprintf (str, "%.2f\r\n", daylightSensor.read());
        int len = strlen(str);
        uartUsb.write("", len);
    }
}

//=====[Implementation of global functions]====================================


