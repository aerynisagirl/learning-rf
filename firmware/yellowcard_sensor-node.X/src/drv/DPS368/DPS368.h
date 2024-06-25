/**********************************************************************************
 *  Yellowcard - Example firmware for the Yellowcard RF Development Kit           *
 * ------------------------------------------------------------------------------ *
 *  DPS368.h - Driver for the DPS368 temperature and barometric pressure sensors  *
 **********************************************************************************/

#ifndef _DPS368_H_
#define	_DPS368_H_

//Import any libraries used by this file
#include <xc.h>      //Include the main header file for the XC32 compiler, provides register definitions
#include "../HAL.h"  //Include the HAL header which contains the I2C interface through which the sensor is attached to


//Define any constants specific to this sensor
#ifndef DPS368_I2C_ADDR
#define DPS368_I2C_ADDR    0x76  //Sets the I2C address of the device
#endif


//Define any enum types used within this file
typedef enum
{
    OVERSAMPLE_1, OVERSAMPLE_2, OVERSAMPLE_4, OVERSAMPLE_8, OVERSAMPLE_16, OVERSAMPLE_32, OVERSAMPLE_64, OVERSAMPLE_128
} precisionDPS368_t;

typedef enum
{
    BACKGROUND_1HZ, BACKGROUND_2HZ, BACKGROUND_4HZ, BACKGROUND_8HZ, BACKGROUND_16HZ, BACKGROUND_32HZ, BACKGROUND_64HZ, BACKGROUND_128HZ
} backgroundDPS368_t;

typedef enum
{
    IDLE = 0x00, SINGLE_PRES = 0x01, SINGLE_TEMP = 0x02, CONT_PRES = 0x05, CONT_TEMP = 0x06, CONT_BOTH = 0x07
} sensorModeDPS368_t;

typedef enum
{
    NONE, PRES_READY, TEMP_READY, BOTH_READY
} resultStatusDPS368_t;


//Initialization Functions
extern uint32_t initializeDPS368(precisionDPS368_t presOversample,    //Initialize DPS368 Function, configures the sensor to behave with the desired operating traits
                                 backgroundDPS368_t presMeasureRate,
                                 precisionDPS368_t tempOversample,
                                 backgroundDPS368_t tempMeasureRate,
                                 uint32_t enableFIFO);

//Interaction Functions
extern uint32_t setModeDPS368(sensorModeDPS368_t newMode);             //Set Mode Function, changes the operating mode of the sensor to the given mode
extern resultStatusDPS368_t getResultStatusDPS368();                   //Get Result Status Function, provides the status of whether measurement results are ready or not
extern uint32_t clearFifoDPS368();                                     //Clear FIFO Function, clears the contents of the FIFO buffer on the sensor
extern uint32_t getPressureResultDPS368(uint32_t *rawPressure);        //Get Pressure Result Function, obtains the raw pressure measurement from the sensor
extern uint32_t getTemperatureResultDPS368(uint32_t *rawTemperature);  //Get Temperature Result Function, obtains the raw temperature measurement from the sensor
extern uint32_t getResultsFromFifoDPS368(uint32_t *rawResults,         //Get Results From FIFO Function, reads the specified number of results from the sensor's FIFO into the provided array
                                         uint32_t count);

//Data Conversion Functions
extern float convertToTempCFromDPS368(uint32_t rawTemperature);     //Convert To Temperature Celsius From DPS368, returns the temperature in Celsius from the provided raw DPS368 sensor data
extern float convertToPressureFromDPS368(uint32_t rawPressure,      //Convert To Pressure From DPS368, returns the pressure in Pascals from the provided raw DPS368 sensor data
                                         uint32_t rawTemperature);

//Utility Functions
extern uint32_t readCalCoeffsDPS368();                        //Get Calibration Coefficients Function, reads the calibration coefficients from the sensor to use for processing the results
extern uint32_t getResultsAtAddressDPS368(uint32_t address,   //Get Results At Address Function, obtains the requested number of 24-bit results from the sensor at the provided start address
                                          uint32_t *results,
                                          uint32_t count);
extern void forceSign16(int16_t *value, uint32_t bitDepth);   //Force Sign 16 Function, forces the given value into it's 16-bit signed equivalent using the provided bit-depth
extern void forceSign32(int32_t *value, uint32_t bitDepth);   //Force Sign 32 Function, forces the given value into it's 32-bit signed equivalent using the provided bit-depth
extern uint32_t isPressureDPS368(uint32_t *rawSensorValue);   //Is Pressure Function, gives boolean status as to whether a given value obtained from the sensors FIFO is a pressure measurement


#endif






//END OF FILE