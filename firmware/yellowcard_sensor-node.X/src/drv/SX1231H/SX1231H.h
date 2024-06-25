/*************************************************************************
 *  Yellowcard - Example firmware for the Yellowcard RF Development Kit  *
 * --------------------------------------------------------------------- *
 *  SX1231H.h - Driver for the SX1231H sub-1GHz radio                    *
 *************************************************************************/

#ifndef _SX1231H_H_
#define _SX1231H_H_

//Import any libraries used by this file
#include <xc.h>                //Include the primary header used by the XC32 compiler
#include "SX1231HRegisters.h"  //Include a list of handy register address definitons for the transceiver


//Define any enum types used in this file
typedef enum
{
    SLEEP, STBY, FS, TX, RX, MODE_RESERVED
} opModeSX1231H_t;

typedef enum
{
    FSK_NOSHAPING = 0x00, FSK_F_BT10 = 0x01, FSK_F_BT05 = 0x02, FSK_F_BT03 = 0x03,
    OOK_NOSHAPING = 0x08, OOK_F_BR = 0x09, OOK_F_2BR = 0x0A, MOD_RESERVED = 0x0B
} modSchemeSX1231H_t;


//Define any variables that are external to this file
extern const uint8_t sx1231hInit_PacketEngine[];  //Stores the default configuration to load into the transceiver to configure the packet engine


//Define prototypes for functions used in the SX1231 source file
extern void initializeSX1231H(modSchemeSX1231H_t modulation);  //Initialize Transceiver Function, configures the transceiver IC to operate as required for the application

extern void writePacketSX1231H(const uint8_t *payloadBytes,  //Load Packet Function, writes the desired packet to the FIFO buffer on the transceiver IC
                               uint32_t payloadLength);

extern void setCarrierFreqSX1231H(uint32_t freqRF);         //Set Carrier Frequency Function, sets the RF transceiver to tune to the desired carrier frequency
extern void setFreqDeviationSX1231H(uint32_t freqDev);      //Set Frequency Deviation Function, sets the FSK (de)modulator frequency deviation
extern void setBitRateSX1231H(uint32_t bitRate);            //Set Bit-Rate Function, sets the data (de)modulator to operate at the desired bit-rate
extern void setDeviceModeSX1231H(opModeSX1231H_t newMode);  //Set Device Mode Function, instructs the RF transceiver to enter the desired mode
extern void setPowerLevelSX1231H(uint32_t txPower);         //Set Power Level Function, sets the TX output power strength

extern opModeSX1231H_t getDeviceModeSX1231H();  //Get Device Mode Function, returns the current mode that the transceiver is operating in

extern void interactWithRegistersSX1231H(uint32_t startAddress,  //Interact With Registers Functions, reads/writes to the registers in the transceiver at the given start address using/into dataBytes
                                         uint8_t *dataBytes,
                                         uint32_t bufferLength,
                                         uint32_t readMode);


#endif






//END OF FILE