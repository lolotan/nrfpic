#include <htc.h>

#define DHT22_PORT      PORTAbits.RA2
#define DHT22_LAT       LATAbits.LA2
#define DHT22_TRIS      TRISAbits.RA2
#define DHT22_SUCCESS   0
#define DHT22_ERROR     -1

void DHT22_Init();
int DHT22_ReadSensor(char * RHh, char * RHl, char * Th, char * Tl);
inline char DHT22_ReadByte();
inline char DHT22_ReadBit();