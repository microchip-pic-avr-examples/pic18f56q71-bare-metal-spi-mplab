/*
© [2022] Microchip Technology Inc. and its subsidiaries.
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

#ifndef SPI1_HOST_H
#define	SPI1_HOST_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include <stdint.h>
    
//If defined, the HW will assert Serial Select (SS) automatically
#define HW_SS_ENABLE
    
    //Initializes a SPI Host
    //I/O must be initialized separately
    void SPI1_initHost(void);
    
    //Initializes the I/O for the SPI Host
    void SPI1_initPins(void);
    
    //Sends and receives a single byte
    uint8_t SPI1_exchangeByte(uint8_t data);
    
    //Sends a single byte. Received data is discarded
    void SPI1_sendByte(uint8_t data);
    
    //Receives a single byte
    uint8_t SPI1_recieveByte(void);
    
    //Send and receives LEN bytes
    void SPI1_exchangeBytes(uint8_t* txData, uint8_t* rxData, uint8_t len);
    
    //Sends LEN bytes. Received data is discarded
    void SPI1_sendBytes(uint8_t* txData, uint8_t len);
    
    //Receives LEN bytes
    void SPI1_receiveBytes(uint8_t* rxData, uint8_t len);
    
#ifdef	__cplusplus
}
#endif

#endif	/* SPI_HOST_H */

