/***************************************************************************
 *  Yellowcard - Example firmware for the Yellowcard RF Development Kit    *
 * ----------------------------------------------------------------------- *
 *  SHT4x.h - Driver for the SHT40/SHT41 temperature and humidity sensors  *
 ***************************************************************************/

#ifndef _SHT4X_H_
#define	_SHT4X_H_

//Import any libraries used by this file
#include <xc.h>   //Include the main header file for the XC32 compiler, provides register definitions
#include "../HAL.h"  //Include the HAL header which contains the I2C interface through which the sensor is attached to


//Define any constants specific to this sensor
#ifndef SHT4X_I2C_ADDR
#define SHT4X_I2C_ADDR    0x44  //Sets the I2C address of the device
#endif


//Define any variables that are external to this file


//Define any enum types used within this file
typedef enum
{
    LOW_PRECISION_NO_HEATER = 0xE0,
    MED_PRECISION_NO_HEATER = 0xF6,
    HIGH_PRECISION_NO_HEATER = 0xFD,
    HIGH_PRECISION_20MW_HEATER_100MS = 0x15,
    HIGH_PRECISION_20MW_HEATER_1S = 0x1E,
    HIGH_PRECISION_110MW_HEATER_100MS = 0x24,
    HIGH_PRECISION_110MW_HEATER_1S = 0x2F,
    HIGH_PRECISION_200MW_HEATER_100MS = 0x32,
    HIGH_PRECISION_200MW_HEATER_1S = 0x39
} measurementTypeSHT4X_t;


//Sensor Interaction Functions
extern uint32_t requestMeasurementSHT4X(measurementTypeSHT4X_t operation);  //Request Measurement Function, instructs the sensor to begin the requested measurement type
extern uint32_t getResultsSHT4X(uint16_t *rawTemperature,                 //Get Results Function, reads the results out of the sensors FIFO buffer and writes them to the provided pointers
                               uint16_t *rawHumidity);

//Data Conversion Functions
extern float convertToTempCFromSHT4X(const uint16_t *rawSensorValue);  //Convert To Temperature Celsius From SHT4X, returns the temperature in Celsius from the provided raw STH4X sensor data
extern float converttoTempFFromSHT4X(const uint16_t *rawSensorValue);  //Convert To Temperature Fahrenheit From SHT4X, returns the temperature in Fahrenheit from the provided raw SHT4X sensor data
extern float convertToRHFromSHT4X(const uint16_t *rawSensorValue);     //Convert To Relative Humidity From SHT4X, returns the relative humidity in % from the provided raw SHT4X sensor data


#endif






//END OF FILE