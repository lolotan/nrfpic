/* 
 * File:   spi.h
 * Author: laurent
 *
 * Created on 15 février 2015, 22:42
 */

#ifndef SPI_H
#define	SPI_H

#define SPI_SUCCESS 0
#define SPI_ERROR   -1

int SPI_Init();
int SPI_SendCommand(char, char *);
int SPI_CommandWrite(char, char *, int, char *);
int SPI_CommandRead(char, char *, int, char *);

#endif	/* SPI_H */

