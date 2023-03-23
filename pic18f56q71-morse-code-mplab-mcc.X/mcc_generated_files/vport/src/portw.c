/**
 * PORTW Generated Driver File
 * 
 * @file portw.c
 * 
 * @ingroup portw
 * 
 * @brief This is the generated driver implementation file for the PIC18 MCU PORTW.
 *
 * @version PORTW Driver Version 1.0.0
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

/**
  Section: Included Files
*/

#include <xc.h>
#include "../portw.h"


/**
  Section: PORTW APIs
*/

void PORTW_Initialize(void) 
{
    //LATW 0; 
    LATW = 0x0;

    //IN LATW0; 
    PORTWIN0 = 0x0;

    //IN LATW1; 
    PORTWIN1 = 0x0;

    //IN LATW2; 
    PORTWIN2 = 0x0;

    //IN LATW3; 
    PORTWIN3 = 0x0;

    //IN LATW4; 
    PORTWIN4 = 0x0;

    //IN LATW5; 
    PORTWIN5 = 0x0;

    //IN LATW6; 
    PORTWIN6 = 0x0;

    //IN LATW7; 
    PORTWIN7 = 0x0;

    //CLK TMR2_OUT; 
    PORTWCLK = 0x19;

    //DF 0xff; 
    PORTWDF = 0xFF;

    //PORTW 0; 
    PORTW = 0x0;

    //PWCLKEN enabled; 
    VPORTCON = 0x1;

    //IOCWP
    IOCWP = 0x0;

    //IOCWN
    IOCWN = 0x0;


}

void PORTW_ClockEnable(void) 
{
    VPORTCONbits.PWCLKEN = 0x1;
}

void PORTW_ClockDisable(void) 
{
    VPORTCONbits.PWCLKEN = 0x0;
}




/**
 End of File
*/