/********************************************************************************************
 *  Yellowcard - Example firmware for the Yellowcard RF Development Kit                     *
 * ---------------------------------------------------------------------------------------- *
 *  Application.h - Houses all things related to the application and program state machine  *
 ********************************************************************************************/

#ifndef _APPLICATION_H_
#define _APPLICATION_H_

//Import any libraries used by this file
#include <xc.h>                   //Include the main header file for the XC32 compiler, provides register definitions
#include "PacketStructures.h"     //Include the packet structures header file to use for handling packet creation
#include "Logging.h"          //Include the logging header file that contains all things logging related
#include "drv/DPS368/DPS368.h"    //Include the driver for the DPS368 barometric pressure sensor
#include "drv/SHT4x/SHT4x.h"      //Include the driver for the SHT4x temperature and humidity sensor
#include "drv/SX1231H/SX1231H.h"  //Include the driver for the SX1231H sub-1GHz radio IC


//Define any enum types used within this file
typedef enum
{
    DO_RESET, DO_MEASUREMENTS, REPORT_MEASUREMENTS, MEASURE_FAIL, ENTER_SLEEP
} NodeState_t;


//Define any variables that are external to this file
extern volatile NodeState_t currentState;       //Used to track where program execution is currently taking place within the program state machine
extern const void (*handlerFunctionTable[])();  //Provides a lookup table of handler functions to allow for proper execution redirection after each state is processed


//State Machine Handler Functions
extern void __attribute__ ((section(".state_machine"))) onReset();             //On Reset Function, handles the startup of the application and sends a startup event packet
extern void __attribute__ ((section(".state_machine"))) doMeasurements();      //Do Measurements Function, performs the data collection routine required to obtain new sensor measurements
extern void __attribute__ ((section(".state_machine"))) reportMeasurements();  //Report Measurements Function, prepares the obtained measurements and then sends them over the air
extern void __attribute__ ((section(".state_machine"))) onMeasureFail();       //On Measure Fail Function, exception handling method for failed measurement attempts
extern void __attribute__ ((section(".state_machine"))) doSleepLowPower();     //Do Sleep Low Power Function, halts program execution with the MCU fully powered down until an interrupt occurs


#endif






//END OF FILE