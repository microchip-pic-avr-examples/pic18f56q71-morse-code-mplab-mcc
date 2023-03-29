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

#define MORSE_TIME_BASE 50
    
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
    } MorseState;
    
    //This function initializes the internal structures required for morse code
    void morseInit(void);
    
    //This function is called by the timer interrupt
    void morseStateMachine(void);
    
    //This function sets up required internal structures
    void morseSetup(char c);
    
    //Ready a string for TX
    void morseLoadString(const char* str);
    
    //Begin to transmit
    void morseStart(void);

#ifdef	__cplusplus
}
#endif

#endif	/* MORSELOOKUP_H */

