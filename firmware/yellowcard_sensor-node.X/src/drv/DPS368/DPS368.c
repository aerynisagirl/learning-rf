/******************************************************************************************************************
 *  Yellowcard - Example firmware for the Yellowcard RF Development Kit                                           *
 * -------------------------------------------------------------------------------------------------------------- *
 *  DPS368.c - Contains all functions for interacting with the DPS368 temperature and barometric pressure sensor  *
 ******************************************************************************************************************/

#include "DPS368.h"



/***************
 *  Variables  *
 ***************/

//Measurement Scaling Factors
uint32_t presScalingFactorDPS368;  //Scaling factor to be applied to all raw pressure data
uint32_t tempScalingFactorDPS368;  //Scaling factor to be applied to all raw temperature data

//Sensor Calibration Coefficients
int16_t dps368Cal_c0;
int16_t dps368Cal_c1;
int32_t dps368Cal_c00;
int32_t dps368Cal_c10;
int16_t dps368Cal_c01;
int16_t dps368Cal_c11;
int16_t dps368Cal_c20;
int16_t dps368Cal_c21;
int16_t dps368Cal_c30;

//Scaling factor lookup table
const uint32_t dps368Cal_scalingFactors[] = {0x00080000,   //No oversampling
                                             0x00180000,   //2x oversampling
                                             0x00380000,   //4x oversampling
                                             0x00780000,   //8x oversampling
                                             0x0003E000,   //16x oversampling
                                             0x0007E000,   //32x oversampling
                                             0x000FE000,   //64x oversampling
                                             0x001FE000};  //128x oversampling



/***************************
 *  Sensor Initialization  *
 ***************************/


//Initialize DPS368 Function, configures the sensor to behave with the desired operating traits
uint32_t initializeDPS368(precisionDPS368_t presOversample, backgroundDPS368_t presMeasureRate, precisionDPS368_t tempOversample, backgroundDPS368_t tempMeasureRate, uint32_t enableFIFO)
{
    presOversample &= 0x00000007;  //Keep only the 3 least significant bits of the provided pressure oversampling setting
    tempOversample &= 0x00000007;  //Keep only the 3 least significant bits of the provided temperature oversampling setting
    
    presScalingFactorDPS368 = dps368Cal_scalingFactors[presOversample];  //Use the lookup table to set the scaling factor for pressure results based on the given oversampling setting
    tempScalingFactorDPS368 = dps368Cal_scalingFactors[tempOversample];  //Use the lookup table to set the scaling factor for temperature results based on the given oversampling setting

    uint8_t dataBuffer[0x00000005];                   //Create a 5 byte long array for buffering the bytes that shall be sent to the sensor to configure it as specified by the application
    uint8_t *arrayPointer = dataBuffer + 0x00000002;  //Declare a pointer that points to the array

    //Determine whether the temperature calibration coefficients are for the ASIC or MEMS sensor
    *arrayPointer = 0x28;  //Write the address of the COEF_SRCE register on the sensor into the array
    if (!readFromI2C(DPS368_I2C_ADDR, arrayPointer, 0x00000001, 0x01)) return 0x00000000;  //Attempt to read the contents of the register, exiting the function when no response is obtained

    arrayPointer = dataBuffer;  //Re-point arrayPointer back to the first index of the dataBuffer array
    *arrayPointer++ = 0x06;     //Write the address of the PRS_CFG register in the sensor to the first index of the configuration bytes

    *arrayPointer++ = ((presMeasureRate & 0x00000007) << 0x00000004) | (presOversample);                           //Form the configuration byte for the PRS_CFG register
    *arrayPointer++ = ((tempMeasureRate & 0x00000007) << 0x00000004) | (tempOversample) | (*arrayPointer & 0x80);  //Form the configuration byte for the TMP_CFG register
    *arrayPointer++ = 0x00;                                                                                        //Form the configuration byte for the MEAS_CFG register
    *arrayPointer = 0x00;                                                                                          //Form the starting configuration byte for the CFG_REG register

    if (enableFIFO) *arrayPointer |= 0x02;                    //Set Bit-1 in CFG_REG to enable the use of the FIFO buffer
    if (presOversample >= 0x00000004) *arrayPointer |= 0x04;  //Set Bit-2 in CFG_REG to activate the pressure result bit-shift mode when oversampling 8 or more times per measurement
    if (tempOversample >= 0x00000004) *arrayPointer |= 0x08;  //Set Bit-3 in CFG_REG to activate the temperature result bit-shift mode when oversampling 8 or more times per measurement

    return writeToI2C(DPS368_I2C_ADDR, dataBuffer, 0x00000005);  //Send the configuration data to the sensor returning whether the I2C write operation was successful or not
}



/************************
 *  Sensor Interaction  *
 ************************/


//Set Mode Function, changes the operating mode of the sensor to the given mode
uint32_t setModeDPS368(sensorModeDPS368_t newMode)
{
    uint8_t dataBuffer[0x00000002] = {0x08, newMode & 0x07};  //Form the data bytes that will be sent to the sensor to select the desired operating mode

    return writeToI2C(DPS368_I2C_ADDR, dataBuffer, 0x00000002);  //Send the configuration bytes to the sensor over I2C and return the success status of the write operation
}

