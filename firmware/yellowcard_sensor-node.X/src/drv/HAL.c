/***************************************************************************
 *  Yellowcard - Example firmware for the Yellowcard RF Development Kit    *
 * ----------------------------------------------------------------------- *
 *  HAL.c - Contains all MCU peripheral interaction functions              *
 ***************************************************************************/

#include "HAL.h"



/***************
 *  Variables  *
 ***************/


//DMA Buffer
volatile uint8_t dmaBufferTxUART[0x000000FF];  //Create a 256 byte array to use for storing the message to be transmitting out of UART



/***********************
 *  System Oscillator  *
 ***********************/


//Allow Sleep Mode Function, selects which low-power mode the CPU will enter on the WAIT instruction, zero forces IDLE mode
void allowSleepMode(uint32_t enabled)
{
    volatile uint32_t *oscconAddress = &OSCCONCLR;  //Declare a pointer that points at the address of the OSCCONCLR register
    if (enabled) oscconAddress += 0x00000001;      //Add an offset of 1 register to oscconAddress such that it points at OSCCONSET when sleep mode is requested
    
    asm volatile ("di");  //Disable interrupts before doing anything else
    SYSKEY = 0x00000000;  //Reset the register lock state machine by writing a 0 to it
    SYSKEY = 0xAA996655;  //Write the first unlock key to the SYSKEY register
    SYSKEY = 0x556699AA;  //Write the second unlock key to the register to finally unlock all protected registers

    *oscconAddress = 0x00000010;  //Write all 0's with Bit-4 set to either set or clear the SLPEN bit in OSCCON

    SYSKEY = 0x00000000;  //Lock the protected registers now that the desired WAIT condition has been selected
    asm volatile ("ei");  //Enable interrupts now that everything is ready to go
}

//Change Clock Speed Function, changes the system clock speed and reconfigures peripherals so that they are unaffected
void changeClockSpeed(SysClkSpeed_t newClockSpeed)
{
    DMACON = 0x00009000;  //Put the DMA peripheral into suspend mode to prevent weird behaviour while switching clocks
    asm volatile ("di");  //Disable interrupts before doing anything else

    SYSKEY = 0x00000000;  //Reset the register lock state machine by writing a 0 to it
    SYSKEY = 0xAA996655;  //Write the first unlock key to the SYSKEY register
    SYSKEY = 0x556699AA;  //Write the second unlock key to the register to finally unlock all protected registers

    //Configure the oscillator to move to the desired clock frequency
    OSCCON = (uint32_t) newClockSpeed;  //Write the appropriate words to OSCCON to select the new desired clock configuration
    OSCCONSET = 0x00000001;             //Start the switch-over process by setting the OSWEN bit
    while (OSCCON & 0x00000001);        //Wait until the system is operating on the new clock settings
//    while (!(OSCCON & 0x00000020));

    SYSKEY = 0x00000000;  //Lock the protected registers now that the switch has completed

    //Reconfigure peripherals as required to maintain proper operation
    switch (newClockSpeed)
    {
        case SYSCLK_1MHZ:
            I2C2BRG = 0x00000004;
            U2BRG = 0x0000000C;
            break;
        case SYSCLK_16MHZ:
            I2C2BRG = 0x00000008;  //Set the bus speed of I2C2 to ~429kHz
            U2BRG = 0x00000019;    //Set the baud rate of UART2 to operate at roughly 19200bps
            break;
    }

    asm volatile ("ei");  //Enable interrupts now that everything is ready to go
    DMACON = 0x00008000;  //Put the DMA back into normal operation
}



/*********
 *  I2C  *
 *********/


//Write To I2C Function, sends the provided array of bytes to the provided slave address using the I2C2 peripheral
uint32_t writeToI2C(uint32_t address, const uint8_t *bytes, uint32_t length)
{
    //Start condition generation
    I2C2CONSET = 0x00000001;       //Generate a start condition on the I2C bus, this signals that new data is about to be sent to all slaves on the line
    while (I2C2CON & 0x00000001);  //Wait until the start condition is done sending to conform to I2C timing specifications

    //Sending address and write flag
    I2C2TRN = (address << 0x00000001);             //Shift the value of the address over to the left by 1 bit, then write it to the transmit register
    while (I2C2STAT & 0x00004000);                 //Wait until the address and write flag have finished being sent, and the ACK flag has been read by the master.
    if (I2C2STAT & 0x00008000) return 0x00000000;  //Leave the function early in cases where no acknowledge is returned by the slave device

    //Transmit each byte to the I2C bus
    while (length--)
    {
        I2C2TRN = *bytes++;                            //Send the next byte in the array out to the I2C bus
        while (I2C2STAT & 0x00004000);                 //Wait until the byte has finished transmitting before checking for acknowledgement
        if (I2C2STAT & 0x00008000) return 0x00000000;  //Leave the function early, the acknowledge bit was not received from the slave device
    }

    //Stop condition generation
    I2C2CONSET = 0x00000004;       //Generate a stop condition on the I2C bus, this signals the end of the transmission to the slave device
    while (I2C2CON & 0x00000004);  //Wait until the stop condition is done sending

    return 0xFFFFFFFF;  //Return a non-zero value to indicate a successful transmission
}

