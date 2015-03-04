#include <htc.h>
#include "timer.h"


void TIMER_Init(void)
{

}

void TIMER_Wait_us(int UsValue)
{
    int i;
    for (i=0 ; i<(UsValue*4) ; i++)
        NOP();
}
