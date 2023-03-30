 /*
 * MAIN Generated Driver File
 * 
 * @file main.c
 * 
 * @defgroup main MAIN
 * 
 * @brief This is the generated driver implementation file for the MAIN driver.
 *
 * @version MAIN Driver Version 1.0.0
*/

/*
� [2023] Microchip Technology Inc. and its subsidiaries.

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
#include "mcc_generated_files/system/system.h"
#include <stdint.h>
#include <stdbool.h>

#include "morseLookup.h"

#define USER_INPUT

int main(void)
{
    SYSTEM_Initialize();
    
    //Configure callback to run morse code state machine
    Timer2_OverflowCallbackRegister(&morseStateMachineTx);
   
    //Setup Callback for CLC3 - Rising Edge (Start of Morse Code)
    CLC3_CLCI_SetInterruptHandler(&morseCallback_onStart);
    
    //Init the Morse Code Functions
    morseInit();

    //Setup UTMR
    TU16A_InterruptHandlerSet(&morseCallback_TU16A);
    TU16B_InterruptHandlerSet(&morseCallback_TU16B);
    
    // Enable the Global High Interrupts 
    INTERRUPT_GlobalInterruptHighEnable(); 

    // Enable the Global Low Interrupts 
    INTERRUPT_GlobalInterruptLowEnable(); 
        
    TU16A_Start();
    TU16B_Start();
    
#ifdef USER_INPUT
    SELECT_USER_DECODE();
#else
    SELECT_TX_DECODE();
#endif
    
    //Define INIT_TEST to print "Hello World" on startup
#ifdef INIT_TEST    
    morseLoadString("hello world");
    morseStart();
#endif
    
    while(1)
    {
        morseStateMachineRx();
    }    
}