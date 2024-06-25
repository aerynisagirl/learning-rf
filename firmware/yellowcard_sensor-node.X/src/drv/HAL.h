/*******************************************************************************************
 *  Yellowcard - Example firmware for the Yellowcard RF Development Kit                    *
 * --------------------------------------------------------------------------------------- *
 *  HAL.h - Provides a layer of abstraction between device drivers and theMCU peripherals  *
 *******************************************************************************************/

#ifndef _HAL_H_
#define	_HAL_H_

//Import any libraries used by this file
#include <xc.h>        //Include the main header file for the XC32 compiler, provides register definitions
#include <sys/kmem.h>  //Include the kmem header, provides address translation macros between virtual and physical addresses


//Define any enums that are used within this file
typedef enum
{
    SYSCLK_1MHZ = 0x13090702, SYSCLK_16MHZ = 0x13090102
} SysClkSpeed_t;


//Define any variables that are external to this
extern volatile uint8_t dmaBufferTxUART[];  //Used as a buffer for UART transmissions that occur using the DMA


//System Oscillator Functions
extern void allowSleepMode(uint32_t enabled);               //Allow Sleep Mode Function, selects which low-power mode the CPU will enter on the WAIT instruction, zero forces IDLE mode
extern void changeClockSpeed(SysClkSpeed_t newClockSpeed);  //Change Clock Speed Function, changes the system clock speed and reconfigures peripherals so that they are unaffected

//I2C Functions
extern uint32_t writeToI2C(uint32_t address,         //Write To I2C Function, sends the provided array of bytes to the provided slave address using the I2C2 peripheral
                           const uint8_t *bytes,
                           uint32_t length);
extern uint32_t readFromI2C(uint32_t address,         //Read from I2C Function, reads the desired number of bytes from the slave device at the provided address using the I2C2 peripheral,
                            uint8_t *bytes,
                            uint32_t readLength,
                            uint32_t addressLength);

//UART Functions
extern void writeToUART(const uint8_t *bytes,  //Write to UART Function, sends the provided array of bytes out the serial port through UART2
                        uint32_t length);
extern void startTxUART(const uint8_t *bytes,     //Start Transmission UART Function, begins sending the provided string over UART
                        const uint32_t *length);


#endif






//END OF FILE