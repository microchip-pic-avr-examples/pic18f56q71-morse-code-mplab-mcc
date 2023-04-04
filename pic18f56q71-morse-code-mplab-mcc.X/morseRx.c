#include "morseCommon.h"
#include "morseRx.h"

#include <stdint.h>
#include <xc.h>
#include "./mcc_generated_files/system/system.h"

#include "ringBuffer.h"

typedef enum {
    MORSE_RX_IDLE = 0, MORSE_RX_POS, MORSE_RX_POS_TO, MORSE_RX_POS_DONE, MORSE_RX_NEG, MORSE_RX_NEG_TO
} MorseStateRx;

static volatile uint8_t rxBitsCaptured = 0;
static volatile uint8_t bufferRx = 0x00;

static volatile MorseStateRx rxState = MORSE_RX_IDLE;

static char cBufferTx[CHAR_BUFFER_SIZE];
static RingBuffer tBuffer;

//This function initializes the internal structures required for morse code
void morseRx_init(void)
{
    //Create the ring buffers
    ringBuffer_createBuffer(&tBuffer, &cBufferTx, CHAR_BUFFER_SIZE);
        
    //Set timeout to max time
    TU16B_PeriodValueSet(MORSE_RX_TIMEOUT);
}

//Converts a 16-bit time into a dot, dash, break, or unknown / error character
MorseCharacter morseConvertToCharacter(uint16_t time)
{
    MorseCharacter c = MORSE_ERROR_CHAR;
    
    uint8_t timeUnits = 0;
    
    while (time >= MORSE_TIME_BASE)
    {
        time -= MORSE_TIME_BASE;
        timeUnits++;
    }
    
    //If 50% or more, round up
    if (time >= (MORSE_TIME_BASE >> 1))
    {
        timeUnits++;
    }
    
    
    if (timeUnits <= MORSE_DOT_MAX)
    {
        //Dot
        c = MORSE_DOT_CHAR;
    }
    else if ((timeUnits >= MORSE_DASH_MIN) && (timeUnits <= MORSE_DASH_MAX))
    {
        //Dash
        c = MORSE_DASH_CHAR;
    }
    else if ((timeUnits >= MORSE_WORD_BREAK_MIN) & (timeUnits <= MORSE_WORD_BREAK_MAX))
    {
        //Break
        c = MORSE_BREAK_CHAR;
    }
    
    return c;
}

//Converts the dot-dash sequence received into a letter
void dotDashLookup(void)
{
    //No bits were captured!
    if (rxBitsCaptured == 0)
    {
        bufferRx = 0;
        return;
    }
        
    char c = '?';
    
    //A - Z search
    uint8_t index = 0;
    bool contSearch = true;
    
    if (rxBitsCaptured == 5)
    {
        //Number or Start
        
        if (bufferRx == MORSE_START_CHAR)
        {
            //This is "START" transmission
            ringBuffer_loadString(&tBuffer, "<START>");
            
            //Reset counters
            rxBitsCaptured = 0;
            bufferRx = 0;

            return;
        }
        
        while (contSearch)
        {
            if (morseTable09[index] == bufferRx)
            {
                //Match!
                c = index + '0';
                contSearch = false;
            }

            //Increment Index
            index++;

            //If at end of numbers
            if (index == 10)
            {
                contSearch = false;
            }
        }
        
    }
    else
    {
        //Letter
        while (contSearch)
        {
            if (morseLengthsAZ[index] == rxBitsCaptured)
            {
                //Possible match - same length
                if (morseTableAZ[index] == bufferRx)
                {
                    //Match!
                    c = index + 'A';
                    contSearch = false;
                }
            }

            //Increment Index
            index++;

            //If at end of alphabet
            if (index == 26)
            {
                contSearch = false;
            }
        }
    }
    
    
    //If still not matched, check 0-9
    if (c == '?')
    {
        index = 0;
    }
    
    //Reset counters
    rxBitsCaptured = 0;
    bufferRx = 0;
    
    //Load captured character
    ringBuffer_loadCharacter(&tBuffer, c);
}

