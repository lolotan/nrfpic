#ifndef INIT_H
#define	INIT_H

//#define DEBUG_MODE  // Uncomment this for verbose debug mode

void InitOsc(void);
void InitInterrupts(void);
void InitGPIO(void);
void InitSerial(void);
void InitADC();
void InitTimer0(void);
void InitPowerMode(void);
void InitSystem(void);

#endif	/* INIT_H */

