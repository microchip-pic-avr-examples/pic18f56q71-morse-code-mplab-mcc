#include "morseLookup.h"

#include <stdint.h>
#include <xc.h>
#include "./mcc_generated_files/system/system.h"

#include "ringBuffer.h"

//Timebase for Morse Code
#define BASE_TIME 30

//Dots and Dashes
#define DOT BASE_TIME
#define DASH (BASE_TIME * 3)

//Length of quiet time between dot and dashes (in the same letter)
#define DOT_DASH_BREAK BASE_TIME

//Length of quiet time between letters
#define CHAR_BREAK (BASE_TIME * 3)

//Length of quiet time between words
#define WORD_BREAK (BASE_TIME * 7)

static volatile MorseState state = MORSE_COMPLETE;
static volatile uint8_t currentC = 0x00;
static volatile uint8_t charBitsRemaining = 0;

static char cBuffer[CHAR_BUFFER_SIZE];
static RingBuffer rBuffer;

//This function initializes the internal structures required for morse code
void morseInit(void)
{
    //Create the ring buffer
    ringBuffer_createBuffer(&rBuffer, &cBuffer, CHAR_BUFFER_SIZE);
    
    //Enable UART RX Interrupts
    PIE8bits.U2RXIE = 1;
    
    //Init UART
    UART2_Enable();
}

//Internal function to setup the morse code variables
void morseSetup(char c)
{
    uint8_t index = 0;
    
    if (c == ' ')
    {
        //Special case - set to Word break
        state = MORSE_WORD_BREAK;
        return;
    }
    else if ((c >= 'A') && (c <= 'Z'))
    {
        //Uppercase
        index = c - 'A';
    }
    else if ((c >= 'a') && (c <= 'z'))
    {
        index = c - 'a';
    }
    else
    {
        //Invalid - put a space in
        charBitsRemaining = 0;
        state = MORSE_WORD_BREAK;
        return;
    }

    currentC = morseTableAZ[index];
    charBitsRemaining = morseLengthsAZ[index];
    
    if (state == MORSE_WORD_BREAK)
    {
        state = MORSE_CHAR;
    }
    else if (state == MORSE_COMPLETE)
    {
        state = MORSE_CHAR;
    }
    else
    {
        state = MORSE_CHAR_BREAK;
    }
    
}

void morseStateMachine(void)
{
    uint8_t nPR = 0;
        
    switch (state)
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
                state = MORSE_COMPLETE_WAIT;
            }
            else if (charBitsRemaining == 0)
            {
                //Load next char
                char c = ringBuffer_getChar(&rBuffer);
                morseSetup(c);
            }
            else
            {
                //Wait for the delay
                state = MORSE_BIT_BREAK;
            }
            
            MORSE_ON();
            break;
        }
        case MORSE_BIT_BREAK:
        {
            nPR = DOT_DASH_BREAK;
            state = MORSE_CHAR;
            MORSE_OFF();
            break;
        }
        case MORSE_CHAR_BREAK:
        {
            nPR = CHAR_BREAK;
                        
            state = MORSE_CHAR;
            MORSE_OFF();
            break;
        }
        case MORSE_WORD_BREAK:
        {
            nPR = WORD_BREAK;
            
            if (!ringBuffer_isEmpty(&rBuffer))
            {
                morseSetup(ringBuffer_getChar(&rBuffer));
            }
            else
            {
                state = MORSE_COMPLETE;
            }
            
            MORSE_OFF();
            break;
        }
        case MORSE_COMPLETE_WAIT:
        {
            //Wait the min period before finishing
            nPR = DOT_DASH_BREAK;
            state = MORSE_COMPLETE;
            MORSE_OFF();
            break;
        }
        case MORSE_COMPLETE:
        {
            //Done with TX
            MORSE_OFF();
            return;
        }
    }
    
    Timer2_PeriodCountSet(nPR);
    Timer2_Start();
}

//Ready a string for TX
void morseLoadString(const char* str)
{
    ringBuffer_loadString(&rBuffer, str);
}

//Begin to transmit
void morseStart(void)
{
    if ((!ringBuffer_isEmpty(&rBuffer)) && (state == MORSE_COMPLETE))
    {
        morseSetup(ringBuffer_getChar(&rBuffer));
        morseStateMachine();
    }
}

void __interrupt(irq(U2RX),base(8)) UART2_RX_Interrupt(void)
{
    while (UART2_IsRxReady())
    {
        char c = U2RXB;
        if ((c == '\n') || (c == '\r'))
        {
            morseStart();
        }
        else
        {
            ringBuffer_loadCharacter(&rBuffer, U2RXB);
        }
        
    }
}