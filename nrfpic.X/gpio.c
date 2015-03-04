#include <htc.h>
#include "gpio.h"

void GPIO_Init(void)
{
    // SCN    
    TRISCbits.RC2 = 0;  // Set SCN as output
    LATCbits.LC2  = 1;  // Set SCN high (idle state)

    // CE    
    TRISCbits.RC1 = 0;  // Set CE as output
    LATCbits.LC1  = 0;  // Set CE low

    // IRQ
    TRISBbits.RB0 = 1;  // Set IRQ as input    
}

void GPIO_SetCSN(int GPIO)
{
    LATCbits.LC2 = GPIO;
}

void GPIO_SetCE(int GPIO)
{
    LATCbits.LC1 = GPIO;
}

int  GPIO_ReadIRQ(void)
{
    return PORTBbits.RB0;
}