/*********************************************************************************************
 *  Yellowcard - Example firmware for the Yellowcard RF Development Kit                      *
 * ----------------------------------------------------------------------------------------- *
 *  Logging.c - Contains all string constants and construction functions related to logging  *
 *********************************************************************************************/

#include "Logging.h"



/***********************
 *  Logging Constants  *
 ***********************/

const uint8_t logConstants_measurementReport[] = "\n\n\n\nMeasurement\n  Temperature:   C\n     Humidity:   %\n     Pressure:   Pa\0";
const uint8_t logConstants_packet[] = "\n\nPacket\n   Length:  \n  Address:  \n     Type:  \n  Frame #:  \n      Raw: ";

const uint8_t logConstants_packetType_acknowledge[] = "ACKNOWLEDGE\0";
const uint8_t logConstants_packetType_event[] = "EVENT\0";
const uint8_t logConstants_packetType_measureReport[] = "MEASURE_REPORT\0";

const uint8_t *logConstants_packetTypeLookup[] = {logConstants_packetType_acknowledge,
                                                  logConstants_packetType_event,
                                                  logConstants_packetType_measureReport};



/****************************
 *  Construction Functions  *
 ****************************/


//Construct Measurement Log Function, constructs a new string to log the provided measurement results
uint32_t constructMeasurementLog(uint8_t *stringBuffer, const float *temperature, const float *humidity, const float *pressure)
{
    int32_t dataBuffer;     //Reserve a new 32-bit variable in RAM to use for doing number manipulation while creating the measurement report
    uint32_t stringLength;  //Create a new variable to use for tracking the length of the string being constructed

    memcpy(stringBuffer, logConstants_measurementReport, 0x00000020);  //Copy the first part of the measurement report string into the string buffer
    stringLength = 0x00000020;                                         //Set the starting length of the string to 32 characters
    dataBuffer = (*temperature * 100.0F);                              //Multiply the most recent temperature measurement by 100, then store it in the dataBuffer with no decimal

    //Handle scenarios where the value of dataBuffer is actually a negative
    if (dataBuffer < 0x00000000)
    {
        stringBuffer[stringLength++] = 0x2D;  //Add a minus character to the string buffer to indicate the negative number
        dataBuffer = -dataBuffer;             //Force the contents of dataBuffer back into the positives again
    }

    uintToDecString(dataBuffer / 0x00000064, stringBuffer + stringLength);  //Convert the whole number portion of the temperature into a string and put it into the string buffer
    stringLength = strlen(stringBuffer);                                    //Find the new length of the string with the whole number added into it
    dataBuffer %= 0x00000064;                                               //Modulo the contents of dataBuffer by 100, storing the result back into the dataBuffer variable

    //Only add the decimal point and digits when actually required, otherwise don't bother
    if (dataBuffer)
    {
        stringBuffer[stringLength++] = 0x2E;                                    //Place the decimal point right after the whole number portion
        uintToDecString(dataBuffer % 0x00000064, stringBuffer + stringLength);  //Convert the decimal portion of the temperature into a string and put it into the string buffer
        stringLength = strlen(stringBuffer);                                    //Find the new length of the string with the decimal places added into it
    }

    memcpy(stringBuffer + stringLength, logConstants_measurementReport + 0x00000020, 0x00000013);  //Load the next segment of the report string into the buffer
    stringLength += 0x00000013;                                                                    //Add the appropriate amount to stringLength to compensate for the added characters
    uintToDecString((uint32_t) *humidity, stringBuffer + stringLength);                            //Convert the measured RH into it's string equivalent and append it to the string buffer
    stringLength = strlen(stringBuffer);                                                           //Find the new length of the string with the humidity measurement added in

    memcpy(stringBuffer + stringLength, logConstants_measurementReport + 0x00000033, 0x00000013);  //Load the next segment of the report string into the buffer
    stringLength += 0x00000013;                                                                    //Add the appropriate amount to stringLength to compensate for the added characters
    uintToDecString((uint32_t) *pressure, stringBuffer + stringLength);                            //Convert the measured pressure into a string and append it to the string buffer
    stringLength = strlen(stringBuffer);                                                           //Find the new length of the string with the barometric pressure value added

    memcpy(stringBuffer + stringLength, logConstants_measurementReport + 0x00000046, 0x00000004);  //Add the final part of the report string into the buffer
    stringLength += 0x00000004;                                                                    //Add the appropriate amount to stringLength to compensate for the added characters

    return stringLength;  //Leave the function returning the final length of the constructed string
}