//Get Result Status Function, provides the status of whether measurement results are ready or not
resultStatusDPS368_t getResultStatusDPS368()
{
    uint8_t dataBuffer = 0x08;  //Address of the MEAS_CFG register on the sensor
    if (!readFromI2C(DPS368_I2C_ADDR, &dataBuffer, 0x00000001, 0x00000001)) return NONE;  //Attempt to read the contents of the MEAS_CFG register on the sensor

    return (resultStatusDPS368_t) (dataBuffer & 0x30) >> 0x00000004;  //Return the obtained measurement status enum
}

//Clear FIFO Function, clears the contents of the FIFO buffer on the sensor
uint32_t clearFifoDPS368()
{
    uint8_t dataBuffer[0x00000002] = {0x0C, 0x80};  //Form the data bytes that will be sent to the sensor to flush the internal FIFO buffer

    return writeToI2C(DPS368_I2C_ADDR, dataBuffer, 0x00000002);  //Send the command bytes to the sensor over I2C and return the success status of the write operation
}

//Get Pressure Result Function, obtains the raw pressure measurement from the sensor
uint32_t getPressureResultDPS368(uint32_t *rawPressure)
{
    return getResultsAtAddressDPS368(0x00, rawPressure, 0x00000001);  //Read the 24-bit result value from the PRS_Bx registers on the sensor
}

//Get Temperature Result Function, obtains the raw temperature measurement from the sensor
uint32_t getTemperatureResultDPS368(uint32_t *rawTemperature)
{
    return getResultsAtAddressDPS368(0x03, rawTemperature, 0x00000001);  //Read the 24-bit result value from the TMP_Bx registers on the sensor
}

//Get Results From FIFO Function, reads the specified number of measurement from the sensor's FIFO into the provided array
uint32_t getResultsFromFifoDPS368(uint32_t *rawResults, uint32_t count)
{
    return getResultsAtAddressDPS368(0x00, rawResults, count);  //Read the desired number of 24-bit result values from the sensors FIFO buffer
}



/*********************
 *  Data Conversion  *
 *********************/


//Convert To Temperature Celsius From DPS368, returns the temperature in Celsius from the provided raw DPS368 sensor data
float convertToTempCFromDPS368(uint32_t rawTemperature)
{
    forceSign32(&rawTemperature, 0x00000018);                                       //Convert the provided 24-bit unsigned 2's complemented temperature value into a signed 32-bit integer
    float scaledTemp = (float) (int32_t) rawTemperature / tempScalingFactorDPS368;  //Calculate the scaled temperature result based on the temperature oversampling configuration

    return (scaledTemp * dps368Cal_c1) + (dps368Cal_c0 * 0.5F);  //Calculate the compensated temperature in Celsius as demonstrated by the DPS368 datasheet
}

//Convert To Pressure From DPS368, returns the pressure in Pascals from the provided raw DPS368 sensor data
float convertToPressureFromDPS368(uint32_t rawPressure, uint32_t rawTemperature)
{
    forceSign32(&rawPressure, 0x00000018);     //Convert the provided 24-bit unsigned 2's complemented pressure value into a signed 32-bit integer
    forceSign32(&rawTemperature, 0x00000018);  //Convert the provided 24-bit unsigned 2's complemented temperature value into a signed 32-bit integer

    float scaledPres = (float) (int32_t) rawPressure / presScalingFactorDPS368;     //Calculate the scaled pressure result based on the pressure oversampling configuration
    float scaledTemp = (float) (int32_t) rawTemperature / tempScalingFactorDPS368;  //Calculate the scaled temperature result based on the temperature oversampling configuration

    //Calculate the compensated pressure in Pascals as demonstrated by the DPS368 datasheet 
    float compensatedPres = (((scaledPres * dps368Cal_c30 + dps368Cal_c20) * scaledPres + dps368Cal_c10) * scaledPres) +
                            ((scaledPres * dps368Cal_c21 + dps368Cal_c11) * scaledPres * scaledTemp) + (scaledTemp + dps368Cal_c01) + dps368Cal_c00;

    return compensatedPres;  //Return the calculated compensated pressure
}



/***********************
 *  Utility Functions  *
 ***********************/


