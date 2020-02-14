#ifndef SAUMMA_H
#define SUMMA_H

#include <stdint.h>
#include <sys/io.h>
#include <stdio.h>

#define basaddr      0x360
#define ainf1        basaddr
#define ainf2        basaddr+1
#define ainf3        basaddr+2
#define adrqx        basaddr+3
#define adran        basaddr+3
#define adrf         basaddr+4
#define adrc         basaddr+5
#define adrl1        basaddr+6
#define adrl2        basaddr+7


uint8_t waitk(void);
void wcnaf(uint8_t cr,uint8_t nr,uint8_t ar,uint8_t fr);


#endif