//Construct Packet Log Function, constructs a new string to log the provided packet bytes
uint32_t constructPacketLog(uint8_t *stringBuffer, const uint8_t *packetBytes)
{
    uint32_t stringLength;                          //Create a new variable to use for tracking the length of the string being constructed
    uint32_t counter = *packetBytes;                //Store the size of the provided packet locally in a new variable to use for later
    uint32_t dataBuffer = packetBytes[0x00000001];  //Reserve a new 32-bit variable in RAM to use for doing number manipulation while creating the log message

    memcpy(stringBuffer, logConstants_packet, 0x00000015);  //Copy the first part of the packet log string into the string buffer
    stringLength = 0x00000015;                              //Set the starting length of the string to 22 characters
    uintToDecString(counter, stringBuffer + stringLength);  //Put the decimal representation of the packet size
    stringLength = strlen(stringBuffer);                    //Find the new length of the string with the packet size added in

    memcpy(stringBuffer + stringLength, logConstants_packet + 0x00000015, 0x0000000D);  //Load the next segment of the log string into the buffer
    stringLength += 0x0000000D;                                                         //Add the appropriate amount to stringLength to compensate for the added characters
    uintToDecString(dataBuffer, stringBuffer + stringLength);                           //Convert the destination address value into it's decimal form and append it to the string buffer
    stringLength = strlen(stringBuffer);                                                //Find the new length of the string with the destination address added

    uint8_t *typeString = (uint8_t *) logConstants_packetTypeLookup[packetBytes[0x00000002]];  //Create a pointer that points to the string that represents the packet type
    memcpy(stringBuffer + stringLength, logConstants_packet + 0x00000022, 0x0000000D);         //Load the next segment of the log string into the buffer
    stringLength += 0x0000000D;                                                                //Add the appropriate amount to stringLength to compensate for the added characters
    memcpy(stringBuffer + stringLength, typeString, strlen(typeString) + 0x00000001);          //Copy the string constants that represents the packet type into the buffer
    stringLength = strlen(stringBuffer);                                                       //Find the new length of the string with the packet type added

    memcpy(stringBuffer + stringLength, logConstants_packet + 0x0000002F, 0x0000000D);  //Load the next segment of the log string into the buffer
    stringLength += 0x0000000D;                                                         //Add the appropriate amount to stringLength to compensate for the added characters
    dataBuffer = (packetBytes[0x00000003] << 0x00000008) | packetBytes[0x00000004];     //Combine the next 3 bytes of the packet to calculate the frame number of the packet
    uintToDecString(dataBuffer, stringBuffer + stringLength);                           //Convert the frame number into it's decimal form and append it to the string buffer
    stringLength = strlen(stringBuffer);                                                //Find the new length of the string with the frame number added

    memcpy(stringBuffer + stringLength, logConstants_packet + 0x0000003C, 0x0000000C);  //Load the next segment of the log string into the buffer
    stringLength += 0x0000000C;                                                         //Add the appropriate amount to stringLength to compensate for the added characters

    //Create a series of hexadecimal translations of the raw packet
    while (counter--)
    {
        stringBuffer[stringLength++] = 0x20;                             //Append a space just before the hexadecimal string
        byteToHexString(*(packetBytes++), stringBuffer + stringLength);  //Convert the next byte of the packet into it's string hexadecimal representation and append it to stringBuffer
        stringLength += 0x00000002;                                      //Increment the value of the stringLength variable by 2 to compensate for the 2 newly added bytes
    }

    stringBuffer[stringLength++] = 0x00;  //Null terminate the end of the string
    return stringLength;                  //Leave the function returning the final length of the constructed string
}



/***********************
 *  Utility Functions  *
 ***********************/


//Unsigned Integer To Decimal String, converts the provided unsigned integer into it's string representation
void uintToDecString(uint32_t numberToConvert, uint8_t *stringBuffer)
{
	uint32_t counter = 0x00000000;      //Create a variable to use for tracking the digit that's being converted
    uint8_t tempBuffer[0x0000000B];     //Create an array of 11 bytes to use for storing the generated ASCII digits before copying them into the main byte 
    uint8_t *tempPointer = tempBuffer;  //Initialize a new pointer that points at the tempBuffer array

    //Perform digit separation for the size of the provided buffer
	while (counter++ <= 0x0000000B)
    {
    	*(tempPointer++) = (numberToConvert % 0x0000000A) | 0x00000030;  //Modulo the contents of numberToConvert by 10, ORing 0x30 to the result to convert the digit into an ASCII character
        numberToConvert /= 0x0000000A;                                   //Divide numberToConvert by 10, then put the result back into the numberToConvert variable
        if (!numberToConvert) break;                                     //Break from the loop whenever numberToConvert becomes empty
    }

    //Starting from the array index given by the value of the counter variable, start reversing the contents of the character array so that it's legible
    while (counter--) *(stringBuffer++) = *(--tempPointer);  //Copy the character at the address before the one stored in tempPointer into the next character in the stringBuffer array

    *stringBuffer = 0x00;  //Complete the string by null terminating it with 0x00
}

//Byte To Hexadecimal String, converts the provided byte into it's hexadecimal string representation
void byteToHexString(uint8_t byteToConvert, uint8_t *stringBuffer)
{
    *stringBuffer = ((byteToConvert & 0x000000F0) >> 0x00000004) | 0x30;  //Mask out the last 4 bits of the provided byte and create the first ASCII character for it
    if (*stringBuffer > 0x39) *stringBuffer += 0x07;                      //Add 0x07 to the character if it's supposed to be a letter
    *(++stringBuffer) = (byteToConvert & 0x0000000F) | 0x30;              //Mask out the first 4 bits of the provided byte and create the second ASCII character for it
    if (*stringBuffer > 0x39) *stringBuffer += 0x07;                      //Add 0x07 to the character if it's supposed to be a letter
    *(++stringBuffer) = 0x00;                                             //Complete the string by null terminating it with 0x00
}






//END OF FILE