//Read from I2C Function, reads the desired number of bytes from the slave device at the provided address using the I2C2 peripheral
uint32_t readFromI2C(uint32_t address, uint8_t *bytes, uint32_t readLength, uint32_t addressLength)
{
    //Determine whether or not an address shall be requested first prior to reading
    if (addressLength)
    {
        if (!writeToI2C(address, bytes, addressLength)) return 0x00000000;  //Inform the device of the desired register address to read from using the address passed through *bytes
    }

    //Start condition generation
    I2C2CONSET = 0x00000001;       //Generate a start condition on the I2C bus, this signals that new data is about to be sent to all slaves on the line
    while (I2C2CON & 0x00000001);  //Wait until the start condition is done sending to conform to I2C timing specifications

    //Sending address and read request flag
    I2C2TRN = (address << 0x00000001) | 0x00000001;  //Shift the address byte to the left by 1 bit, forcing setting Bit-0 to indicate a read request, finally writing it all to the transmit register
    while (I2C2STAT & 0x00004000);                   //Wait until the address and read request flag have finished being sent, and the ACK flag has been read by the master
    if (I2C2STAT & 0x00008000) return 0x00000000;    //Leave the function early in cases where no acknowledge is returned by the slave device

    uint8_t *ptrStop = bytes + readLength;  //Calculate the address of the bytes pointer to stop the read process at

    //Read the next byte from the slave and store it in the provided buffer
    while (bytes != ptrStop)
    {
        I2C2CONSET = 0x00000008;       //Start the receive process by setting the RCEN bit
        while (I2C2CON & 0x00000008);  //Wait until the receive process has completed

        *bytes++ = I2C2RCV;  //Write the byte received from the slave device into the next available byte of the provided buffer array

        if (bytes == ptrStop) I2C2CONSET = 0x00000020;  //Configure the I2C peripheral to send an NACK response next if we've just received the last byte

        I2C2CONSET = 0x00000010;       //Send an ACK response to the slave device on the I2C bus to confirm that the byte was received
        while (I2C2CON & 0x00000010);  //Wait until the ACK condition is done sending
    }

    I2C2CONCLR = 0x00000020;  //Clear the ACKDT bit of the I2C control register now that sending NACK responses is no longer required

    //Stop condition generation
    I2C2CONSET = 0x00000004;       //Generate a stop condition on the I2C bus, this signals the end of transmission to the slave device
    while (I2C2CON & 0x00000004);  //Wait until the stop condition is done sending

    return 0xFFFFFFFF;  //Return a non-zero value to indicate a successful reception
}



/**********
 *  UART  *
 **********/


//Write to UART Function, sends the provided array of bytes out the serial port through UART2
void writeToUART(const uint8_t *bytes, uint32_t length)
{
    while (length--)
    {
        while (U2STA & 0x00000200);  //Wait until there is room in the UART2 transmit FIFO buffer before writing the next byte
        U2TXREG = *bytes++;          //Put the next byte in the array into the transmit buffer of UART2
    }

    while (!(U2STA & 0x00000100));  //Wait until the transmission has completed fully before leaving
}

//Start Transmission UART Function, begins sending the provided string over UART
void startTxUART(const uint8_t *bytes, const uint32_t *length)
{
    DCH2SSA = KVA_TO_PA(bytes);  //Assign the source address of DMA2 to the physical address of the provided buffer
    DCH2SSIZ = *length;          //Set the length of the source location to the provided length value
    DCH2CON = 0x00000080;        //Enable channel 2 of the DMA peripheral
    DCH2ECON = 0x000037B0;       //Start the transfer process by forcing the first cell transfer on DMA2

//    while (DCH2CON & 0x00008000);  //Wait until the block transfer has fully completed
}






//END OF FILE