/*************************************************************************
 *  Yellowcard - Example firmware for the Yellowcard RF Development Kit  *
 * --------------------------------------------------------------------- *
 *  Interrupts.c - Contains all application interrupt functions          *
 *************************************************************************/

#include "Interrupts.h"



/********************
 *  Initialization  *
 ********************/


//Setup Interrupts Function, configures the interrupt controller to receive and create program interrupts for the application
inline void setupInterrupts()
{
    asm volatile ("di");  //Disable global interrupts
    INTCON = 0x00001000;  //Enable multi-vector interrupt modes

    IFS0 = 0x00000000;  //Clear the entire IFS0 register to clear all interrupt flags
    IFS1 = 0x00000000;  //Clear the entire IFS1 register to clear all interrupt flags

    IPC1 = 0x00000008;   //Set the Timer 1 interrupt priority level to 2
    IPC4 = 0x04000000;   //Set the External Interrupt 4 priority level to 1
    IPC6 = 0x00000800;   //Set the RTCC interrupt priority level to 2
    IPC8 = 0x00040000;   //Set the Change Notice interrupt priority level to 1
    IPC10 = 0x00080000;  //Set the DMA 2 interrupt priority level to 2

    IEC0 = 0x40800010;  //Enable the Timer 1 period match, RTCC, and fourth external interrupts
    IEC1 = 0x40000000;
//    IEC1 = 0x40004000;  //Enable the DMA 2 abort/complete interrupt and Port B change notification interrupts

    asm volatile ("ei");  //Enable global interrupts again
}



/********************************
 *  Interrupt Service Routines  *
 ********************************/


//Priority 3


//Priority 2

//Timer 1 Period Match Interrupt Handler Function, called when the TMR1 register matches PR1
void __ISR(_TIMER_1_VECTOR, IPL2SOFT) timer1PeriodMatchISR()
{
    IFS0CLR = 0x00000010;  //Clear the Timer 1 interrupt flag

    T1CONCLR = 0x00008000;  //Stop Timer 1 by clearing Bit-15 of the T1CON register

    //Do something here
}

//RTCC Alarm Interrupt Handler Function, called whenever an alarm goes off within the RTCC
void __ISR(_RTCC_VECTOR, IPL2SOFT) rtccAlarmISR()
{
    IFS0CLR = 0x40000000;  //Clear the RTCC interrupt flag

    //Do something here
}

//DMA Channel 2 Interrupt Handler Function, called when DMA2 aborts or finishes transferring a block of data
void __ISR(_DMA_2_VECTOR, IPL2SOFT) dma2ISR()
{
    IFS1CLR = 0x40000000;  //Clear the DMA 2 interrupt flag

    DCH2INT = 0x00080000;           //Clear the interrupts flags for DMA 2 itself
    while (!(U2STA & 0x00000100));  //Wait until the transmission has completed fully before leaving
}


//Priority 1 (Lowest)

//External Interrupt 4 Handler Function, called on the falling edge of INT4
void __ISR(_EXTERNAL_4_VECTOR, IPL1SOFT) int4ISR()
{
    IFS0CLR = 0x00800000;  //Clear the INT4 interrupt flag

    //Do something here
}

//Port Change Notice Interrupt Handler Function, called when any of the 3 buttons or reed switch changes state
void __ISR(_CHANGE_NOTICE_VECTOR, IPL1SOFT) portChangeNoticeISR()
{
    uint32_t stataBuffer = CNSTATB;  //Create a temp copy of the state of CNSTATA so that PORTA calls aren't changing CNSTATA for the next if statement
    IFS1CLR = 0x00002000;            //Clear Port Change Notice interrupt flag

    //Do something here
}