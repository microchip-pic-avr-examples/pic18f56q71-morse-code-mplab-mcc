#ifndef MORSELOOKUP_H
#define	MORSELOOKUP_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdint.h>
    
//Time base for morse code (in ms)
//Note: With internal transmitter, this must be a multiple of 40
#define MORSE_TIME_BASE 200
    
//Depth of the character buffers
#define CHAR_BUFFER_SIZE 128
    
//If set, the START character is transmitted before the message    
//#define ENABLE_START_TX
#define MORSE_START_CHAR 0b10101
        
    const uint8_t morseLengthsAZ[] = {
        2, 4, 4, 3, 1, 
        4, 3, 4, 2, 4, 3, 4, 2, 2, 3,
        4, 4, 3, 3, 1, 3, 4, 3, 4, 4, 
        4};
    
    const uint8_t morseTableAZ[] = {0b10, 0b0001, 0b0101, 0b001, 0b0, 
    0b0100, 0b011, 0b0000, 0b00, 0b1110, 0b101, 0b0010, 0b11, 0b01, 0b111, 
    0b0110, 0b1011, 0b010, 0b000, 0b1, 0b100, 0b1000, 0b110, 0b1001, 0b1101,
    0b0011};
    
    //Note: All have length = 5
    const uint8_t morseTable09[] = {
        0b11111, 0b11110, 0b11100, 0b11000, 0b10000, 0b00000, 0b00001, 0b00011, 0b00111, 0b01111
    };
        
#ifdef	__cplusplus
}
#endif

#endif	/* MORSELOOKUP_H */