//Get Calibration Coefficients Function, reads the calibration coefficients from the sensor to use for processing the results
uint32_t readCalCoeffsDPS368()
{
    uint8_t dataBuffer[0x00000012];  //Create an array of 18 bytes to use as a buffer for reading data from the sensor

    //Verify that the calibration coefficients are ready to be retrieved from the sensor
    *dataBuffer = 0x08;                                                      //Select the MEAS_CFG register on the sensor
    if (!readFromI2C(DPS368_I2C_ADDR, dataBuffer, 0x00000001, 0x00000001));  //Attempt to read the contents of the MEAS_CFG register on the sensor, leave if no response is received

    if (!(*dataBuffer & 0x80)) return 0x00;  //Leave the function if the coefficients aren't available yet

    //Read the calibration coefficients from the sensor
    *dataBuffer = 0x10;                                                                        //Set the start address of the read sequence to the first byte of the calibration coefficients
    if (!readFromI2C(DPS368_I2C_ADDR, dataBuffer, 0x00000012, 0x00000001)) return 0x00000000;  //Read the memory space in which the calibration data lives in from the sensor into the dataBuffer array

    uint8_t *arrayPointer = dataBuffer;  //Declare a pointer to the dataBuffer array to use for iterating through each index of the array while extracting the coefficients

    //Extract the data from the byte array into their respective variables
    dps368Cal_c0 = (*arrayPointer++ << 0x00000004) | ((*arrayPointer & 0xF0) >> 0x00000004);
    dps368Cal_c1 = ((*arrayPointer++ & 0x0F) << 0x00000008) | (*arrayPointer++);
    dps368Cal_c00 = (*arrayPointer++ << 0x0000000C) | (*arrayPointer++ << 0x00000004) | ((*arrayPointer & 0xF0) >> 0x00000004);
    dps368Cal_c10 = ((*arrayPointer++ & 0x0F) << 0x00000010) | (*arrayPointer++ << 0x00000008) | (*arrayPointer++);
    dps368Cal_c01 = (*arrayPointer++ << 0x00000008) | (*arrayPointer++);
    dps368Cal_c11 = (*arrayPointer++ << 0x00000008) | (*arrayPointer++);
    dps368Cal_c20 = (*arrayPointer++ << 0x00000008) | (*arrayPointer++);
    dps368Cal_c21 = (*arrayPointer++ << 0x00000008) | (*arrayPointer++);
    dps368Cal_c30 = (*arrayPointer++ << 0x00000008) | (*arrayPointer);

    //Correct the sign of the values stored in the variables when required
    forceSign16(&dps368Cal_c0, 0x0000000C);   //Apply 12-bit 2's complements to the number to correct the sign
    forceSign16(&dps368Cal_c1, 0x0000000C);   //Apply 12-bit 2's complements to the number to correct the sign
    forceSign32(&dps368Cal_c00, 0x00000014);  //Apply 20-bit 2's complements to the number to correct the sign
    forceSign32(&dps368Cal_c10, 0x00000014);  //Apply 20-bit 2's complements to the number to correct the sign

    return 0xFFFFFFFF;  //Return a non-negative value to indicate the coefficients were read successfully
}

//Get Results At Address Function, obtains the requested number of 24-bit results from the sensor at the provided start address
uint32_t getResultsAtAddressDPS368(uint32_t address, uint32_t *results, uint32_t count)
{
    uint32_t byteCount = count * 0x00000003;  //Calculate the number of bytes that need to be read to obtain the requested result count
    uint8_t dataBuffer[byteCount];            //Create an array containing 3 bytes for every result requested to retrieve

    *dataBuffer = address;  //Begin the read process at the provided start address
    if (!readFromI2C(DPS368_I2C_ADDR, dataBuffer, byteCount, 0x00000001)) return 0x00000000;  //Attempt to read the contents of result registers on the sensor

    uint8_t *arrayPointer = dataBuffer;  //Declare a pointer to the dataBuffer array to use for iterating through each index of the array while extracting the measurement results

    //Extract each individual result from the obtained byte array and store them into the array provided
    while (count--)
    {
        *results++ = (*arrayPointer++ << 0x00000010) | (*arrayPointer++ << 0x00000008) | (*arrayPointer++);  //Combine the next 3 bytes in the dataBuffer array to form a new 24-bit measurement
    }

    return 0xFFFFFFFF;  //Return a non-negative value to indicate the results were obtained successfully
}

//Force Sign 16 Function, forces the given value into it's 16-bit signed equivalent using the provided bit-depth
void forceSign16(int16_t *value, uint32_t bitDepth)
{
    uint16_t cutoff = 0x0001 << (bitDepth - 0x00000001);  //Calculate the half way point for a signed value of the given bit depth, storing it in the cutoff variable

    if (*value & cutoff) *value -= (cutoff << 0x00000001);  //Force the value to a negative in 32-bit form when it's negative at the specified bit-depth
}

//Force Sign 32 Function, forces the given value into it's 32-bit signed equivalent using the provided bit-depth
void forceSign32(int32_t *value, uint32_t bitDepth)
{
    uint32_t cutoff = 0x00000001 << (bitDepth - 0x00000001);  //Calculate the half way point for a signed value of the given bit depth, storing it in the cutoff variable

    if (*value & cutoff) *value -= (cutoff << 0x00000001);  //Force the value to a negative in 32-bit form when it's negative at the specified bit-depth
}

//Is Pressure Function, gives boolean status as to whether a given value obtained from the sensors FIFO is a pressure measurement
uint32_t isPressureDPS368(uint32_t *rawSensorValue)
{
    return (*rawSensorValue & 0x00000001) ? 0xFFFFFFFF : 0x00000000;  //Give a non-negative value when Bit-1 of the given sensor result is sets
}






//END OF FILE