#ifndef MORSERX_H
#define	MORSERX_H

#ifdef	__cplusplus
extern "C" {
#endif

//Select input to decoder
#define SELECT_TX_DECODE() do { RW0_SetLow(); } while(0)
#define SELECT_USER_DECODE() do { RW0_SetHigh(); } while(0)
    
//Nominal Dot - 1 unit
#define MORSE_DOT_MAX 1
    
//Nominal Dash - 3 units
#define MORSE_DASH_MIN 2
#define MORSE_DASH_MAX 4

//Nominal Word Break (Space or Slash) - 7 units
#define MORSE_WORD_BREAK_MIN 5
#define MORSE_WORD_BREAK_MAX 7
   
//Sets the timeout for RX (in ms)
#define MORSE_RX_TIMEOUT 1500
    
//This string is used where word breaks occur
#define SPACE_CHAR_RX "/"

//If set, vertical lines '|' will separate characters in morse code
//E.g.: ...---... will be shown as ...|---|...
#define SHOW_LETTER_BREAKS
    
    typedef enum {
        MORSE_DOT_CHAR = 0, MORSE_DASH_CHAR, MORSE_BREAK_CHAR, MORSE_ERROR_CHAR
    } MorseCharacter;
        
    //This function initializes the internal structures required for morse code
    void morseRx_init(void);
    
    //This function is called by main to handle receive events
    void morseStateMachineRx(void);
    
    //Converts a 16-bit time into a dot, dash, break, or unknown / error character
    MorseCharacter morseConvertToCharacter(uint16_t time);
    
    //Converts the dot-dash sequence received into a letter
    void dotDashLookup(void);
    
    //Prints the string received
    void printReceivedString(void);
    
    //ISR callback for TU16A
    void morseCallback_TU16A(void);
    
    //ISR callback for TU16B
    void morseCallback_TU16B(void);
    
    //ISR callback for CLC3 Rising Edge
    void morseCallback_onStart(void);

#ifdef	__cplusplus
}
#endif

#endif	/* MORSERX_H */

