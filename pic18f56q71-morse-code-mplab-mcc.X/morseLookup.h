/* 
 * File:   morseLookup.h
 * Author: C62081
 *
 * Created on March 22, 2023, 12:43 PM
 */

#ifndef MORSELOOKUP_H
#define	MORSELOOKUP_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "mcc_generated_files/system/pins.h"
    
//Morse Code TX
#define MORSE_ON() do { RW1_SetHigh(); LED0_SetLow(); } while(0)
#define MORSE_OFF() do { RW1_SetLow(); LED0_SetHigh(); } while(0)
    
//Select input to decoder
#define SELECT_TX_DECODE() do { RW0_SetLow(); } while(0)
#define SELECT_USER_DECODE() do { RW0_SetHigh(); } while(0)

//Units of ms
#define MORSE_TIME_BASE 200
    
//Nominal - 2 units
#define MORSE_DOT_MAX 2
    
//Nominal - 3 units
#define MORSE_DASH_MIN 3
#define MORSE_DASH_MAX 4

//Nominal - 7 units
#define MORSE_WORD_BREAK_MIN 5
#define MORSE_WORD_BREAK_MAX 9
    
#define SPACE_CHAR_RX "/"
    
//Depth of the character buffer
#define CHAR_BUFFER_SIZE 128
    
    const static uint8_t morseLengthsAZ[] = {
        2, 4, 4, 3, 1, 
        4, 3, 4, 2, 4, 3, 4, 2, 2, 3,
        4, 4, 3, 3, 1, 3, 4, 3, 4, 4, 
        4};
    
    const static uint8_t morseTableAZ[] = {0b10, 0b0001, 0b0101, 0b001, 0b0, 
    0b0100, 0b011, 0b0000, 0b00, 0b1110, 0b101, 0b0010, 0b11, 0b01, 0b111, 
    0b0110, 0b1011, 0b010, 0b000, 0b1, 0b100, 0b1000, 0b110, 0b1001, 0b1101,
    0b0011};
    
    typedef enum {
    MORSE_CHAR = 0, MORSE_BIT_BREAK, MORSE_CHAR_BREAK, MORSE_WORD_BREAK, MORSE_COMPLETE_WAIT, MORSE_COMPLETE
    } MorseStateTx;
    
    typedef enum {
        MORSE_DOT_CHAR = 0, MORSE_DASH_CHAR, MORSE_BREAK_CHAR, MORSE_ERROR_CHAR
    } MorseCharacter;
    
    //This function initializes the internal structures required for morse code
    void morseInit(void);
    
    //This function is called by the timer interrupt
    void morseStateMachineTx(void);
    
    //This function is called by main to handle receive events
    void morseStateMachineRx(void);
    
    //This function sets up required internal structures
    void morseSetup(char c);
    
    //Ready a string for TX
    void morseLoadString(const char* str);
    
    //Begin transmission of data in buffer
    void morseStart(void);
    
    //Converts a 16-bit time into a dot, dash, break, or unknown / error character
    MorseCharacter morseConvertToCharacter(uint16_t time);
    
    //Converts the dot-dash sequence received into a letter
    char dotDashLookup(void);
    
    //ISR callback for TU16A
    void morseCallback_TU16A(void);
    
    //ISR callback for TU16B
    void morseCallback_TU16B(void);
    
    //ISR callback for CLC3 Rising Edge
    void morseCallback_onStart(void);

#ifdef	__cplusplus
}
#endif

#endif	/* MORSELOOKUP_H */

