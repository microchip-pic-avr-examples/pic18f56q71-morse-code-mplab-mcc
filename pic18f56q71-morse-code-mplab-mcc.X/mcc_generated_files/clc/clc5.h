/**
 * CLC5 Generated Driver API Header File.
 * 
 * @file clc5.h
 * 
 * @defgroup  clc5 CLC5
 * 
 * @brief This file contains the API prototypes for the CLC5 driver.
 *
 * @version CLC5 Driver Version 1.0.1
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

#ifndef CLC5_H
 #define CLC5_H

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>

/**
 * @ingroup clc5
 * @brief  Initializes the CLC5. This routine configures the CLC5 specific control registers.
 * @param None.
 * @return None.
 * 
 */
void CLC5_Initialize(void);


/**
 * @ingroup clc5
 * @brief Returns the output pin status of the CLC module.
 * @param  None.
 * @retval True - Output is 1.
 * @retval False - Output is 0.
 */
bool CLC5_OutputStatusGet(void);

#endif  // CLC5_H
/**
 End of File
*/

