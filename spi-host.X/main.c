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
// BBSIZE = No Setting

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
#include "spi_host.h"

#include <stdint.h>
#include <stdbool.h>

bool SPI_TEST_Exchange(void)
{
    uint8_t testPattern[] = {0xA5, 0x00, 0x7F, 0xAA, 0x99};
    uint8_t results[5];

    //Test Single Byte Exchange
    results[0] = SPI_transferByte(testPattern[0]);

    //Test Multi-Byte Exchange (1 to 4)
    SPI_transferBytes(&testPattern[1], &results[1], 4);
    
    //Validate Data
    for (uint8_t i = 0; i < sizeof(testPattern); i++)
    {
        if (results[i] != testPattern[i])
        {
            return false;
        }
    }
    
    return true;
}

bool SPI_TEST_Send(void)
{
    uint8_t testPattern[] = { 0xA0, 0x00, 0x01, 0x02, 0x03};
    
    //Test byte send
    SPI_sendByte(testPattern[0]);
    
    //Test multi-byte send
    SPI_sendBytes(&testPattern[1], 4);
    
    //RX should not be set
    if (PIR3bits.SPI1RXIF)
    {
        return false;
    }
    
    //Validate behavior with logic analyzer
    return true;
}

bool SPI_TEST_Receive(void)
{
    //To test this function, connect the MISO line to VDD (3.3V)
    uint8_t receiveData[5];
    
    //Test Single Byte Receive
    receiveData[0] = SPI_recieveByte();
    
    //Test Multi-Byte Receive
    SPI_receiveBytes(&receiveData[1], 4);

    for (uint8_t i = 0; i < sizeof(receiveData); i++)
    {
        if (receiveData[i] != 0xFF)
        {
            return false;
        }
    }
    
    return true;
}

#define TEST_ENABLE_TX
//#define TEST_ENABLE_RX

void main(void) {
    
    //Init SPI I/O
    SPI_initPins();
    
    //Init the SPI peripheral in Host mode
    SPI_initHost();
    
    //Configure LED0 on Board
    TRISC7 = 0;
    LATC7 = 1;
    
    bool ok = false;
    
#ifdef TEST_ENABLE_TX
    
    //Test Exchange Functions
    ok = SPI_TEST_Exchange();
    
    if (!ok)
    {
        //If test failed, set LED
        LATC7 = 0;
    }
    
    //Test Send Functions
    ok = SPI_TEST_Send();
    
    if (!ok)
    {
        //If test failed, set LED
        LATC7 = 0;
    }
    
#elif TEST_ENABLE_RX
    
    //Test Read Functions
    //Connect MISO to 3.3V before running this test!
    ok = SPI_TEST_Receive();
    
    if (!ok)
    {
        //If test failed, set LED
        LATC7 = 0;
    }
    
#endif

    while (1)
    {
        
    }
    
    return;
}
