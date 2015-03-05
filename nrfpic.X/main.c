#include <htc.h>
#include <stdio.h>
#include <string.h>
#include "dht22.h"
#include "nrflib.h"
#include "init.h"

#pragma config OSC     = INTIO67
#pragma config FCMEN   = OFF
#pragma config IESO    = OFF
#pragma config PWRT    = ON
#pragma config BOREN   = OFF
#pragma config WDT     = OFF
#pragma config CCP2MX  = PORTC
#pragma config PBADEN  = OFF
#pragma config MCLRE   = OFF
#pragma config LPT1OSC = OFF
#pragma config STVREN  = ON
#pragma config DEBUG   = OFF
#pragma config LVP     = OFF
#pragma config XINST   = OFF

// Write protect
#pragma config CP0     = OFF
#pragma config CP1     = OFF
#pragma config CP2     = OFF
#pragma config CPB     = OFF
#pragma config CPD     = OFF
#pragma config WRT0    = OFF
#pragma config WRT1    = OFF
#pragma config WRT2    = OFF
#pragma config WRTB    = OFF
#pragma config WRTC    = OFF
#pragma config WRTD    = OFF
#pragma config EBTR0   = OFF
#pragma config EBTR1   = OFF
#pragma config EBTR2   = OFF
#pragma config EBTRB   = OFF


#define _XTAL_FREQ          16000000

#define HUMIDITYH_POS       0
#define HUMIDITYL_POS       1
#define TEMPERATUREH_POS    2
#define TEMPERATUREL_POS    3
#define BATTERYLEVEL_POS    4
#define SENDBUFFER_SIZE     5
#define BATTLVL_SAMPLES     3

#define TIMER_BASE          (65536-15625)
#define TIMER_8SEC_VALUE    1
#define TIMER_START_VALUE   TIMER_8SEC_VALUE
#define TIMER_10MIN_VALUE   75
#define MAXPOLLCNT          300

#define ACTIVITY_LED        LATAbits.LATA3

unsigned char GlobalCounter;

void interrupt isr(void)
{
    if (INTCONbits.TMR0IF)
    {
        INTCONbits.TMR0IF = 0;
        TMR0 = TIMER_BASE;     // Rearm timer for 8s
        GlobalCounter++;
    }    
}

void putch(char data)
{
#ifdef DEBUG_MODE
    while(TXSTAbits.TRMT == 0); // check buffer
    TXREG = data;
#endif
}

void WaitPowerSave(unsigned char TimeValue)
{    
    OSCTUNEbits.PLLEN = 0; // PLL OFF
    OSCCONbits.IRCF   = 1; // Oscillator configuration 125kHz
    T0CONbits.TMR0ON  = 1; // Timer0 ON

    while(GlobalCounter < TimeValue)
        SLEEP();
    
    GlobalCounter     = 0;
    T0CONbits.TMR0ON  = 0; // Timer0 OFF
    OSCCONbits.IRCF   = 6; // 4MHz
    OSCTUNEbits.PLLEN = 1; // PLL ON
}

void main(void)
{
    int   ret;    
    int   PollCounter;
    char  Status;
    char  SendBuffer[SENDBUFFER_SIZE];    
    short Temperature;
    
    unsigned short Humidity;
    unsigned char  i;
    unsigned short BatteryLvl;

    GlobalCounter = 0;
    
    InitSystem();
    DHT22_Init();
    NRF_Init();        

    WaitPowerSave(TIMER_START_VALUE);   
    
    for(;;)
    {
        ACTIVITY_LED = 1;             // Turn ON LED

        // Read DHT22 sensor values
        ret = DHT22_ReadSensor(&SendBuffer[HUMIDITYH_POS],
                               &SendBuffer[HUMIDITYL_POS],
                               &SendBuffer[TEMPERATUREH_POS],
                               &SendBuffer[TEMPERATUREL_POS]);
        // Sample battery level
        for (i=0, BatteryLvl=0 ; i<BATTLVL_SAMPLES ; i++)
        {
            ADCON0bits.GODONE = 1;      // Begin ADC convertion
            while(ADCON0bits.GODONE);
            BatteryLvl += ADRESH;
        }        
        SendBuffer[BATTERYLEVEL_POS] = (char)(BatteryLvl / BATTLVL_SAMPLES); // Battery level average
        
#ifdef DEBUG_MODE
        Temperature = (SendBuffer[TEMPERATUREH_POS] << 8) | SendBuffer[TEMPERATUREL_POS];
        Humidity    = (SendBuffer[HUMIDITYH_POS] << 8)    | SendBuffer[HUMIDITYL_POS];

        printf("ReadSensor : %02X %02X %02X %02X %d\r\n", SendBuffer[HUMIDITYH_POS],
                                                  SendBuffer[HUMIDITYL_POS],
                                                  SendBuffer[TEMPERATUREH_POS],
                                                  SendBuffer[TEMPERATUREL_POS], ret);

        printf("Temperature   : %3.1f 'C\r\n", (float)Temperature/10.0);
        printf("Humidity      : %3.1f %%\r\n", (float)Humidity/10.0);
        printf("Battery level : %d\r\n", SendBuffer[BATTERYLEVEL_POS]);        
#endif

        // Power ON NRF module
        NRF_SetPowerMode(POWER_ON, &Status);
        NRF_SetModePTX(&Status);
        NRF_FlushTX(&Status);
        printf("After Power ON STATUS : %02X\r\n", Status);
        
        NRF_WriteTXPayload(SendBuffer, SENDBUFFER_SIZE, &Status);
        NRF_TXPayload();                    // Send payload

        // Poll TX_DS & MAX_RT bit to know packet transmission status
        PollCounter = 0;
        while(PollCounter++ < MAXPOLLCNT)
        {
            NRF_GetStatus(&Status);

            if ((Status & TX_DS) == TX_DS)
            {
                NRF_ClearTX_DS(&Status);
                break;
            }

            if ((Status & MAX_RT) == MAX_RT)
            {
                NRF_ClearMAX_RT(&Status);
                break;
            }
        }

        printf("After PL transmit Status : %02X\r\n", Status);
        NRF_SetPowerMode(POWER_OFF, &Status);   // Power off NRF module
        printf("After Power OFF Status : %02X\r\n", Status);

        ACTIVITY_LED = 0;                       // Turn OFF LED

        WaitPowerSave(TIMER_10MIN_VALUE);       // Powersave wait function
    }
}

