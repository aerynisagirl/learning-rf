/****************************************************************************
 *  Yellowcard - Example firmware for the Yellowcard RF Development Kit     *
 * ------------------------------------------------------------------------ *
 *  Logging.h - Houses all things related to logging strings       *
 ****************************************************************************/

#ifndef _LOGGING_H_
#define _LOGGING_H_

//Include any libraries used by this file
#include <xc.h>           //Include the main header file for the XC32 compiler, provides register definitions
#include <sys/attribs.h>  //Include the attribs file, contains compiler level memory organization macros
#include <string.h>       //Include the default string library which has some handy memory and string manipulation functions


//Logging strings
extern const uint8_t __attribute__ ((space(prog), section(".logging_constants"))) logConstants_measurementReport[];
extern const uint8_t __attribute__ ((space(prog), section(".logging_constants"))) logConstants_packet[];

//Packet Type strings
extern const uint8_t __attribute__ ((space(prog), section(".logging_constants"))) logConstants_packetType_acknowledge[];
extern const uint8_t __attribute__ ((space(prog), section(".logging_constants"))) logConstants_packetType_event[];
extern const uint8_t __attribute__ ((space(prog), section(".logging_constants"))) logConstants_packetType_measureReport[];

extern const uint8_t* __attribute__ ((space(prog), section(".logging_constants_ptrs"))) logConstants_packetTypeLookup[];


//Define prototypes for functions used in the Logging source file
extern uint32_t constructMeasurementLog(uint8_t *stringBuffer,     //Construct Measurement Log Function, constructs a new string to log the provided measurement results
                                        const float *temperature,
                                        const float *humidity,
                                        const float *pressure);
extern uint32_t constructPacketLog(uint8_t *stringBuffer,          //Construct Packet Log Function, constructs a new string to log the provided packet bytes
                                   const uint8_t *packetBytes);

extern void uintToDecString(uint32_t numberToConvert, uint8_t *stringBuffer);  //Unsigned Integer To Decimal String, converts the provided unsigned integer into it's decimal string representation
extern void byteToHexString(uint8_t byteToConvert, uint8_t *stringBuffer);     //Byte To Hexadecimal String, converts the provided byte into it's hexadecimal string representation


#endif






//END OF FILE