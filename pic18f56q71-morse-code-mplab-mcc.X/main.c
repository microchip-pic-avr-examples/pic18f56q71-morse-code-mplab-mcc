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

#include "morseCommon.h"
#include "morseTx.h"
#include "morseRx.h"

//#define USER_INPUT_DECODE
//#define PRINT_ON_STARTUP

int main(void)
{
    SYSTEM_Initialize();
    
    //Configure callback to run morse code state machine
    Timer2_OverflowCallbackRegister(&morseTx_stateMachine);
   
    //Setup Callback for CLC3 - Rising Edge (Start of Morse Code)
    CLC3_CLCI_SetInterruptHandler(&morseRx_CLC3_onRisingEdge);
    
    //Init the Morse Code Functions
    morseRx_init();
    morseTx_init();

    //Setup UTMR
    TU16A_InterruptHandlerSet(&morseRx_TU16A_Callback);
    TU16B_InterruptHandlerSet(&morseRx_TU16B_Callback);
    
    // Enable the Global High Interrupts 
    INTERRUPT_GlobalInterruptHighEnable(); 

    // Enable the Global Low Interrupts 
    INTERRUPT_GlobalInterruptLowEnable(); 
        
    //Start UTMRs
    TU16A_Start();
    TU16B_Start();
    
#ifdef USER_INPUT_DECODE
    SELECT_USER_DECODE();
#else
    SELECT_TX_DECODE();
#endif
    
    //Define PRINT_ON_STARTUP to print "Hello World" on boot
#ifdef PRINT_ON_STARTUP    
    morseTx_queueString("hello world");
    morseTx_startTransmit();
#endif
    
    while(1)
    {
        //Morse Decoder State Machine
        morseRx_stateMachine();
        
        if (morseTx_isSwitchRequested() && morseRx_isIdle() && morseTx_isIdle())
        {
            //Request to switch input sources
            SWITCH_DECODE_SOURCE();
            morseTx_clearSwitchRequest();
            
            if (IS_USER_INPUT_ACTIVE())
            {
                //User Input
                printf("User input is now active.\r\n");
            }
            else
            {
                //TX Input
                printf("Transmitter input is now active.\r\n");
            }
        }
    }    
}