#include <stdio.h>
#include <stdint.h>


void uintToString(uint32_t numberToConvert, uint8_t *stringBuffer, uint32_t bufferSize)
{
	uint32_t counter = 0x00000000;      //Create a variable to use for tracking the digit that's being converted
    uint8_t tempBuffer[0x0000000B];     //Create an array of 11 bytes to use for storing the generated ASCII digits before copying them into the main byte 
    uint8_t *tempPointer = tempBuffer;  //Initialize a new pointer that points at the tempBuffer array

    //Perform digit separation for the size of the provided buffer
	while (counter++ < bufferSize)
    {
    	*(tempPointer++) = (numberToConvert % 0x0000000A) | 0x00000030;  //Modulo the contents of numberToConvert by 10, ORing 0x30 to the result to convert the digit into an ASCII character
        numberToConvert /= 0x0000000A;                                   //Divide numberToConvert by 10, then put the result back into the numberToConvert variable
        if (!numberToConvert) break;                                     //Break from the loop whenever numberToConvert becomes empty
    }

    //Starting from the array index given by the value of the counter variable, start reversing the contents of the character array so that it's legible
    while (counter--) *(stringBuffer++) = *(--tempPointer);  //Copy the character at the address before the one stored in tempPointer into the next character in the stringBuffer array

    *stringBuffer = 0x00;  //Complete the string by null terminating it with 0x00
}


int main()
{
    uint8_t chars[0x000000FF];
    uint32_t testNumber = 12345;

    uintToString(testNumber, chars, 0x000000FF);
    chars[0x000000FE] = 0x00;
    printf("%s", chars);
    return 0;
}