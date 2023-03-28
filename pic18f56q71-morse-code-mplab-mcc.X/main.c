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
#include "mcc_generated_files/system/system.h"
#include <stdint.h>
#include <stdbool.h>

#include "morseLookup.h"


typedef enum 
{
    NO_EVENT_UTMR = 0, CAPTURE_UTMR, PR_MATCH_UTMR, RESET_UTMR
} StateUTMR;

static volatile StateUTMR printUTMRA = NO_EVENT_UTMR;
static volatile StateUTMR printUTMRB = NO_EVENT_UTMR;

void UTMRA_callback(void)
{
    if (TU16A_HasCaptureOccured())
    {
        printUTMRA = CAPTURE_UTMR;
    }
    else if (TU16A_HasPRMatchOccured())
    {
        printUTMRA = PR_MATCH_UTMR;
    }
    else if (TU16A_HasResetOccured())
    {
        printUTMRA = RESET_UTMR;
    }
}

void UTMRB_callback(void)
{
    if (TU16B_HasCaptureOccured())
    {
        printUTMRB = CAPTURE_UTMR;
    }
    else if (TU16B_HasPRMatchOccured())
    {
        printUTMRB = PR_MATCH_UTMR;
    }
    else if (TU16B_HasResetOccured())
    {
        printUTMRB = RESET_UTMR;
    }
}

int main(void)
{
    SYSTEM_Initialize();
    
    //Configure callback to run morse code state machine
    Timer2_OverflowCallbackRegister(&morseStateMachine);
   
    //Init the Morse Code Functions
    morseInit();

    //Setup UTMR
    TU16A_InterruptHandlerSet(&UTMRA_callback);
    TU16B_InterruptHandlerSet(&UTMRB_callback);
    
    LED0_SetDigitalOutput();
    
    // Enable the Global High Interrupts 
    INTERRUPT_GlobalInterruptHighEnable(); 

    // Enable the Global Low Interrupts 
    INTERRUPT_GlobalInterruptLowEnable(); 
        
//    TU16A_Start();
//    TU16B_Start();
    
    //Define INIT_TEST to print "Hello World" on startup
#ifdef INIT_TEST    
    morseLoadString("hello world");
    morseStart();
#endif
    
    while(1)
    {
        if (printUTMRA != NO_EVENT_UTMR)
        {
            printf("UTMR A - ");
            switch(printUTMRA)
            {
                case CAPTURE_UTMR:
                {
                    printf("Capture Event\r\nCapture = %d\r\n", TU16A_CaptureValueRead());
                    break;
                }
                case PR_MATCH_UTMR:
                {
                    printf("PR Match Event\r\nCapture = %d\r\n", TU16A_CaptureValueRead());
                    break;
                }
                case RESET_UTMR:
                {
                    printf("Reset Event\r\nCapture = %d\r\n", TU16A_CaptureValueRead());
                    break;
                }

            }
            printUTMRA = NO_EVENT_UTMR;
        }
        if (printUTMRB != NO_EVENT_UTMR)
        {
            printf("UTMR B - ");
            switch(printUTMRB)
            {
                case CAPTURE_UTMR:
                {
                    printf("Capture Event\r\nCapture = %d\r\n", TU16B_CaptureValueRead());
                    break;
                }
                case PR_MATCH_UTMR:
                {
                    printf("PR Match Event\r\nCapture = %d\r\n", TU16B_CaptureValueRead());
                    break;
                }
                case RESET_UTMR:
                {
                    printf("Reset Event\r\nCapture = %d\r\n", TU16B_CaptureValueRead());
                    break;
                }

            }
            printUTMRB = NO_EVENT_UTMR;
        }

    }    
}