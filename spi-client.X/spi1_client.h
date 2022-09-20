#ifndef SPI1_CLIENT_H
#define	SPI1_CLIENT_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include <stdint.h>
#include <stdbool.h>
    
    //Initializes a SPI Client
    //I/O must be initialized separately
    //TX and RX are enabled separately
    void SPI1_initClient(void);
    
    //Initializes the I/O for the SPI Client
    void SPI1_initPins(void);
    
    //Flushes the SPI Buffer
    void SPI1_flushBuffer(void);
    
    //Returns true if the RX FIFO can be read from
    bool SPI1_canReadData(void);
    
    //Returns true if the TX FIFO can accept data
    bool SPI1_canWriteData(void);
    
    //Returns true when SS transitions from de-asserted to asserted
    bool SPI1_isStarted(void);
    
    //Returns true when SS transitions from asserted to de-asserted
    bool SPI1_isStopped(void);

    //Clears the start flag
    void SPI1_clearStartFlag(void);
    
    //Clears the stop flag
    void SPI1_clearStopFlag(void);
    
    //Reads a byte of data from the RX FIFO
    uint8_t SPI1_readData(void);
    
    //Writes a byte of data to the TX FIFO
    void SPI1_writeData(uint8_t data);
    
    //Enable TX
    void SPI1_enableTransmit(void);
    
    //Disable TX
    void SPI1_disableTransmit(void);
    
    //Enable RX
    void SPI1_enableReceive(void);
    
    //Disable RX
    void SPI1_disableReceive(void);
    
    //Enable SPI Interrupts
    void SPI1_enableInterrupts(void);
    
    //Disable SPI Interrupts
    void SPI1_disableInterrupts(void);
    
    //Sets a TX callback function when new data can be sent
    //Interrupts must be enabled for the callback to be run
    void SPI1_setTXHandler(uint8_t (*callback)(void));
    
    //Sets an RX callback function when new data can be read
    //Interrupts must be enabled for the callback to be run
    void SPI1_setRXHandler(void (*callback)(uint8_t));
    
    //Sets a callback function when SS is asserted
    //Interrupts must be enabled for the callback to be run
    void SPI1_setStartHandler(void (*callback)(void));
    
    //Sets a callback function when SS is de-asserted
    //Interrupts must be enabled for the callback to be run
    void SPI1_setStopHandler(void (*callback)(void));

    
#ifdef	__cplusplus
}
#endif

#endif	/* SPI_CLIENT_H */

