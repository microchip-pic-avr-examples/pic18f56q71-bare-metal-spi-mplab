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
    uint8_t SPI1_transferByte(uint8_t data);
    
    //Sends a single byte. Received data is discarded
    void SPI1_sendByte(uint8_t data);
    
    //Receives a single byte
    uint8_t SPI1_recieveByte(void);
    
    //Send and receives LEN bytes
    void SPI1_transferBytes(uint8_t* txData, uint8_t* rxData, uint8_t len);
    
    //Sends LEN bytes. Received data is discarded
    void SPI1_sendBytes(uint8_t* txData, uint8_t len);
    
    //Receives LEN bytes
    void SPI1_receiveBytes(uint8_t* rxData, uint8_t len);
    
#ifdef	__cplusplus
}
#endif

#endif	/* SPI_HOST_H */

