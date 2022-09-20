// PIC18F56Q71 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1
#pragma config FEXTOSC = OFF    // External Oscillator Selection (Oscillator not enabled)
#pragma config RSTOSC = HFINTOSC_64MHZ// Reset Oscillator Selection (HFINTOSC with HFFRQ = 64 MHz and CDIV = 1:1)

// CONFIG2
#pragma config CLKOUTEN = OFF   // Clock out Enable bit (CLKOUT function is disabled)
#pragma config PR1WAY = ON      // PRLOCKED One-Way Set Enable bit (PRLOCKED bit can be cleared and set only once)
#pragma config BBEN = OFF       // Boot Block enable bit (Boot block disabled)
#pragma config CSWEN = ON       // Clock Switch Enable bit (Writing to NOSC and NDIV is allowed)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor enabled)
#pragma config FCMENP = ON      // Fail-Safe Clock Monitor - Primary XTAL Enable bit (Fail-Safe Clock Monitor enabled; timer will flag FSCMP bit and OSFIF interrupt on EXTOSC failure.)
#pragma config FCMENS = ON      // Fail-Safe Clock Monitor - Secondary XTAL Enable bit (Fail-Safe Clock Monitor enabled; timer will flag FSCMP bit and OSFIF interrupt on SOSC failure.)

// CONFIG3
#pragma config MCLRE = EXTMCLR  // MCLR Enable bit (If LVP = 0, MCLR pin is MCLR; If LVP = 1, RE3 pin function is MCLR )
#pragma config PWRTS = PWRT_16  // Power-up timer selection bits (PWRT set at 16ms)
#pragma config MVECEN = ON      // Multi-vector enable bit (Multi-vector enabled, Vector table used for interrupts)
#pragma config IVT1WAY = ON     // IVTLOCK bit One-way set enable bit (IVTLOCKED bit can be cleared and set only once)
#pragma config LPBOREN = OFF    // Low Power BOR Enable bit (Low-Power BOR disabled)
#pragma config BOREN = SBORDIS  // Brown-out Reset Enable bits (Brown-out Reset enabled , SBOREN bit is ignored)

// CONFIG4
#pragma config BORV = VBOR_1P9  // Brown-out Reset Voltage Selection bits (Brown-out Reset Voltage (VBOR) set to 1.9V)
#pragma config ZCD = OFF        // ZCD Disable bit (ZCD module is disabled. ZCD can be enabled by setting the ZCDSEN bit of ZCDCON)
#pragma config PPS1WAY = ON     // PPSLOCK bit One-Way Set Enable bit (PPSLOCKED bit can be cleared and set only once; PPS registers remain locked after one clear/set cycle)
#pragma config STVREN = ON      // Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
#pragma config LVP = ON         // Low Voltage Programming Enable bit (Low voltage programming enabled. MCLR/VPP pin function is MCLR. MCLRE configuration bit is ignored)
#pragma config DEBUG = OFF      // Background Debugger (Background Debugger disabled)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Extended Instruction Set and Indexed Addressing Mode disabled)

// CONFIG5
#pragma config WDTCPS = WDTCPS_31// WDT Period selection bits (Divider ratio 1:65536; software control of WDTPS)
#pragma config WDTE = OFF       // WDT operating mode (WDT Disabled; SWDTEN is ignored)

// CONFIG6
#pragma config WDTCWS = WDTCWS_7// WDT Window Select bits (window always open (100%); software control; keyed access not required)
#pragma config WDTCCS = SC      // WDT input clock selector (Software Control)

// CONFIG7
#pragma config BBSIZE = BBSIZE_256// Boot Block Size Selection bits (Boot Block size is 256 words)

// CONFIG8
#pragma config SAFSZ = SAFSZ_NONE// SAF Block Size Selection bits (NONE)

// CONFIG9
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot Block not Write protected)
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers not Write protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM not Write protected)
#pragma config WRTSAF = OFF     // SAF Write protection bit (SAF not Write Protected)
#pragma config WRTAPP = OFF     // Application Block write protection bit (Application Block not write protected)

// CONFIG10
#pragma config CPD = OFF        // Data EEPROM Code Protection bit (Data EEPROM code protection disabled)

// CONFIG11
#pragma config CP = OFF         // PFM Code Protection bit (PFM code protection disabled)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include "spi1_client.h"
#include "interrupts.h"

#include <stdint.h>
#include <stdbool.h>

#define BUFFER_SIZE 64
static volatile uint8_t buffer[BUFFER_SIZE];
static volatile uint8_t readIndex = 0, writeIndex = 0;

/* 
 * Expected Behavior
 * Use a host to transmit a constant byte pattern (such as 0x01, 0xF0, 0x03, etc...)
 * After the 1st complete cycle, the device will retransmit the same values it received.
 */
void SPI_TEST_Polling(void)
{
    bool active = false;
    
    while (1)
    {
        if (SPI1_isStopped())
        {
            //Stop
            
            active = false;
            SPI1_clearStopFlag();
            SPI1_flushBuffer();
        }
        else if (SPI1_isStarted())
        {
            //Start

            //Reset Indexes
            readIndex = 0;
            writeIndex = 0;
            
            //Update State
            active = true;
            
            //Clear Flag
            SPI1_clearStartFlag();
        }
        else if (active)
        {
            //Data can be read
            while (SPI1_canReadData() && (readIndex <= BUFFER_SIZE))
            {
                buffer[readIndex] = SPI1_readData();
                readIndex++;
            }

            //Data can be written
            while (SPI1_canWriteData() && (writeIndex <= BUFFER_SIZE))
            {
                SPI1_writeData(buffer[writeIndex]);
                writeIndex++;
            }

        }
    }
}

void SPI_TEST_myRXFunction(uint8_t data)
{
    buffer[readIndex] = data;
    readIndex++;
}

uint8_t SPI_TEST_myTXFunction(void)
{
    //Caution: Due to FIFO, n + 2 bytes are loaded
    uint8_t data = buffer[writeIndex];
    writeIndex++;
    return data;
}

void SPI_TEST_myStartFunction(void)
{
    LATC7 = !LATC7;
}

void SPI_TEST_myStopFunction(void)
{
    //Flush the buffer
    SPI1_flushBuffer();
    readIndex = 0;
    writeIndex = 0;

}

//Tests to run (only 1 will be run)
//#define TEST_SPI_POLLING
#define TEST_SPI_INT

void main(void) {    
    //Init SPI I/O
    SPI1_initPins();
    
    //Init the SPI peripheral in Host mode
    SPI1_initClient();
    
    //Enable TX and RX
    SPI1_enableTransmit();
    SPI1_enableReceive();
    
    //Init Interrupts
    Interrupts_init();
    
    //Configure LED0 on Board
    TRISC7 = 0;
    LATC7 = 1;
    
    bool ok = false;
    
#ifdef TEST_SPI_POLLING
    
    //Note: Infinite Loop - does not execute code below
    SPI_TEST_Polling();
        
#elif defined TEST_SPI_INT
    
    SPI1_setTXHandler(&SPI_TEST_myTXFunction);
    SPI1_setRXHandler(&SPI_TEST_myRXFunction);
    SPI1_setStartHandler(&SPI_TEST_myStartFunction);
    SPI1_setStopHandler(&SPI_TEST_myStopFunction);
    
    //Enable Interrupts
    SPI1_enableInterrupts();    
    Interrupts_enable();
    
#endif
    
    while (1)
    {
    }
    
    return;
}
