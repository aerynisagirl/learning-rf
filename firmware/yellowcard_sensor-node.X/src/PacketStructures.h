/***************************************************************************************
 *  Yellowcard - Example firmware for the Yellowcard RF Development Kit                *
 * ----------------------------------------------------------------------------------- *
 *  PacketStructures.h - Provides translation methods and data types for data packets  *
 ***************************************************************************************/

#ifndef _PACKET_STRUCTURES_H_
#define	_PACKET_STRUCTURES_H_

//Import any libraries used by this file
#include <xc.h>  //Include the main header file for the XC32 compiler, provides register definitions


//Define any constants related to packet lengths
#define PACKET_LENGTH_EVENT            0x00000007
#define PACKET_LENGTH_MEASUREREPORT    0x0000000B


//Define any enums used within this file
typedef enum
{
    ACKNOWLEDGE = 0x00, EVENT = 0x01, MEASURE_REPORT = 0x02
} packetPayloadType_t;

typedef enum
{
    RESET = 0x00
} packetEventType_t;


//Define any structs used within this file
typedef struct
{
    uint8_t length;
    uint8_t sourceAddress;
    uint8_t payloadType;
    uint8_t frameNumberMSB;
    uint8_t frameNumberLSB;
} packetHeader_t;


//Define any unions used within this file
typedef union
{
    struct
    {
        packetHeader_t packetHeader;

        uint8_t eventType;
        uint8_t auxArgument;
    };
    struct
    {
        uint8_t bytes[PACKET_LENGTH_EVENT];
    };
} packetEvent_t;

typedef union
{
    struct
    {
        packetHeader_t packetHeader;

        uint8_t reportedTempMSB;
        uint8_t reportedTempLSB;
        uint8_t reportedRH;
        uint8_t reportedPresHSB;
        uint8_t reportedPresMSB;
        uint8_t reportedPresLSB;
    };
    struct
    {
        uint8_t bytes[PACKET_LENGTH_MEASUREREPORT];
    };
} packetMeasureReport_t;


//Define any variables that are external to this file
extern volatile uint16_t globalFrameCount;  //Used to determine what the frame number of the next packet will be
extern const uint8_t configNodeID;          //The Node ID set within the application configuration region of flash memory


//Define prototypes for functions used in the Packet Structures source file
extern void generateHeader(packetHeader_t *header,                       //Generate Header Function, creates a new packet header to use for constructing a full packet
                           packetPayloadType_t packetType,
                           uint8_t packetLength);
extern void newEventPacket(packetEvent_t *packetBuffer,                  //New Event Packet Function, generates a new event packet at the provided address
                           packetEventType_t eventType,
                           uint8_t argument);
extern void newMeasureReportPacket(packetMeasureReport_t *packetBuffer,  //New Measure Report Packet Function, generates a new measurement report packet at the provided address
                                   const float *temperature,
                                   const float *humidity,
                                   const float *pressure);


#endif






//END OF FILE