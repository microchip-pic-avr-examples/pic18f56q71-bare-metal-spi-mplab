#include "spi1_client.h"
#include "interrupts.h"

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>

static void (*rxCallback)(uint8_t) = 0;
static uint8_t (*txCallback)(void) = 0;
static void (*startCallback)(void) = 0;
static void (*stopCallback)(void) = 0;

//Initializes a SPI Client
//I/O must be initialized separately
//TX and RX are enabled separately
void SPI1_initClient(void)
{
    SPI1CON0 = 0x00;
    
    //Active Low CS
    SPI1CON1 = 0x00;
    SPI1CON1bits.SSP = 1;
    
    SPI1CON2 = 0x00;
    
    //Select HFINOTSC
    SPI1CLK = 0b00001;
    
    //At HFINOTSC = 64 MHz, this generates a 1 MHz signal
    SPI1BAUD = 31;
    
    //Clear Flags
    SPI1INTF = 0x00;
    
    //Clear set interrupts
    SPI1INTE = 0x00;
    
    //Enable Module
    SPI1CON0bits.EN = 1;
}

//Initializes the I/O for the SPI Client
void SPI1_initPins(void)
{
    //RC2 - SDO
    //RC5 - SDI
    //RC6 - SCK
    //RA5 - CS1
    
    //SDO Config
    TRISC2 = 1;     //Set as input for tri-stating
    ANSELC2 = 0;
    RC2PPS = 0x1E;
    
    //SDI Config
    TRISC5 = 1;
    ANSELC5 = 0;
    SPI1SDIPPS = 0b010101;
    
    //SCK Config
    TRISC6 = 1;
    ANSELC6 = 0;
    SPI1SCKPPS = 0b010110;
    
    //CS Config
    TRISA5 = 1;
    ANSELA5 = 0;
    SPI1SSPPS = 0b000101;
}

//Flushes the SPI Buffer
void SPI1_flushBuffer(void)
{
    SPI1STATUSbits.CLRBF = 1;
}

//Returns true if the RX FIFO can be read from
bool SPI1_canReadData(void)
{
    return PIR3bits.SPI1RXIF;
}

//Returns true if the TX FIFO can accept data
bool SPI1_canWriteData(void)
{
    return PIR3bits.SPI1TXIF;
}

//Returns true when SS transitions from de-asserted to asserted
bool SPI1_isStarted(void)
{
    return SPI1INTFbits.SPI1SOSIF;
}

//Returns true when SS transitions from asserted to de-asserted
bool SPI1_isStopped(void)
{
    return SPI1INTFbits.SPI1EOSIF;
}

//Clears the start flag
void SPI1_clearStartFlag(void)
{
    SPI1INTFbits.SPI1SOSIF = 0;
}

//Clears the stop flag
void SPI1_clearStopFlag(void)
{
    SPI1INTFbits.SPI1EOSIF = 0;
}

//Reads a byte of data from the RX FIFO
uint8_t SPI1_readData(void)
{
    return SPI1RXB;
}

//Writes a byte of data to the TX FIFO
void SPI1_writeData(uint8_t data)
{
    SPI1TXB = data;
}

//Enable TX
void SPI1_enableTransmit(void)
{
    SPI1CON2bits.TXR = 1;
}

//Disable TX
void SPI1_disableTransmit(void)
{
    SPI1CON2bits.TXR = 0;
}

//Enable RX
void SPI1_enableReceive(void)
{
    SPI1CON2bits.RXR = 1;
}

//Disable RX
void SPI1_disableReceive(void)
{
    SPI1CON2bits.RXR = 0;
}

//Enable SPI Interrupts
void SPI1_enableInterrupts(void)
{
    PIE3bits.SPI1TXIE = 1;
    PIE3bits.SPI1RXIE = 1;
    PIE3bits.SPI1IE = 1;
}

//Disable SPI Interrupts
void SPI1_disableInterrupts(void)
{
    PIE3bits.SPI1TXIE = 0;
    PIE3bits.SPI1RXIE = 0;
    PIE3bits.SPI1IE = 0;
}


//Sets a TX callback function when new data can be sent
void SPI1_setTXHandler(uint8_t (*callback)(void))
{
    txCallback = callback;
}

//Sets an RX callback function when new data can be read
void SPI1_setRXHandler(void (*callback)(uint8_t))
{
    rxCallback = callback;
}

//Sets a callback function when SS is asserted
//Interrupts must be enabled for the callback to be run
void SPI1_setStartHandler(void (*callback)(void))
{
    //Enable Start Interrupt
    SPI1INTEbits.SOSIE = 1;

    startCallback = callback;
}
    
//Sets a callback function when SS is de-asserted
//Interrupts must be enabled for the callback to be run
void SPI1_setStopHandler(void (*callback)(void))
{
    //Enable Stop Interrupt
    SPI1INTEbits.EOSIE = 1;
    
    stopCallback = callback;
}


void __interrupt(irq(SPI1TX), base(INTERRUPT_BASE)) SPI_readTX_ISR(void)
{
    if (txCallback != 0)
    {
        asm("NOP");
        SPI1TXB = txCallback();
    }
    else
    {
        SPI1TXB = 0x00;
    }
    
    //Interrupt flag is cleared automatically by writing
}

void __interrupt(irq(SPI1RX), base(INTERRUPT_BASE)) SPI_readRX_ISR(void)
{
    volatile uint8_t rx = SPI1RXB;
    
    if (rxCallback != 0)
    {
        rxCallback(rx);
    }
    
    //Interrupt flag is cleared automatically by reading
}

void __interrupt(irq(SPI1), base(INTERRUPT_BASE)) SPI_status_ISR(void)
{
    if (SPI1INTFbits.SOSIF)
    {
        //SS was Asserted
        if (startCallback != 0)
        {
            startCallback();
        }
        
        SPI1INTFbits.SOSIF = 0;
        
    }
    else if (SPI1INTFbits.EOSIF)
    {
        //SS was De-asserted
        if (stopCallback != 0)
        {
            stopCallback();
        }

        
        SPI1INTFbits.EOSIF = 0;
    }
}
