#include <htc.h>
#include "spi.h"
#include "gpio.h"

char PIC_SpiTxRx(char SpiByte)
{
    SSPBUF = SpiByte;
    while (SSPSTATbits.BF == 0);
    return SSPBUF;
}

int  SPI_Init()
{
    // GPIO configuration
    TRISCbits.RC4 = 1;      // SDI as input
    TRISCbits.RC5 = 0;      // SDO as output
    TRISCbits.RC3 = 0;      // SCK as output
    GPIO_SetCSN(GPIO_ON);   // SCN high as idle state

    // SPI configuration
    SSPSTATbits.SMP   = 0;  // Input data sampled at middle of data output time
    SSPSTATbits.CKE   = 1;  // Transmit occurs on transition from active to Idle clock state
    SSPCON1bits.CKP   = 0;  // Idle state for clock is a low level
    SSPCON1bits.SSPM  = 1;  // SPI Master mode, clock = F OSC /16
    SSPCON1bits.SSPEN = 1;  // Enables SPI
}

int SPI_SendCommand(char Command, char * RetStatus)
{
    GPIO_SetCSN(GPIO_OFF);
    *RetStatus = PIC_SpiTxRx(Command);
    GPIO_SetCSN(GPIO_ON);
    return SPI_SUCCESS;
}

int SPI_CommandWrite(char Command, char * WriteBuffer, int Length, char * RetStatus)
{
    int i;
    char DummyByte;

    GPIO_SetCSN(GPIO_OFF);

    //SPI_SendCommand(Command, RetStatus);
    *RetStatus = PIC_SpiTxRx(Command);
    for (i=0 ; i<Length ; i++)
        DummyByte = PIC_SpiTxRx(WriteBuffer[i]);

    GPIO_SetCSN(GPIO_ON);
    return SPI_SUCCESS;
}

int SPI_CommandRead(char Command, char * ReadBuffer, int Length, char * RetStatus)
{
    int i;

    GPIO_SetCSN(GPIO_OFF);
    
    //SPI_SendCommand(Command, RetStatus);
    *RetStatus = PIC_SpiTxRx(Command);
    for (i=0 ; i<Length ; i++)
        ReadBuffer[i] = PIC_SpiTxRx(0x00);
    

    GPIO_SetCSN(GPIO_ON);
    return SPI_SUCCESS;
}