//Prints the string received
void printReceivedString(void)
{
    while (!ringBuffer_isEmpty(&tBuffer))
    {
        if (UART2_IsTxReady())
        {
            UART2_Write(ringBuffer_getChar(&tBuffer));
        }
    }
    
    printf("\r\n");
}

//ISR callback for TU16A
void morseCallback_TU16A(void)
{
    if (TU16A_HasCaptureOccured())
    {
        //Capture Done
        rxState = MORSE_RX_POS;
    }
    else if (TU16A_HasPRMatchOccured())
    {
        //Timeout!
        
        //Stop TU16A to prevent constant interrupts
        TU16A_Stop();
        
        rxState = MORSE_RX_POS_TO;
    }
}
    
//ISR callback for TU16B
void morseCallback_TU16B(void)
{
    if (TU16B_HasCaptureOccured())
    {
        //Capture Done
        
        if (rxState == MORSE_RX_POS_DONE)
        {
            rxState = MORSE_RX_NEG;
        }
        else
        {
            //Invalid positive width, return to idle
            rxState = MORSE_RX_IDLE;
        }

    }
    else if (TU16B_HasPRMatchOccured())
    {
        //Timeout!
        
        //Stop TU16B to prevent constant interrupts
        TU16B_Stop();
        
        if (rxState == MORSE_RX_POS_DONE)
        {
            rxState = MORSE_RX_NEG_TO;
        }
        else
        {
            //Invalid positive width, return to idle
            rxState = MORSE_RX_IDLE;
        }
    }
}

//ISR callback for CLC3 Rising Edge
void morseCallback_onStart(void)
{
    //Restart TU16B
    TU16B_Start();
}

//This function is called by main to handle receive events
void morseStateMachineRx(void)
{
    switch (rxState)
    {
        case MORSE_RX_POS:
        {
            //Valid positive width
            
            MorseCharacter pos = morseConvertToCharacter(TU16A_CaptureValueRead());
            
            if (pos == MORSE_DOT_CHAR)
            {
                printf(".");
                rxBitsCaptured++;
                //Note - shifts in 0, so no OR needed
            }
            else if (pos == MORSE_DASH_CHAR)
            {
                printf("-");
                bufferRx |= (0b1 << rxBitsCaptured);
                rxBitsCaptured++;
            }
            
            rxState = MORSE_RX_POS_DONE;
            
            break;
        }
        case MORSE_RX_POS_TO:
        {
            //Positive width timeout (TO)
            
            //Wait for timer to stop (done by ISR)
            while (TU16A_IsTimerRunning());
            
            //Restart!
            TU16A_Start();
            
            //Update state
            rxState = MORSE_RX_POS_DONE;
            
            break;
        }
        case MORSE_RX_POS_DONE:
        {
            //Positive done - waiting for negative
            break;
        }
        case MORSE_RX_NEG:
        {
            //Valid negative width
            //Transmission still on-going
            
            MorseCharacter neg;
            neg = morseConvertToCharacter(TU16B_CaptureValueRead());
            
            if (neg == MORSE_DASH_CHAR)
            {
                //End of character
                dotDashLookup();
#ifdef SHOW_LETTER_BREAKS
                printf("|");
#endif
            }
            else if (neg == MORSE_BREAK_CHAR)
            {
                //Word break
                
                dotDashLookup();
                ringBuffer_loadCharacter(&tBuffer, ' ');
                printf(SPACE_CHAR_RX);
            }
            
            //Update state machine
            rxState = MORSE_RX_IDLE;
            
            break;
        }
        case MORSE_RX_NEG_TO:
        {
            //Negative width timeout (TO)
            //End of transmission
            
            //Last lookup
            dotDashLookup();
            
            printf("\r\n> ");
            printReceivedString();
            
            //Update state machine
            rxState = MORSE_RX_IDLE;
            
            break;
        }
        case MORSE_RX_IDLE:
        default:
        {
            
        }
    }
}