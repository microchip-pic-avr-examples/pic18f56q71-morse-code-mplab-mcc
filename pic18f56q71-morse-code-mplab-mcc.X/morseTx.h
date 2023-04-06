#ifndef MORSETX_H
#define	MORSETX_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
        
//Morse Code TX
#define MORSE_ON() do { RW1_SetHigh(); LED0_SetLow(); } while(0)
#define MORSE_OFF() do { RW1_SetLow(); LED0_SetHigh(); } while(0)
    
    //Init the Morse Code Transmitter
    void morseTx_init(void);
    
    //This function is called by the timer interrupt
    void morseTx_stateMachine(void);
    
    //This function returns true if the user switched the input source
    bool morseTx_isSwitchRequested(void);
    
    //Returns true if the transmitter is idle
    bool morseTx_isIdle(void);
    
    //Clear the RX source request flag
    void morseTx_clearSwitchRequest(void);
    
    //This function sets up required internal structures
    void morseTx_setupTransmitter(char c);
    
    //Ready a string for TX
    void morseTx_queueString(const char* str);
    
    //Begin transmission of data in buffer
    void morseTx_startTransmit(void);
    
#ifdef	__cplusplus
}
#endif

#endif	/* MORSETX_H */

