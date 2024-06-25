/**********************************************************************************
 *  Yellowcard - Example firmware for the Yellowcard RF Development Kit           *
 * ------------------------------------------------------------------------------ *
 *  Main.h - Primary header, contains the program entrance and MCU configuration  *
 **********************************************************************************/

#ifndef _MAIN_H_
#define _MAIN_H_



/********************************
 *  MCU Configuration Settings  *
 ********************************/


//DEVCFG3 Register
#pragma config USERID = 0xFFFF         //Enter Hexadecimal value (Enter Hexadecimal value)
#pragma config PMDL1WAY = OFF          //Peripheral Module Disable Configuration (Allow multiple reconfigurations)
#pragma config IOL1WAY = OFF           //Peripheral Pin Select Configuration (Allow multiple reconfigurations)

//DEVCFG2 Register
#pragma config FPLLIDIV = DIV_2        //PLL Input Divider (2x Divider)
#pragma config FPLLMUL = MUL_16        //PLL Multiplier (16x Multiplier)
#pragma config FPLLODIV = DIV_4        //System PLL Output Clock Divider (PLL Divide by 4)

//DEVCFG1 Register
#pragma config FNOSC = FRCPLL          //Oscillator Selection Bits (Fast RC Osc with PLL)
#pragma config FSOSCEN = ON            //Secondary Oscillator Enable (Enabled)
#pragma config IESO = OFF              //Internal/External Switch Over (Disabled)
#pragma config POSCMOD = OFF           //Primary Oscillator Configuration (Primary osc disabled)
#pragma config OSCIOFNC = OFF          //CLKO Output Signal Active on the OSCO Pin (Disabled)
#pragma config FPBDIV = DIV_2          //Peripheral Clock Divisor (Pb_Clk is Sys_Clk/2)
#pragma config FCKSM = CSECMD          //Clock Switching and Monitor Selection (Clock Switch Enable, FSCM Disabled)
#pragma config WDTPS = PS1048576       //Watchdog Timer Postscaler (1:1048576)
#pragma config WINDIS = OFF            //Watchdog Timer Window Enable (Watchdog Timer is in Non-Window Mode)
#pragma config FWDTEN = OFF            //Watchdog Timer Enable (WDT Disabled (SWDTEN Bit Controls))
#pragma config FWDTWINSZ = WINSZ_25    //Watchdog Timer Window Size (Window Size is 25%)

//DEVCFG0 Register
#pragma config JTAGEN = OFF            //JTAG Enable (JTAG Disabled)
#pragma config ICESEL = ICS_PGx1       //ICE/ICD Comm Channel Select (Communicate on PGEC1/PGED1)
#pragma config PWP = OFF               //Program Flash Write Protect (Disable)
#pragma config BWP = OFF               //Boot Flash Write Protect bit (Protection Disabled)
#pragma config CP = OFF                //Code Protect (Protection Disabled)



/*******************
 *  Main Firmware  *
 *******************/


//Import any libraries used by this file
#include <xc.h>            //Include the primary header used by the XC32 compile
#include "Interrupts.h"    //Include the Interrupts header file, contains all interrupt related functions of the application
//#include "Application.h"  //Inlcude the Application header file, contains the actual application program and the program control state machine


//Application Configuration flash memory allocation
extern const uint8_t __attribute__ ((space(prog), section(".app_config"))) configNodeID;
extern const uint32_t __attribute__ ((space(prog), section(".app_config"))) configSampleInterval;


//Define any enum types used within this file
//#ifndef _ENUM_SYSCLKSPEED_T_
//#define _ENUM_SYSCLKSPEED_T_
//typedef enum
//{
//    SYSCLK_1MHZ = 0x13010703, SYSCLK_16MHZ = 0x13010103
//} SysClkSpeed_t;
//#endif


//Define any variables that are external to this file
//extern volatile NodeState_t currentState;       //Used to track where program execution is currently taking place within the program state machine
//extern const void (*handlerFunctionTable[])();  //Provides a lookup table of handler functions to allow for proper execution redirection after each state is processed


//Program Run-time Functions
extern inline void setupMCU();  //Setup MCU Function, sets the appropriate control registers on the MCU needed to make things work for the application
extern void main();             //Main Function, main entry point of the program of the MCU after reset and initialization
//extern void printSampleToUART(uint16_t adcValue);


#endif






//END OF FILE