#include "spi_host.h"

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>

//Initializes a SPI Host
//I/O must be initialized separately
void SPI_initHost(void)
{
    //Host Mode, Bit Mode
    SPI1CON0 = 0x00;
    SPI1CON0bits.MST = 1;
    
    //SS is Active Low
    SPI1CON1 = 0x00;
    SPI1CON1bits.SSP = 1;
    
    //Clear RXR, TXR, SS is active only when CNT > 0
    SPI1CON2 = 0x00;
    
    //Select HFINTOSC as Clock Source
    SPI1CLK = 0b00001;
    
    //From a 64MHz clock, 1 MHz SCK
    SPI1BAUD = 31;
    
    //Set Width to 8-bits (n = 0)
    SPI1TWIDTH = 0;
    
    //Enable SPI
    SPI1CON0bits.EN = 1;
}

//Initializes the I/O for the SPI Host
void SPI_initPins(void)
{
    //RC2 - SDO
    //RC5 - SDI
    //RC6 - SCK
    //RA5 - CS1
    
    //SDO Config
    TRISC2 = 0;
    RC2PPS = 0x1E;
    
    //SDI Config
    TRISC5 = 1;
    ANSELC5 = 0;
    SPI1SDIPPS = 0b010101;
    
    //SCK Config
    TRISC6 = 0;
    RC6PPS = 0x1D;
    
    //CS Config
    TRISA5 = 0;
    RA5PPS = 0x1F;
}

//Sends and receives a single byte
uint8_t SPI_transferByte(uint8_t data)
{
    uint8_t output = data;
    SPI_transferBytes(&output, &output, 1);
    return output;
}

//Sends a single byte. Received data is discarded.
void SPI_sendByte(uint8_t data)
{
    SPI_sendBytes(&data, 1);
}

//Receives a single byte. Transmitted data is 0x00
uint8_t SPI_recieveByte(void)
{
    uint8_t rx = 0x00;
    SPI_receiveBytes(&rx, 1);
    return rx;
}

//Send and receives LEN bytes.
void SPI_transferBytes(uint8_t* txData, uint8_t* rxData, uint8_t len)
{
    //Clear data buffers
    SPI1STATUSbits.CLRBF = 1;
    
    //Enable TX and RX
    SPI1CON2bits.TXR = 1;
    SPI1CON2bits.RXR = 1;
    
    //Clear status bit
    SPI1INTFbits.TCZIF = 0;
    
    //Load Byte 0
    SPI1TXB = txData[0];
    
    //Set data length
    SPI1TCNTL = len;
    
    //Write / Read Index
    uint8_t wIndex = 1, rIndex = 0;
    
    //While counter is not zero
    while (!SPI1INTFbits.TCZIF)
    {
        if ((PIR3bits.SPI1TXIF) && (wIndex < len))
        {
            //TX Buffer has space, load next byte (until we hit the LEN)
            SPI1TXB = txData[wIndex];
            wIndex++;
        }
        
        if (PIR3bits.SPI1RXIF)
        {
            //RX Buffer Ready
            rxData[rIndex] = SPI1RXB;
            rIndex++;
        }
    }
    
    //Protects against a possible edge case where a byte is received as the module stops
    if (PIR3bits.SPI1RXIF)
    {
        //RX Buffer Ready
        rxData[rIndex] = SPI1RXB;
        rIndex++;
    }
}

//Sends LEN bytes. Received data is discarded.
void SPI_sendBytes(uint8_t* txData, uint8_t len)
{
    //Clear data buffers
    SPI1STATUSbits.CLRBF = 1;
    
    //Enable TX and Disable RX
    SPI1CON2bits.TXR = 1;
    SPI1CON2bits.RXR = 0;
    
    //Clear status bit
    SPI1INTFbits.TCZIF = 0;
    
    //Load Byte 0
    SPI1TXB = txData[0];
    
    //Set data length
    SPI1TCNTL = len;
    
    //Write / Read Index
    uint8_t wIndex = 1;
    
    //While counter is not zero
    while (!SPI1INTFbits.TCZIF)
    {
        if ((PIR3bits.SPI1TXIF) && (wIndex < len))
        {
            //TX Buffer has space, load next byte (until we hit the LEN)
            SPI1TXB = txData[wIndex];
            wIndex++;
        }
    }
}

//Receives LEN bytes. Transmitted data is 0x00
void SPI_receiveBytes(uint8_t* rxData, uint8_t len)
{
    //Clear data buffers
    SPI1STATUSbits.CLRBF = 1;
    
    //Enable RX and Disable TX
    SPI1CON2bits.TXR = 0;
    SPI1CON2bits.RXR = 1;
    
    //Clear status bit
    SPI1INTFbits.TCZIF = 0;
    
    //Set data length
    SPI1TCNTL = len;
    
    //Write / Read Index
    uint8_t rIndex = 0;
    
    //While counter is not zero
    while (!SPI1INTFbits.TCZIF)
    {
        //Protects against a possible edge case where a byte is received as the module stops
        if (PIR3bits.SPI1RXIF)
        {
            //RX Buffer Ready
            rxData[rIndex] = SPI1RXB;
            rIndex++;
        }
    }
    
    //Protects against a possible edge case where a byte is received as the module stops
    if (PIR3bits.SPI1RXIF)
    {
        //RX Buffer Ready
        rxData[rIndex] = SPI1RXB;
        rIndex++;
    }
}
