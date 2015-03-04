#include "dht22.h"
#define _XTAL_FREQ 16000000

// Global static variables to avoid creation on function
unsigned char gi = 0;
unsigned char gj = 0;
char DHT22_Byte = 0;

void DHT22_Init()
{
    // Port value = 0 and high impedance
    DHT22_TRIS = 1;
    DHT22_PORT = 0;
    DHT22_LAT  = 0;
}

int DHT22_ReadSensor(char * RHh, char * RHl, char * Th, char * Tl)
{
    char CRCVal;
    gi = 0;

    // Step 1 : Send start signal
    // Push down the DATA line during at least 1ms
    DHT22_TRIS = 0;
    DHT22_LAT  = 0;
    __delay_ms(1);
    DHT22_TRIS = 1; // Release the DATA line
    
    // Wait until 50us (+10us of margin)
    while ((DHT22_PORT == 1) && (++gi < 50))
        __delay_us(1);
    
    // If the loop ended without response
    if (gi == 50)
        return DHT22_ERROR;

    gi = 0;
    // Step 2 : DHT22 response
    // Wait for rising edge until 90us (+10us of margin)
    while ((DHT22_PORT == 0) && (++gi < 90))
        __delay_us(1);
    
    // If the loop ended without response
    if (gi == 90)
        return DHT22_ERROR;
    gi = 0;
    
    // Wait for falling edge until 90us (+10us of margin)
    while ((DHT22_PORT == 1) && (++gi < 90))
        __delay_us(1);
    
    // If the loop ended without response
    if (gi == 90)
        return DHT22_ERROR;

    // Step 3 : DHT22 send data
    // Read byte 0
    *RHh = DHT22_ReadByte();

    // Read byte 1
    *RHl = DHT22_ReadByte();

    // Read byte 2
    *Th = DHT22_ReadByte();
    
    // Read byte 3
    *Tl = DHT22_ReadByte();

    // Read byte 4
    CRCVal = DHT22_ReadByte();

    
    if (CRCVal != ((*RHh + *RHl + *Th + *Tl) & 0xFF))
        return DHT22_ERROR;
    else
        return DHT22_SUCCESS;
}
inline char DHT22_ReadByte()
{
    //DHT22_Byte = 0;
    DHT22_Byte  = (DHT22_ReadBit() << 7);
    DHT22_Byte |= (DHT22_ReadBit() << 6);
    DHT22_Byte |= (DHT22_ReadBit() << 5);
    DHT22_Byte |= (DHT22_ReadBit() << 4);

    DHT22_Byte |= (DHT22_ReadBit() << 3);
    DHT22_Byte |= (DHT22_ReadBit() << 2);
    DHT22_Byte |= (DHT22_ReadBit() << 1);
    DHT22_Byte |= DHT22_ReadBit();
    return DHT22_Byte;
}

inline char DHT22_ReadBit()
{
    gj = 0;
    // If the line is not pushed down
    while ((DHT22_PORT == 0) && (++gj<50));
    gj = 0;
    while (DHT22_PORT == 1) ++gj;

    if (gj < 20)
        return 0x00;
    else
        return 0x01;
}