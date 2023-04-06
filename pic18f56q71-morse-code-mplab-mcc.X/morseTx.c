#include "mcc_generated_files/system/system.h"
#include "morseTx.h"
#include "morseCommon.h"

#include "ringBuffer.h"

#include <stdint.h>
#include <stdbool.h>

//Timebase for Morse Code
#define BASE_TIME_TX (MORSE_TIME_BASE / 40)

//Dots and Dashes
#define DOT BASE_TIME_TX
#define DASH (BASE_TIME_TX * 3)

//Length of quiet time between dot and dashes (in the same letter)
#define DOT_DASH_BREAK BASE_TIME_TX

//Length of quiet time between letters
#define CHAR_BREAK (BASE_TIME_TX * 3)

//Length of quiet time between words
#define WORD_BREAK (BASE_TIME_TX * 7)

typedef enum {
    MORSE_CHAR = 0, MORSE_BIT_BREAK, MORSE_CHAR_BREAK, MORSE_WORD_BREAK, MORSE_COMPLETE_WAIT, MORSE_COMPLETE
} MorseStateTx;

static volatile MorseStateTx txState = MORSE_COMPLETE;
static volatile uint8_t currentC = 0x00;
static volatile uint8_t charBitsRemaining = 0;
static volatile bool inputSwitched = false;

static char cBufferRx[CHAR_BUFFER_SIZE];
static RingBuffer rBuffer;

//Init the Morse Code Transmitter
void morseTx_init(void)
{
    //Create UART Rx Ring Buffer
    ringBuffer_createBuffer(&rBuffer, &cBufferRx, CHAR_BUFFER_SIZE);

    //Enable UART RX Interrupts
    PIE8bits.U2RXIE = 1;

    //Init UART
    UART2_Enable();
}

//Internal function to setup the morse code variables
void morseTx_setupTransmitter(char c)
{
    uint8_t index = 0;
    
    if (c == ' ')
    {
        //Special case - set to Word break
        txState = MORSE_WORD_BREAK;
        return;
    }
    else if ((c >= 'A') && (c <= 'Z'))
    {
        //Uppercase
        index = c - 'A';
        currentC = morseTableAZ[index];
        charBitsRemaining = morseLengthsAZ[index];
    }
    else if ((c >= 'a') && (c <= 'z'))
    {
        //Lowercase
        index = c - 'a';
        currentC = morseTableAZ[index];
        charBitsRemaining = morseLengthsAZ[index];
    }
    else if ((c >= '0') && (c <= '9'))
    {
        //0 - 9
        //Note: All numbers are length 5
        
        currentC = morseTable09[c - '0'];
        charBitsRemaining = 5;
    }
    else
    {
        //Invalid - put a space in
        charBitsRemaining = 0;
        txState = MORSE_WORD_BREAK;
        return;
    }

    if (txState == MORSE_WORD_BREAK)
    {
        txState = MORSE_CHAR;
    }
    else if (txState == MORSE_COMPLETE)
    {
        txState = MORSE_CHAR;
    }
    else
    {
        txState = MORSE_CHAR_BREAK;
    }
    
}

void morseTx_stateMachine(void)
{
    uint8_t nPR = 0;
        
    switch (txState)
    {
        case MORSE_CHAR:
        {
            if (currentC & 0x1)
            {
                //Dash
                nPR = DASH;
            }
            else
            {
                nPR = DOT;
            }
            currentC >>= 1;
            charBitsRemaining--;
            
            if ((charBitsRemaining == 0) && (ringBuffer_isEmpty(&rBuffer)))
            {
                //Done!
                txState = MORSE_COMPLETE_WAIT;
            }
            else if (charBitsRemaining == 0)
            {
                //Load next char
                char c = ringBuffer_getChar(&rBuffer);
                morseTx_setupTransmitter(c);
            }
            else
            {
                //Wait for the delay
                txState = MORSE_BIT_BREAK;
            }
            
            MORSE_ON();
            break;
        }
        case MORSE_BIT_BREAK:
        {
            nPR = DOT_DASH_BREAK;
            txState = MORSE_CHAR;
            MORSE_OFF();
            break;
        }
        case MORSE_CHAR_BREAK:
        {
            nPR = CHAR_BREAK;
                        
            txState = MORSE_CHAR;
            MORSE_OFF();
            break;
        }
        case MORSE_WORD_BREAK:
        {
            nPR = WORD_BREAK;
            
            if (!ringBuffer_isEmpty(&rBuffer))
            {
                morseTx_setupTransmitter(ringBuffer_getChar(&rBuffer));
            }
            else
            {
                txState = MORSE_COMPLETE;
            }
            
            MORSE_OFF();
            break;
        }
        case MORSE_COMPLETE_WAIT:
        {
            //Wait the min period before finishing
            nPR = DOT_DASH_BREAK;
            txState = MORSE_COMPLETE;
            MORSE_OFF();
            break;
        }
        case MORSE_COMPLETE:
        {
            //Done with TX
            //Exit Function
            
            MORSE_OFF();
            return;
        }
    }
    
    Timer2_PeriodCountSet(nPR);
    Timer2_Start();
}

//This function returns true if the user switched the input source
bool morseTx_isSwitchRequested(void)
{
    return inputSwitched;
}

//Returns true if the transmitter is idle
bool morseTx_isIdle(void)
{
    return (txState == MORSE_COMPLETE);
}

//Clear the RX source request flag
void morseTx_clearSwitchRequest(void)
{
    inputSwitched = false;
}

//Ready a string for TX
void morseTx_queueString(const char* str)
{
    ringBuffer_loadString(&rBuffer, str);
}

//Begin to transmit
void morseTx_startTransmit(void)
{
    if ((!ringBuffer_isEmpty(&rBuffer)) && (txState == MORSE_COMPLETE))
    {
#ifdef ENABLE_START_TX
        //Transmit START Sequence (0b10101)
        currentC = MORSE_START_CHAR;
        charBitsRemaining = 5;
        txState = MORSE_CHAR;
        morseTx_stateMachine();
#else
        morseTx_setupTransmitter(ringBuffer_getChar(&rBuffer));
#endif        
        
        //Note - call state machine from ISR to sync timings
        Timer2_PeriodCountSet(1);
        Timer2_Start();
    }
}

void __interrupt(irq(U2RX),base(8)) UART2_RX_Interrupt(void)
{
    while (UART2_IsRxReady())
    {
        char c = U2RXB;
        if ((c == '\n') || (c == '\r'))
        {
            morseTx_startTransmit();
        }
        else if (c == '#')
        {
            //Request Decode Source Change
            inputSwitched = true;
        }
        else
        {
            ringBuffer_loadCharacter(&rBuffer, U2RXB);
        }
        
    }
}