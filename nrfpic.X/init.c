#include <htc.h>
#include "init.h"

void InitOsc(void)
{
    OSCCONbits.IRCF   = 6;  // 4MHz
    OSCTUNEbits.PLLEN = 1;  // PLL ON
}

void InitInterrupts(void)
{
    di();
    INTCONbits.PEIE    = 0; // Disable Peripheral interrupts
    INTCONbits.TMR0IE  = 1; // Enable TMR0 overflow interrupt
    INTCONbits.INT0IE  = 0; // Disable INT0 interrupt
    INTCONbits.RBIE    = 0; // Disable RB port interrupt
    INTCON3bits.INT2IE = 0; // Disable INT2 external interrupt
    INTCON3bits.INT1IE = 0; // Disable INT3 external interrupt
    ei();
}

void InitGPIO(void)
{
    TRISAbits.TRISA3 = 0;   // RA3 as a output
    INTCON2bits.RBPU = 1;   // No pull-ups on PORTB
    CMCON = 0x07;           // Disable comparators
}

void InitSerial(void)
{
#ifdef DEBUG_MODE
    // Serial port for debugging purposes
    TRISCbits.TRISC6 = 0;
    TRISCbits.TRISC7 = 1;

    // Speed configuration : 19200 at 16Mhz
    BAUDCONbits.BRG16 = 1;
    TXSTAbits.BRGH = 0;
    SPBRG = 51;

    TXSTAbits.SYNC = 0;     // Asynchronous mode
    RCSTAbits.SPEN = 1;     // Enable serial port
    TXSTAbits.TXEN = 1;     // Enable transmission
#endif
}

void InitADC()
{
    TRISAbits.RA0   = 1;    // RA0 as input
    ADCON0bits.CHS  = 0;    // Channel 0 (AN0)
    ADCON1          = 0x0E; // AN0 analog, REF = VSS/VDD
    ADCON2bits.ACQT = 5;    // 12 TAD
    ADCON2bits.ADFM = 0;    // Left justified
    ADCON2bits.ADCS = 2;    // 010 = FOSC /32
    ADCON0bits.ADON = 1;    // Activate ADC
}

void InitTimer0(void)
{
    TMR0 = 0x0000;
    INTCONbits.TMR0IF = 0;    
    T0CONbits.T08BIT  = 0;  // 16 bits counter
    T0CONbits.T0CS    = 0;  // Internal instruction cycle clock
    T0CONbits.PSA     = 0;  // Timer0 prescaler is assigned
    T0CONbits.T0PS    = 3;  // 1:16 Prescale value
    T0CONbits.TMR0ON  = 0;  // Timer0 OFF
}

void InitPowerMode(void)
{
    // PRI_IDLE mode when sleep
    OSCCONbits.IDLEN = 1;
    OSCCONbits.SCS   = 0;   // PRI_IDLE
}

void InitSystem(void)
{
    InitOsc();
    InitInterrupts();
    InitGPIO();
    InitSerial();
    InitADC();    
    InitPowerMode();
    InitTimer0();
}