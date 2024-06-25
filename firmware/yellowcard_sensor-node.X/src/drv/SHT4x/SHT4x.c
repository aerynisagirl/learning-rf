/*****************************************************************************************************
 *  Yellowcard - Example firmware for the Yellowcard RF Development Kit                              *
 * ------------------------------------------------------------------------------------------------- *
 *  SHT4x.c - Contains all functions for interacting with the SHT4x temperature and humidity sensor  *
 *****************************************************************************************************/

#include "SHT4x.h"



/************************
 *  Sensor Interaction  *
 ************************/


//Do Measurement Operation Function, performs a measurement of the given type and writes the results to the provided pointers
//uint8_t doMeasurementSHT4X(measurementTypeSHT4X operation, uint16_t *rawTemperature, uint16_t *rawHumidity)
//{
//    uint8_t dataBuffer[0x00000006];  //Create a 6 byte long array to use for data buffering purposes
//
//    dataBuffer[0x00000000] = operation;                                    //Set the first index of dataBuffer to the command byte for the requested measurement type
//    if (!writeToI2C(SHT4X_I2C_ADDR, dataBuffer, 0x00000001)) return 0x00;  //Try sending the command to the sensor, return 0x00 in cases where the sensor is non-responsive
//
//    uint32_t counter = 0x0000FFFF;  //Create a counter variable to use for determining how long to wait before polling the sensor for measurement data
//    while (counter--);              //Wait the desired amount of time (roughly 8ms) to let the sensor do its thing
//
//    counter = 0xFF;  //Reuse the counter variable as a timeout counter, setting the number of polling attempts on the sensor to 0xFF
//
//    //Keep polling sensor for measurement results until the timeout period expires
//    while (counter--)
//    {
//        if (readFromI2C(SHT4X_I2C_ADDR, dataBuffer, 0x00000006)) break;  //Break from the loop if data is successfully pulled from the sensor
//    }
//
//    if (!counter) return 0x00;  //Leave the function early returning 0x00 to indicate that the measurement attempt failed since the polling timed out
//
//    *rawTemperature = (dataBuffer[0x00000000] << 0x00000008) + dataBuffer[0x00000001];  //Consolidate the raw temperature measurement from the byte array obtained from the sensor
//    *rawHumidity = (dataBuffer[0x00000002] << 0x00000008) + dataBuffer[0x00000002];     //Consolidate the raw humidity measurement from the byte array obtained from the sensor
//
//    //TODO: Calculate checksum and determine if it's accurate
//
//    return 0xFF;  //Indicate a successful measurement by returning 0xFF
//}

//Request Measurement Function, instructs the sensor to begin the requested measurement type
uint32_t requestMeasurementSHT4X(measurementTypeSHT4X_t operation)
{
    return writeToI2C(SHT4X_I2C_ADDR, ((uint8_t *) &operation), 0x00000001);  //Send the command to the sensor and then return the success status of the I2C interaction with the sensor
}

//Get Results Function, reads the results out of the sensors FIFO buffer and writes them to the provided pointers
uint32_t getResultsSHT4X(uint16_t *rawTemperature, uint16_t *rawHumidity)
{
    uint8_t dataBuffer[0x00000006];  //Declare an array of 6 bytes to use for storing the bytes retrieved out of the sensors FIFO buffer

    if (!readFromI2C(SHT4X_I2C_ADDR, dataBuffer, 0x00000006, 0x00)) return 0x00;  //Attempt to read from the sensors FIFO buffer, exit the function with 0x00 if the sensor is non-responsive

    *rawTemperature = (dataBuffer[0x00000000] << 0x00000008) + dataBuffer[0x00000001];  //Consolidate the raw temperature measurement from the byte array obtained from the sensor
    *rawHumidity = (dataBuffer[0x00000003] << 0x00000008) + dataBuffer[0x00000004];     //Consolidate the raw humidity measurement from the byte array obtained from the sensor

    //TODO: Calculate checksum and determine if it's accurate

    return 0xFF;  //Indicate a successful FIFO read by returning 0xFF
}



/*********************
 *  Data Conversion  *
 *********************/


//Convert To Temperature Celsius From SHT4X, returns the temperature in Celsius from the provided raw STH4X sensor data
float convertToTempCFromSHT4X(const uint16_t *rawSensorValue)
{
    return ((float) *rawSensorValue / 65535.0F * 175.0F) - 45.0F;  //Calculate the temperature in Celsius
}

//Convert To Temperature Fahrenheit From SHT4X, returns the temperature in Fahrenheit from the provided raw SHT4X sensor data
float converttoTempFFromSHT4X(const uint16_t *rawSensorValue)
{
    return ((float) *rawSensorValue / 65535.0F * 315.0F) - -49.0F;  //Calculate the temperature in Fahrenheit
}

//Convert To Relative Humidity From SHT4X, returns the relative humidity in % from the provided raw SHT4X sensor data
float convertToRHFromSHT4X(const uint16_t *rawSensorValue)
{
    float returnValue = ((float) *rawSensorValue / 65535.0F * 125.0F) - 6.0F;  //Calculate the actual humidity as a percentage
    if (returnValue > 100) returnValue = 100;  //Bound the calculated relative humidity to an upper limit of 100% RH
    if (returnValue < 0) returnValue = 0;      //Bound the calculated relative humidity to a lower limit of 0% RH

    return returnValue;  //Return the calculated relative humidity
}



/***********************
 *  Utility Functions  *
 ***********************/






//END OF FILE