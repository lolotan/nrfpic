#ifndef GPIO_H
#define	GPIO_H

#define GPIO_ON     1
#define GPIO_OFF    0

void GPIO_Init(void);
void GPIO_SetCSN(int);
void GPIO_SetCE(int);
int  GPIO_ReadIRQ(void);

#endif	/* GPIO_H */

