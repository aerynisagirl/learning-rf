/*******************************************************************************
 *  Yellowcard - Example firmware for the Yellowcard RF Development Kit        *
 * --------------------------------------------------------------------------- *
 *  Application.c - Contains all functions related to the application program  *
 *******************************************************************************/

#include "Application.h"



/***************
 *  Variables  *
 ***************/


//Measurement Results
float mostRecentTemp;  //Create a float to store the most recent temperature measurement
float mostRecentRH;    //Create a float to store the most recent relative humidity measurement
float mostRecentPres;  //Create a float to store the most recent barometric pressure measurement

//State Machine and Program Control
volatile NodeState_t currentState = RESET;  //Initialize a variable to keep track of where program execution is within the state machine, starting in the RESET state



/***********************************
 *  Handler Function Lookup Table  *
 ***********************************/

const void (*handlerFunctionTable[])() = {&onReset,
                                          &doMeasurements,
                                          &reportMeasurements,
                                          &onMeasureFail,
                                          &doSleepLowPower};



/***********************
 *  Handler Functions  *
 ***********************/


//On Reset Function, handles the startup of the application and sends a startup event packet
void onReset()
{
//    changeClockSpeed(SYSCLK_16MHZ);  //Boost the CPU clock for interacting with the radio
    LATBSET = 0x00000400;

    packetEvent_t packetBuffer;  //Allocate a new packetEvent_t structure in memory to store the generated packet for transmission
    uint32_t logSize;            //Create a new variable to use for storing the size of the constructed log string

    newEventPacket(&packetBuffer, RESET, 0x00);  //Generate a new event packet that signifies a system reset event

    logSize = constructPacketLog((uint8_t *) dmaBufferTxUART, packetBuffer.bytes);  //Construct a new packet log and store it in dmaBufferTxUART
    startTxUART((uint8_t *) dmaBufferTxUART, &logSize);                             //Start the transmission of the log message

    writePacketSX1231H(packetBuffer.bytes, PACKET_LENGTH_EVENT);  //Transmit the packet over the air

    LATBCLR = 0x00000400;

    logSize = 0x003FFFFF;  //Re-use the logSize variable as a counter to create a delay
    while (logSize--);     //Wait for the logSize variable to become 0 before going continuing

    currentState = DO_MEASUREMENTS;  //Next state is DO_MEASUREMENTS
}

//Do Measurements Function, performs the data collection routine required to obtain new sensor measurements
void doMeasurements()
{
    uint32_t resultBuffer[0x00000002] = {0x000000FF, 0x00000000};  //Create an array of 2 32-bit unsigned integers to use for caching the results obtained from the sensors

    requestMeasurementSHT4X(HIGH_PRECISION_NO_HEATER);  //Ask the SHT4x sensor to start a new temperature and humidity measurement
    setModeDPS368(CONT_BOTH);                           //Start background measurements of both pressure and temperature on the DPS368 sensor

    while (resultBuffer[0x00000000]--);  //Wait a little bit before polling for measurement results

    //Obtain and calculate the barometric pressure measurement
    while (getResultStatusDPS368() != BOTH_READY);                                              //Proceed only if both temperature and pressure readings are available from the DPS368
    setModeDPS368(IDLE);                                                                        //Put the DPS368 sensor back into IDLE mode to save power
    getResultsFromFifoDPS368(resultBuffer, 0x00000002);                                         //Read both the temperature and pressure data from the sensor into resultBuffer
    mostRecentPres = convertToPressureFromDPS368(*resultBuffer, *(resultBuffer + 0x00000001));  //Convert the raw sensor data into the compensated barometric pressure in Pascal

    //Obtain and calculate the temperature and relative humidity measurements
    while (!getResultsSHT4X((uint16_t *) resultBuffer, (uint16_t *) (resultBuffer + 0x00000001)));  //Poll the SHT4x for results, storing them in the resultBuffer array
    mostRecentTemp = convertToTempCFromSHT4X((uint16_t *) resultBuffer);                            //Convert the raw temperature data into it's compensated form in Celsius
    mostRecentRH = convertToRHFromSHT4X((uint16_t *) (resultBuffer + 0x00000001));                  //Convert the raw humidity data into it's compensated form as a percentage

    currentState = REPORT_MEASUREMENTS;  //Next state is REPORT_MEASUREMENTS
}

//Report Measurements Function, prepares the obtained measurements and then sends them over the air
void reportMeasurements()
{
//    changeClockSpeed(SYSCLK_16MHZ);  //Boost the CPU clock for both interacting with the radio later on and also to keep computation time short
    LATBSET = 0x00000400;

    packetMeasureReport_t packetBuffer;  //Allocate a new packetMeasureReport_t structure in memory to store the generated packet for transmission
    uint32_t logSize;                    //Create a new variable to use for storing the size of constructed log strings
    
    newMeasureReportPacket(&packetBuffer, &mostRecentTemp, &mostRecentRH, &mostRecentPres);  //Generate a new measurement report packet containing the most recent measurement data

    logSize = constructMeasurementLog((uint8_t *) dmaBufferTxUART, &mostRecentTemp, &mostRecentRH, &mostRecentPres);  //Construct a new measurement report log and store it in dmaBufferTxUART
    logSize += constructPacketLog((uint8_t *) dmaBufferTxUART + logSize - 0x00000001, packetBuffer.bytes);            //Construct a new packet log and append it to dmaBufferTxUART
    startTxUART((uint8_t *) dmaBufferTxUART, &logSize);                                                               //Start the transmission of the log message over UART

    writePacketSX1231H(packetBuffer.bytes, PACKET_LENGTH_MEASUREREPORT);
    while (DCH2CON & 0x00008000) asm volatile ("wait");  //Keep the CPU in idle mode until DMA 2 is done writing to UART 2 before down-clocking the CPU
    while (!(U2STA & 0x00000100));                       //Wait until the transmission has completed fully before we down-clock the CPU
    
    LATBCLR = 0x00000400;
//    changeClockSpeed(SYSCLK_1MHZ);

    //Reset the RTC before going entering sleep mode
    RTCDATE = 0x00000000;  //Reset the date value back to 0
    RTCTIME = 0x00000000;  //Reset the time value back to 0
    RTCALRM = 0x00008600;  //Setup a single alarm to occur at the pre-set interval

    currentState = ENTER_SLEEP;
}

//On Measure Fail Function, exception handling method for failed measurement attempts
void onMeasureFail()
{
    currentState = ENTER_SLEEP;
}

//Do Sleep Low Power Function, halts program execution with the MCU fully powered down until an interrupt occurs
void doSleepLowPower()
{
    //TODO:  Disable unwanted interrupt sources
    RTCCON = 0x0000A248;   //Enable the RTCC and start counting
    
    currentState = DO_MEASUREMENTS;  //The assumption is made that when we wake from sleep the next state will be DO_MEASUREMENTS

    allowSleepMode(0xFFFFFFFF);  //Enable sleep mode such that the next time to WAIT instruction is called, the MCU enters a deep sleep
    asm volatile ("wait");       //Go to sleep and power down the MCU entirely
    allowSleepMode(0x00000000);  //Disable sleep mode now that we've woken up

    RTCCON = 0x00002208;  //Stop and disable the RTCC now that we have woken up again
    //TODO:  Re-enable the previously disabled interrupt sources
}






//END OF FILE