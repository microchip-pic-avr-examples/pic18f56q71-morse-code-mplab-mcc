/**
 * TU16A Generated Driver File
 *
 * @file tu16a.c
 *  
 * @ingroup tu16a
 *
 * @brief This file contains the API definitions for the TU16A module.
 *
 * @version TU16A Driver Version 2.1.0
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
#include "../tu16a.h"

static void (*TU16A_InterruptHandler) (void);
static void (*TU16A_PRMatchInterruptHandler) (void);
static void (*TU16A_ZeroMatchInterruptHandler) (void);
static void (*TU16A_CaptureMatchInterruptHandler) (void);

const struct TMR_INTERFACE TU16A = {
    .Initialize = TU16A_Initialize,
    .Start = TU16A_Start,
    .Stop = TU16A_Stop,
    .PeriodCountSet = TU16A_Write,
    .TimeoutCallbackRegister = NULL,
    .Tasks = NULL
};

void TU16A_Initialize(void)
{
    //Stop Timer
    TU16ACON0bits.ON = 0;

    //CIF disabled; ZIF disabled; PRIF disabled; CAPT disabled; LIMIT enabled; CLR disabled; OSEN disabled;  
    TU16ACON1 = 0x10;
    //STOP Either ERS Edge; RESET At Start and PR match; START Rising ERS edge; CSYNC enabled; EPOL non inverted;  
    TU16AHLT = 0x69;
    //CLK CLKREF_OUT;   
    TU16ACLK = 0x9;
    //ERS CLC3;  
    TU16AERS = 0x10;
    //PS 0;  
    TU16APS = 0x0;
    //PRH 255; 
    TU16APRH = 0xFF;
    //PRL 255; 
    TU16APRL = 0xFF;
    //TMRH 0; 
    TU16ATMRH = 0x0;
    //TMRL 4; 
    TU16ATMRL = 0x4;

    // Clearing IF flag before enabling the interrupt.
    TU16ACON1bits.PRIF = 0;
    TU16ACON1bits.ZIF = 0;
    TU16ACON1bits.CIF = 0;
    // Set Default Interrupt Handler.
    TU16A_PRMatchInterruptHandlerSet(TU16A_PRMatchDefaultInterruptHandler);
    TU16A_CaptureMatchInterruptHandlerSet(TU16A_CaptureMatchDefaultInterruptHandler);
    TU16A_InterruptHandlerSet(TU16A_DefaultInterruptHandler);
    //Enable TUI interrupt
    PIE5bits.TU16AIE = 1;

    //CIE enabled; ZIE disabled; PRIE enabled; RDSEL read; OPOL low; OM pulse mode; CPOL rising edge; ON disabled;  
    TU16ACON0 = 0x45;
}

void TU16A_Start(void)
{
    TU16ACON0bits.ON = 1;
}

void TU16A_Stop(void)
{
    TU16ACON0bits.ON = 0;
}

uint16_t TU16A_CaptureValueRead(void)
{
    TU16ACON0bits.RDSEL = 0;
    return (uint16_t)(((uint16_t)TU16ACRHbits.CRH<< 8) | TU16ACRLbits.CRL);
}

uint16_t TU16A_OnCommandCapture(void)
{
    TU16ACON1bits.CAPT = 1;
    while(TU16ACON1bits.CAPT == 1);
    return TU16A_CaptureValueRead();
}

uint16_t TU16A_Read(void)
{
    bool onVal = TU16ACON0bits.ON;
    TU16ACON0bits.ON = 0;
    TU16ACON0bits.RDSEL = 1;
    uint16_t tmrVal = (uint16_t)(((uint16_t)TU16ATMRHbits.TMRH << 8) | TU16ATMRLbits.TMRL);
    TU16ACON0bits.ON = onVal;
    return tmrVal;
}

void TU16A_Write(size_t timerVal)
{
    uint16_t timerValGet = (uint16_t) timerVal;
    bool onVal = TU16ACON0bits.ON;
    TU16ACON0bits.ON = 0;
    TU16ATMRHbits.TMRH = (uint8_t) (timerValGet >> 8);
    TU16ATMRLbits.TMRL = (uint8_t) (timerValGet & 0xFF);
    TU16ACON0bits.ON = onVal;
}

void TU16A_CounterClear(void)
{
    TU16ACON1bits.CLR = 1;
    while(TU16ACON1bits.CLR == 1);
}

void TU16A_PeriodValueSet(uint16_t prVal)
{
    TU16APRHbits.PRH = (uint8_t)((prVal >> 8) & 0xFF);
    TU16APRLbits.PRL = (uint8_t)(prVal & 0xFF);
}

void TU16A_PRMatchInterruptEnable(void)
{
    TU16ACON0bits.PRIE = 1;
}

void TU16A_PRMatchInterruptDisable(void)
{
    TU16ACON0bits.PRIE = 0;
}

void TU16A_ZeroInterruptEnable(void)
{
    TU16ACON0bits.ZIE = 1;
}

void TU16A_ZeroInterruptDisable(void)
{
    TU16ACON0bits.ZIE = 0;
}

void TU16A_CaptureInterruptEnable(void)
{
    TU16ACON0bits.CIE = 1;
}

void TU16A_CaptureInterruptDisable(void)
{
    TU16ACON0bits.CIE = 0;
}

bool TU16A_HasPRMatchOccured(void)
{
    return TU16ACON1bits.PRIF;
}

bool TU16A_HasResetOccured(void)
{
    return TU16ACON1bits.ZIF;
}

bool TU16A_HasCaptureOccured(void)
{
    return TU16ACON1bits.CIF;
}

bool TU16A_IsTimerRunning(void)
{
    return TU16ACON1bits.RUN;
}

void TU16A_InterruptEnable(void)
{
    PIE5bits.TU16AIE = 1;
}

void TU16A_InterruptDisable(void)
{
    PIE5bits.TU16AIE = 0;
}

bool TU16A_IsInterruptEnabled(void)
{
    return PIE5bits.TU16AIE;
}

void TU16A_InterruptFlagsClear(void)
{
    TU16ACON1bits.PRIF = 0;
    TU16ACON1bits.ZIF = 0;
    TU16ACON1bits.CIF = 0;
}

void __interrupt(irq(TU16A),base(8)) TU16A_ISR(void)
{
    if(TU16A_InterruptHandler)
    {
        TU16A_InterruptHandler();
    }
    if(TU16ACON1bits.PRIF == 1U)
    {
        TU16ACON1bits.PRIF = 0;
        if(TU16A_PRMatchInterruptHandler)
        {
            TU16A_PRMatchInterruptHandler();
        }
    }
    if(TU16ACON1bits.CIF == 1U)
    {
        TU16ACON1bits.CIF = 0;
        if(TU16A_CaptureMatchInterruptHandler)
        {
            TU16A_CaptureMatchInterruptHandler();
        }
    }
    // add your TU16A interrupt custom code
}

void TU16A_PRMatchInterruptHandlerSet(void (* InterruptHandler)(void)){
    TU16A_PRMatchInterruptHandler = InterruptHandler;
}

void TU16A_PRMatchDefaultInterruptHandler(void){
    // add your TU16A interrupt custom code
    // or set custom function using TU16A_PRMatchInterruptHandlerSet()
}

void TU16A_ZeroMatchInterruptHandlerSet(void (* InterruptHandler)(void)){
    TU16A_ZeroMatchInterruptHandler = InterruptHandler;
}

void TU16A_ZeroMatchDefaultInterruptHandler(void){
    // add your TU16A interrupt custom code
    // or set custom function using TU16A_ZeroMatchInterruptHandlerSet()
}

void TU16A_CaptureMatchInterruptHandlerSet(void (* InterruptHandler)(void)){
    TU16A_CaptureMatchInterruptHandler = InterruptHandler;
}

void TU16A_CaptureMatchDefaultInterruptHandler(void){
    // add your TU16A interrupt custom code
    // or set custom function using TU16A_CaptureMatchInterruptHandlerSet()
}

void TU16A_InterruptHandlerSet(void (* InterruptHandler)(void)){
    TU16A_InterruptHandler = InterruptHandler;
}

void TU16A_DefaultInterruptHandler(void){
    // add your TU16A interrupt custom code
    // or set custom function using TU16A_DefaultInterruptHandlerSet()
}

