/*************************************************************************
 *  Yellowcard - Example firmware for the Yellowcard RF Development Kit  *
 * --------------------------------------------------------------------- *
 *  Main.c - Defines the main functionality of the program               *
 *************************************************************************/

#include "Main.h"  //Include the Main header file that houses all global entities to this system



/********************************
 *  Application  Configuration  *
 ********************************/

const uint8_t  configNodeID = 0x01;                //Sets the device's address
const uint32_t configSampleInterval = 0x00010000;  //Sets the time between measurements



/******************
 *  Main Program  *
 ******************/


//Setup MCU Function, sets the appropriate control registers on the MCU needed to make things work for the application
inline void setupMCU()
{
    asm volatile ("di");  //Disable interrupts for the duration of the setup routine

    //Configure Port A
    LATA = 0x00000000;   //Clear all of Port A to logic LOW
    TRISA = 0x00000018;  //Set RA0 and RA1 to outputs

    //Configure Port B
    ANSELB = 0x0000E003;  //Force RB2 and RB3 to fully digital IO, allowing the I2C peripheral to function correctly
    LATB = 0x00001000;    //Clear all of Port B to logic LOW, setting RB12 to logic HIGH
    ODCB = 0x00001000;    //Enable the open-drain function on RB12
    CNENB = 0x000003C0;   //Enable change notification interrupts on inputs RB6 through RB9
    TRISB = 0x0000EBFF;   //Set RB10 and RB12 to outputs

    //Configure PPS connections
    RPA3R = 0x00000002;   //Assign RA3 to the TX output of UART2
    U2RXR = 0x00000001;   //Assign RB5 to the RX input of UART2
    INT4R = 0x00000004;   //Assign RB7 to the 4th external interrupt
    RPB13R = 0x00000003;  //Assign RB13 to the SDO output of SPI1
    SDI1R = 0x00000003;   //Assign RB11 to the SDI input of SPI1
    
    //Enable and configure the first SPI peripheral, SPI1
    SPI1BRG = 0x00000001;   //Set the clock frequency of SPI1 to operate at 4MHz
    SPI1CON2 = 0x00000C00;  //Disable all audio codec functionality of SPI1
    SPI1CON = 0x00008120;   //Enable SPI1 in master mode, configured for 8-bit words in standard buffer mode

    //Enable and configure the second I2C peripheral, I2C2
    I2C2BRG = 0x00000008;  //Set the bus speed of I2C2 to ~429kHz
    I2C2CON = 0x0000A000;  //Enable I2C2 with the stop in idle mode active

    //Enable and configure the second UART peripheral, UART2
    U2BRG = 0x00000019;   //Set the baud rate of UART2 to operate at roughly 19200bps
    U2STA = 0x00008400;   //Enable the transmitter with the interrupt asserted on an empty TX buffer
    U2MODE = 0x00008000;  //Enable UART2 in standard baud mode

    //Prepare for configuraiton of the RTCC peripheral
    SYSKEY = 0x00000000;  //Reset the register lock state machine by writing a 0 to it
    SYSKEY = 0xAA996655;  //Write the first unlock key to the SYSKEY register
    SYSKEY = 0x556699AA;  //Write the second unlock key to the register to finally unlock all protected registers
    RTCCON = 0x00000008;  //Set Bit-3 of RTCCON to enable writes to all RTCC related registers
    SYSKEY = 0x00000000;  //Lock the protected registers now that we're done writing to protected registers

    //Configure the RTCC
    ALRMTIME = configSampleInterval;  //Set the alarm time of the RTCC to the application configured sample interval
    ALRMDATE = 0x00000000;            //Set the desired RTCC alarm date
    RTCCON = 0x00002208;              //Configure the RTCC to use SOSC as the source clock with stop-in-idle mode active

    //Configure Timer 1
//    PR1 = 0x00001FFF;
//    T1CON = 0x00000016;  //Enable Timer 1 using the secondary oscillator as the clock source with a 256 cycle pre-scaler
//    T1CON = 0x00000002;
//    TMR1 = 0x00000000;   //Clear the contents of the Timer 1 count register

    //Enable the DMA peripheral
    DMACON = 0x00008000;

    //Configure DMA 1 for SPI TX transfers

    //Configure DMA 2 for UART TX transfers
    DCH2DSIZ = 0x00000001;          //Destination pointer has a size of 1 byte
    DCH2CSIZ = 0x00000001;          //Use a cell size of 1, ensuring only 1 byte is transfered at a time
    DCH2DSA = KVA_TO_PA(&U2TXREG);  //Assign the destination address of DMA2 to the transmit buffer of UART2
    DCH2DAT = 0x00000000;           //Use a NULL character as the termination byte for the transfer sequence
    DCH2CON = 0x00000000;           //Setup DMA2 for pattern matching with the lowest DMA priority
    DCH2ECON = 0x00003730;          //Using pattern match mode, allow start events from IRQ 55 (UART2 TX interrupt)
    DCH2INT = 0x00080000;           //Enable block transfer complete interrupts for DMA 2

    setupInterrupts();               //Setup the interrupt controller of the MCU and enable interrupts
}

//Main Function, called upon reset of the MCU
void main()
{
    setupMCU();  //Configure the main functionality of the microcontroller for the application

    uint32_t counter = 0x000FFFFF;  //Create a counter variable to use for the various reset tasks
    while (counter--);              //Wait a short period of time before continuing

    //Initialize any hardware connected to the microcontroller for the application
    initializeSX1231H(OOK_F_2BR);
    setCarrierFreqSX1231H(432950000);
    setFreqDeviationSX1231H(600);
    setBitRateSX1231H(2400);
    setPowerLevelSX1231H(0x00000016);
    setDeviceModeSX1231H(SLEEP);
    
    counter = 0x000000FF;  //Allow a maximum of 255 attempts when trying to read the calibration data from the pressure sensor
    while (counter--)
    {
        initializeDPS368(OVERSAMPLE_32, BACKGROUND_1HZ, OVERSAMPLE_8, BACKGROUND_1HZ, 0x00000000);  //Send the desired operating configuration to the DPS368 pressure sensor
        if (readCalCoeffsDPS368()) break;                                                           //Attempt to load the calibration data from the sensor, exiting the loop when successful
    }

    //Infinite loop of death :3
    while (0xFFFFFFFF)
    {
        handlerFunctionTable[currentState]();  //Invoke the function located at the address obtained from the lookup table
    }
}






//END OF FILE