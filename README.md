<!-- Please do not change this logo with link -->

[![MCHP](images/microchip.png)](https://www.microchip.com)

# Bare Metal SPI for the PIC18F56Q71 Family of Microcontrollers
This example provides bare metal reference code for the SPI peripheral of the PIC18F56Q71 family of microcontrollers. Also compatiable (with minor modifications) to otber devices with a standalone SPI peripheral, such as the PIC18F16Q41 family. 


## Related Documentation


## Software Used

- MPLAB® X IDE 6.0.0 or newer [(MPLAB® X IDE 6.0)](#)
- MPLAB XC8 2.40.0 or newer compiler [(MPLAB XC8 2.40)](#)

## Hardware Used

- PIC18F56Q71 Curiosity Nano Evaluation Kit
- Curiosity Nano Base Board
- Wire

For client testing, a SPI host is required. A sutible host was created with a PIC18F16Q41 Curiosity Nano and this host mode driver. 

## Default Pin Assignments

| Pin | Curiosity Board Label | Host I/O Direction | Client I/O Direction | Description 
| --- | ----- | ------------------ | -------------------- | ----------- 
| RC2 | MOSI  | Output | Input | Host Serial Data Output / Client Serial Data Input
| RC5 | MISO  | Input  | Output | Host Serial Data Input / Client Serial Data Output
| RC6 | SCLK  | Output | Input | Serial Clock 
| RA5 | CS1   | Output | Input | Serial Select 1 

These pin assignments correlate with socket 1 on the Curiosity Base Board. 

## What is SPI?

SPI, or Serial Peripheral Interface, is a synchronous serial data interface that transfers and receives data on a clocked signal. There are 4 wires that can be used for SPI, however not all lines are always used:

- Serial Select (SS) / Chip Select (CS)
     -  A unique SS line is required for each client
- Serial Clock (SCLK)
- Serial Data Out (SDO) /  Host Data Output (MOSI) / Client Data Input
- Serial Data In (SDI) / Host Data Input (MISO) / Client Data Output

SDI and SDO can be omitted depending on what is being communicated. If the host device is always writing data, than SDI can be omitted. If the host is always reading data, then SDO can also be omitted. 

### SPI Hosts

The SPI host on the bus controls all communication. There is only 1 SPI host on each bus due to push-pull communication. Hosts are responsible for generating the SCLK signal that is used to move data. Additionally, hosts must select which client they are talking to before communication begins.

### SPI Clients

SPI client devices each have a unique SS input. The SS signal is used to select a specific client on the bus. Once selected, hosts will generate an SCLK to shift data into and out of the clients. 

### SPI vs I<sup>2</sup>C

SPI and I<sup>2</sup>C are both common synchronous serial interfaces for moving data. However, they have distinct differences and advantages over each other. Some of the major differences include:

- SPI can operate at much higher clock speeds
- SPI can send and receive data at the same time
- SPI clients are unlikely to deadlock the communication bus
- I<sup>2</sup>C only uses 2 wires for full-duplex communication with multiple clients, versus 3 + n (where n is the number of clients) for SPI
- I<sup>2</sup>C communication has acknowledgement indicators during communication whereas SPI has no way to tell if clients are receiving

## Host Mode

The driver is composed of 2 files `spi1_host.h` and `spi1_host.c`. **This driver only supports polling mode.**

### Configuring the Driver

#### Selecting I/O
By default, pins RC2, RC5, RC6, and RA5 are used by the driver (see *Default Pin Assignments*). I/O assignments can be changed via the PPS feature on the microcontroller. (In the case of SS, PPS may not be needed. See *Disabling Hardware Control* for more details). All I/O initialization is performed in the function `SPI1_initPins`. 

#### Disabling Hardware Control
In `spi1_host.h`, hardware control of SS can be disabled by commenting out `HW_SS_ENABLE`. When enabled, the hardware peripheral automatically asserts and releases SS during communication. However, the time between SS and and SCLK is very short, which may fail to meet timing requirements for some devices. In this case, SS should controlled by discrete I/O operations, rather than the hardware directly. 

**Important: The (desired) SS pin must be configured as an output elsewhere in the program.**

### Testing Setup  
To test host mode operation, 3 tests were written.

1. `SPI_TEST_Exchange`
2. `SPI_TEST_Send`
3. `SPI_TEST_Receive`

`SPI_TEST_Exchange` and `SPI_TEST_Send` can be run one-after-another, while `SPI_TEST_Receive` must be run in a different setup. A **logic analyzer is highly recommended for validation**, as the send and receive tests will not cover every case. (For our purposes, a logic analyzer was used.) 

Tests 1 and 2 use MISO and MOSI connected together in loopback mode, while test 3 uses MISO tied to 3.3V (**MOSI should be left floating. Do not connect a SPI client.**)

Tests can be switched by enabling the macros `TEST_ENABLE_TX` for 1 and 2, and `TEST_ENABLE_RX` for 3. 

#### Exchange Tests  
In this test, the functions `SPI1_exchangeByte` and `SPI1_exchangeBytes` are called to send and receive data. A test pattern is transmitted and received by the same hardware module. The test validates that the same byte sequence sent is also received.

#### Transmit Only Tests  
In this test, `SPI1_sendByte` and `SPI1_sendBytes` are called to send, but NOT receive data. The test will pass unless data is present in the receive buffer. A logic analyzer can confirm if expected data was sent. 

#### Receive Only Tests  
**Tie MISO to 3.3V for this test. Leave MOSI floating. Do not connect a SPI client.**

In this test, the device reads 5 bytes. Since MISO is tied to 3.3V, it will receive 0xFF into the buffer. This test will pass if the buffer contains only 0xFF. 

### Using the Driver

#### Asserting SS
If you are using hardware control, SS will be asserted and deasserted automatically by hardware.

**If you are using software control, you must assert SS before running any functions in the API. SS must be deasserted afterwards.**

#### Transmit and Receive (Exchange)

The exchange functions `SPI1_exchangeByte` and `SPI1_exchangeBytes` send data while receiving it on the same clock. 

The function `SPI1_exchangeByte` is a wrapper over the multi-byte function `SPI1_exchangeBytes`. Unlike `SPI1_exchangeBytes`, however, the exchange byte function returns the data received, rather than loading it directly into a receiving buffer.

`SPI1_exchangeBytes` sends and receives `len` bytes of data. The `txData` buffer is transmitted, while the `rxData` buffer is filled.

#### Transmit Only Operations (Send)

The transmit functions `SPI1_sendByte` and `SPI1_sendBytes` disable the receiver while transmitting data.

The function `SPI1_sendByte` is a wrapper over the multi-byte function `SPI1_sendBytes`. `SPI1_sendBytes` sends `len` bytes of data in `txData` to client devices.

#### Receive Only Operations (Receive)

The receive functions `SPI1_recieveByte` and `SPI1_recieveBytes` disable the transmitter while receiving data. The transmitter will still transmit, however it will either be the 1st byte in the transmitter's FIFO queue (which is not advanced) or the last received byte. Please consult the datasheet for more information. 

### API Reference 

| Function Definition | Description
| ------------------- | -----------
| void SPI1_initHost(void) | Initializes SPI1 as a host. `SPI1_initPins` must be called to init I/O
| void SPI1_initPins(void) | Initializes the I/O for the SPI Host
| uint8_t SPI1_exchangeByte(uint8_t data) | Sends and receives a single byte
| void SPI1_sendByte(uint8_t data) | Sends a single byte to a client. Received data is discarded
| uint8_t SPI1_recieveByte(void) | Receives a single byte from a client
| void SPI1_exchangeBytes(uint8_t* txData, uint8_t* rxData, uint8_t len) | Send and receives `len` bytes
| void SPI1_sendBytes(uint8_t* txData, uint8_t len) | Sends `len` bytes to clients. Received data is discarded
| void SPI1_receiveBytes(uint8_t* rxData, uint8_t len) | Receives `len` bytes from clients

## Client Mode

The client mode driver is defined in `spi1_client.h` and `spi1_client.c`. Both polling and interrupt mode operation are supported. Interrupt mode requires the use of the Vector Interrupt Controller (VIC). Interrupt definitions can be modified to remove this requirement.

**Caution: Setup time is required between SS being asserted and communication beginning. With the host driver, use software I/O control.**

### Configuring the Driver

#### Selecting I/O  
Like the host driver, I/O is configured in `SPI1_initPins`. Modifying the PPS and I/O assignments can move the pins around. 

#### Enabling Interrupts
By default, the driver does not enable interrupts. SPI interrupts are enabled by the function `SPI1_enableInterrupts` and disabled by `SPI1_disableInterrupts`. The start and stop interrupts are not enabled until an appropriate callback function is set (see *Configuring Interrupt Callbacks* in the interrupt mode description).

#### Modifying the Interrupt Vector  
With interrupts on, the VIC is used by the interrupt definitions. The VIC base address is set in `interrupts.h` by `INTERRUPT_BASE`.

#### Transmit and Receive Enable  
Before using the driver, the transmitter and receiver must be enabled. The functions `SPI1_enableTransmit` and `SPI1_enableReceive` set the flags in the hardware to enable this functionality. If desired, these can be disabled later with `SPI1_disableTransmit` and `SPI1_disableReceive`. 

### Polling Mode
In polling mode, the microcontroller blocks until SPI activity occurs. The polling mode driver requires the user to create a state machine that handles HW signals from the peripheral. A sample state machine is included at the end of this section (excerpted from `main.c`).

#### Start and Stop Events

Fundementally, SPI has 2 states dictated by SS. When SS is deasserted, serial communication is ignored by the module, and thus the driver is inactive. When SS is asserted, serial communication can begin at anytime. The standalone SPI peripheral contains dedicated hardware that can detect when SS changes state. These flags are set by the hardware and are accessible in the functions `SPI1_isStopped` and `SPI1_isStarted`. 

When a stop flag is asserted, the driver needs to cease communication, as no further clock signals will be incoming. When the start flag is asserted, the SS line has been asserted, which suggests communication will begin shortly. 

Note: Some devices like Analog-to-Digital Converters (ADCs) can trigger conversions when SS is asserted. After a delay from the host, the result can be read back from the device. As the polling state machine function is created by the user, such functionality could be mimicked, if desired. 

With these 2 flags, the stop flag should **always** preempt and clear a start flag. Both start and stop flags must be cleared in software with `SPI1_clearStartFlag` and `SPI1_clearStopFlag`.

#### Data Receive and Transmit

When the state machine has detected a start and has not detected a stop, data should be loaded into the SPI module. The SPI hardware supports reading and writing multiple bytes of data at once. To determine if data can be read or written, the function `SPI1_canReadData` and `SPI1_canWriteData` are used. While these are true, data can be written or read using the functions `SPI1_readData` and `SPI1_writeData`, which operate on a per-byte basis. 

Since data is queued by `SPI1_writeData`, usually some data will remain in the buffers when communication is stopped. The function `SPI1_flushBuffer` clears the TX and RX queues. 

#### Sample State Machine

```
void SPI_TEST_Polling(void)
{
    bool active = false;
    
    while (1)
    {
        if (SPI1_isStopped())
        {
            //Stop
            
            //Update State
            active = false;
            
            //Clear flag
            SPI1_clearStopFlag();
            
            //Discard any remaining data in the buffers
            SPI1_flushBuffer();
            
            //Prevents a glitch from triggering a new conversion
            SPI1_clearStartFlag();
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
``` 

### Interrupt Mode

The interrupt mode driver is more flexible driver than the polling mode driver, as the interrupts do not block execution of software while running. However, this results in more complexity to implement desired behavior. 

**Important: Global interrupts must be enabled on the device.**

#### Configuring Interrupt Callbacks
The driver provides 4 interrupts:

- Transmit (Can load data into queue)
- Receive (Can read data from queue)
- Start
- Stop

When SPI interrupts are enabled (via `SPI1_enableInterrupts`), only transmit and receive interrupts will be generated by the hardware (the generic SPI flag interrupt is enabled, but has nothing to trigger an interrupt). To generate start and stop interrupts, assign an interrupt handler to the start and stop flags with `SPI1_setStartHandler` and `SPI1_setStopHandler`. 

Note: If these start and stop interrupts are enabled outside of the driver, the driver will handle them without issue. 

For transmit and receive, if no user function is provided, default behavior will be executed instead. In the case of transmit, 0x00 is loaded into the buffer. For receive, data is read and discarded.

**Important: For all interrupt callbacks, HW flags are cleared automatically by the driver after the callback.**

#### Start and Stop

Start and stop interrupts are generated when the SS line changes state. Function callbacks are assigned with `SPI1_setStartHandler` and `SPI1_setStopHandler`. An example of start and stop interrupts is shown below.

```
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
```

The behavior inside of the interrupts is specific to the application. 

#### Transmitting Data

Transmit interrupts are generated whenever there is room in the FIFO buffer of the SPI peripheral. As a result, extra data will remain in the buffer once communication ceases. Function callbacks are assigned with `SPI1_setTXHandler`. An example callback function is shown below.

```
uint8_t SPI_TEST_myTXFunction(void)
{
    //Caution: Due to FIFO, n + 2 bytes are loaded
    uint8_t data = buffer[writeIndex];
    writeIndex++;
    return data;
}
```

#### Receving Data

Receive interupts are generated whenever data is present in the FIFO buffer of the SPI peripheral. Function callbacks are assigned with `SPI1_setRXHandler`. An example callback function is shown below.

```
void SPI_TEST_myRXFunction(uint8_t data)
{
    buffer[readIndex] = data;
    readIndex++;
}
```

### API Reference

| Function Definition | Description
| ------------------- | -----------
| void SPI1_initClient(void) | Initializes SPI1 as a client. I/O must be configured with `SPI1_initPins`. TX and RX are enabled separately.
| void SPI1_initPins(void) | Initializes the I/O for the SPI Client
| void SPI1_flushBuffer(void) | Flushes the SPI Buffer
| bool SPI1_canReadData(void) | Returns true if the RX FIFO can be read from
| bool SPI1_canWriteData(void) | Returns true if the TX FIFO can accept data
| bool SPI1_isStarted(void) | Returns true when SS transitions from de-asserted to asserted
| bool SPI1_isStopped(void) | Returns true when SS transitions from asserted to de-asserted
| void SPI1_clearStartFlag(void) | Clears the start flag
| void SPI1_clearStopFlag(void) | Clears the stop flag
| uint8_t SPI1_readData(void) | Reads a byte of data from the RX FIFO
| void SPI1_writeData(uint8_t data) | Writes a byte of data to the TX FIFO
| void SPI1_enableTransmit(void) | Enable transmit
| void SPI1_disableTransmit(void) | Disable transmit
| void SPI1_enableReceive(void) | Enable receive
| void SPI1_disableReceive(void) | Disable receive
| void SPI1_enableInterrupts(void) | Enable SPI Interrupts
| void SPI1_disableInterrupts(void) | Disable SPI Interrupts
| void SPI1_setTXHandler(uint8_t (*callback)(void)) | Sets a TX callback function when new data can be sent. Interrupts must be enabled for the callback to be run.
| void SPI1_setRXHandler(void (*callback)(uint8_t)) | Sets an RX callback function when new data can be read. Interrupts must be enabled for the callback to be run.
| void SPI1_setStartHandler(void (*callback)(void)) | Sets a callback function when SS is asserted. Interrupts must be enabled for the callback to be run.
| void SPI1_setStopHandler(void (*callback)(void)) | Sets a callback function when SS is de-asserted. Interrupts must be enabled for the callback to be run.

## Summary
This example has provided a simple driver for standalone SPI modules on the PIC18F56Q71 family.