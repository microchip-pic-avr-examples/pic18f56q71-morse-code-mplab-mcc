/**
 * TU16B Generated Driver File
 *
 * @file tu16b.c
 *  
 * @ingroup tu16b
 *
 * @brief This file contains the API definitions for the TU16B module.
 *
 * @version TU16B Driver Version 2.0.1
 */

 /*
© [2023] Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip 
    software and any derivatives exclusively with Microchip products. 
    You are responsible for complying with 3rd party license terms  
    applicable to your use of 3rd party software (including open source  
    software) that may accompany Microchip software. SOFTWARE IS ?AS IS.? 
    NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS 
    SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT,  
    MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT 
    WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY 
    KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF 
    MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE 
    FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP?S 
    TOTAL LIABILITY ON ALL CLAIMS RELATED TO THE SOFTWARE WILL NOT 
    EXCEED AMOUNT OF FEES, IF ANY, YOU PAID DIRECTLY TO MICROCHIP FOR 
    THIS SOFTWARE.
*/

#include <xc.h>
#include "../tu16b.h"

void (*TU16B_InterruptHandler) (void);

const struct TMR_INTERFACE TU16B = {
    .Initialize = TU16B_Initialize,
    .Start = TU16B_Start,
    .Stop = TU16B_Stop,
    .PeriodCountSet = TU16B_Write,
    .TimeoutCallbackRegister = NULL,
    .Tasks = NULL
};

void TU16B_Initialize(void)
{
    //Stop Timer
    TU16BCON0bits.ON = 0;

    //CIF disabled; ZIF disabled; PRIF disabled; CAPT disabled; LIMIT disabled; CLR disabled; OSEN enabled;  
    TU16BCON1 = 0x40;
    //STOP Rising ERS Edge; RESET At PR Match; START No hardware Start (Always On); CSYNC enabled; EPOL non inverted;  
    TU16BHLT = 0x4E;
    //CLK CLKREF_OUT;   
    TU16BCLK = 0x9;
    //ERS CLC3;  
    TU16BERS = 0x10;
    //PS 0;  
    TU16BPS = 0x0;
    //PRH 3; 
    TU16BPRH = 0x3;
    //PRL 232; 
    TU16BPRL = 0xE8;
    //TMRH 3; 
    TU16BTMRH = 0x3;
    //TMRL 232; 
    TU16BTMRL = 0xE8;

    // Clearing IF flag before enabling the interrupt.
    TU16BCON1bits.PRIF = 0;
    TU16BCON1bits.ZIF = 0;
    TU16BCON1bits.CIF = 0;
    // Set Default Interrupt Handler.
    TU16B_InterruptHandlerSet(TU16B_DefaultInterruptHandler);
    //Enable TUI interrupt
    PIE10bits.TU16BIE = 1;

    //CIE enabled; ZIE disabled; PRIE enabled; RDSEL read; OPOL low; OM pulse mode; CPOL rising edge; ON disabled;  
    TU16BCON0 = 0x45;
}

inline void TU16B_Start(void)
{
    TU16BCON0bits.ON = 1;
}

inline void TU16B_Stop(void)
{
    TU16BCON0bits.ON = 0;
}

uint16_t TU16B_CaptureValueRead(void)
{
    TU16BCON0bits.RDSEL = 0;
    return (uint16_t)(((uint16_t)TU16BCRHbits.CRH<< 8) | TU16BCRLbits.CRL);
}

uint16_t TU16B_OnCommandCapture(void)
{
    TU16BCON1bits.CAPT = 1;
    while(TU16BCON1bits.CAPT == 1);
    return TU16B_CaptureValueRead();
}

uint16_t TU16B_Read(void)
{
    bool onVal = TU16BCON0bits.ON;
    TU16BCON0bits.ON = 0;
    TU16BCON0bits.RDSEL = 1;
    uint16_t tmrVal = (uint16_t)(((uint16_t)TU16BTMRHbits.TMRH << 8) | TU16BTMRLbits.TMRL);
    TU16BCON0bits.ON = onVal;
    return tmrVal;
}

void TU16B_Write(size_t timerVal)
{
    timerVal = (uint16_t) timerVal;
    bool onVal = TU16BCON0bits.ON;
    TU16BCON0bits.ON = 0;
    TU16BTMRHbits.TMRH = (uint8_t) (timerVal >> 8);
    TU16BTMRLbits.TMRL = (uint8_t) (timerVal & 0xFF);
    TU16BCON0bits.ON = onVal;
}

inline void TU16B_CounterClear(void)
{
    TU16BCON1bits.CLR = 1;
    while(TU16BCON1bits.CLR == 1);
}

void TU16B_PeriodValueSet(uint16_t prVal)
{
    TU16BPRHbits.PRH = (uint8_t)((prVal >> 8) & 0xFF);
    TU16BPRLbits.PRL = (uint8_t)(prVal & 0xFF);
}

inline void TU16B_PRMatchInterruptEnable(void)
{
    TU16BCON0bits.PRIE = 1;
}

inline void TU16B_PRMatchInterruptDisable(void)
{
    TU16BCON0bits.PRIE = 0;
}

inline void TU16B_ZeroInterruptEnable(void)
{
    TU16BCON0bits.ZIE = 1;
}

inline void TU16B_ZeroInterruptDisable(void)
{
    TU16BCON0bits.ZIE = 0;
}

inline void TU16B_CaptureInterruptEnable(void)
{
    TU16BCON0bits.CIE = 1;
}

inline void TU16B_CaptureInterruptDisable(void)
{
    TU16BCON0bits.CIE = 0;
}

inline bool TU16B_HasPRMatchOccured(void)
{
    return TU16BCON1bits.PRIF;
}

inline bool TU16B_HasResetOccured(void)
{
    return TU16BCON1bits.ZIF;
}

inline bool TU16B_HasCaptureOccured(void)
{
    return TU16BCON1bits.CIF;
}

inline bool TU16B_IsTimerRunning(void)
{
    return TU16BCON1bits.RUN;
}

inline void TU16B_InterruptEnable(void)
{
    PIE10bits.TU16BIE = 1;
}

inline void TU16B_InterruptDisable(void)
{
    PIE10bits.TU16BIE = 0;
}

inline bool TU16B_IsInterruptEnabled(void)
{
    return PIE10bits.TU16BIE;
}

inline void TU16B_InterruptFlagsClear(void)
{
    TU16BCON1bits.PRIF = 0;
    TU16BCON1bits.ZIF = 0;
    TU16BCON1bits.CIF = 0;
}

void __interrupt(irq(TU16B),base(8)) TU16B_ISR(void)
{
    if(TU16B_InterruptHandler)
    {
        TU16B_InterruptHandler();
    }
    
    if(TU16BCON1bits.PRIF == 1)
    {
        TU16BCON1bits.PRIF = 0;
    }
    if(TU16BCON1bits.ZIF == 1)
    {
        TU16BCON1bits.ZIF = 0;
    }
    if(TU16BCON1bits.CIF == 1)
    {
        TU16BCON1bits.CIF = 0;
    }
    // add your TU16B interrupt custom code
}

void TU16B_InterruptHandlerSet(void (* InterruptHandler)(void)){
    TU16B_InterruptHandler = InterruptHandler;
}

void TU16B_DefaultInterruptHandler(void){
    // add your TU16B interrupt custom code
    // or set custom function using TU16B_InterruptHandlerSet()
}
