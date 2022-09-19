#ifndef SPI_HOST_H
#define	SPI_HOST_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include <stdint.h>
    
    //Initializes a SPI Host
    //I/O must be initialized separately
    void SPI_initHost(void);
    
    //Initializes the I/O for the SPI Host
    void SPI_initPins(void);
    
    //Sends and receives a single byte
    uint8_t SPI_transferByte(uint8_t data);
    
    //Sends a single byte. Received data is discarded
    void SPI_sendByte(uint8_t data);
    
    //Receives a single byte
    uint8_t SPI_recieveByte(void);
    
    //Send and receives LEN bytes
    void SPI_transferBytes(uint8_t* txData, uint8_t* rxData, uint8_t len);
    
    //Sends LEN bytes. Received data is discarded
    void SPI_sendBytes(uint8_t* txData, uint8_t len);
    
    //Receives LEN bytes
    void SPI_receiveBytes(uint8_t* rxData, uint8_t len);
    
#ifdef	__cplusplus
}
#endif

#endif	/* SPI_HOST_H */

