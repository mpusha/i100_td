//#include <stdint.h>
//#include <sys/io.h>
//#include <stdio.h>
#include "summa.h"
// All function except instrc instrz return codes of error
//0 - ok
//1 absent signal end of cyclee
//2 absent X
//3 absent Q
//4 absent crate

void setcr(uint8_t cr);
uint8_t instrc(void);
uint8_t instrz(void);
uint8_t rcam16(uint8_t cr,uint8_t nr,uint8_t ar,uint8_t fr,uint16_t *res);
uint8_t wcam16(uint8_t cr,uint8_t nr,uint8_t ar,uint8_t fr, uint16_t data);
uint8_t ccamac(uint8_t cr,uint8_t nr,uint8_t ar,uint8_t fr);
int init_K331(uint8_t cr);

uint8_t waitk(void)
{
  int i;
  uint8_t tmp=inb(adrqx);
  if((tmp&0x08)==0) return(4);  
  for(i=0;i<10;i++){
    tmp=inb(adrqx); 
    if((tmp&0x04)==0){ 
      if((tmp&0x01)==0) {
        if((tmp&0x02)==0) return(0); else return(3);
      }
      else {
        return(2);
      }
    }
  }
  return(1);
}

void wcnaf(uint8_t cr,uint8_t nr,uint8_t ar,uint8_t fr)
{
  uint8_t tmp;
  outb(cr,adrc); 
  tmp=((nr&0x0f)<<4)|(ar&0x0f);
  outb(tmp,adran); 
  tmp=((nr&0x10)>>4)|((fr&0x1f)<<1);
  outb(tmp,adrf); 
}

// set crate
void setcr(uint8_t cr)
{
  outb(cr,adrc); 
}

// Run instrc on summa
uint8_t instrc(void)
{
  outb(0,adran); 
  outb(0x80,adrf); 
  uint8_t tmp=waitk();
  return(tmp);
}

// Run instrz on summa
uint8_t instrz(void)
{
  outb(0,adran); 
  outb(0x40,adrf); 
  uint8_t tmp=waitk();
  return(tmp);
}

// Read 16 bit summa device with crate - cr, number - nr, address - ar, function - fr and result in pointer on res
// return code of error. 0 - if OK
uint8_t rcam16(uint8_t cr,uint8_t nr,uint8_t ar,uint8_t fr,uint16_t *res)
{
  wcnaf(cr,nr,ar,fr);
  uint8_t tmp=waitk(); 
  if(tmp) return tmp;
  *res=(inb(ainf2)<<8) | inb(ainf1); 
  return(0);  
}

// Write 16 bit summa device with crate - cr, number - nr, address - ar, function - fr and data - data
// return code of error. 0 - if OK
uint8_t wcam16(uint8_t cr,uint8_t nr,uint8_t ar,uint8_t fr, uint16_t data)
{
  outb((uint8_t)(data&0xff),ainf1);
  outb((uint8_t)((data>>8)&0xff),ainf2);
  wcnaf(cr,nr,ar,fr);
  return(waitk()); 
  
}

// Clear summa device with crate cr number nr address ar and function fr
// return code of error. 0 - if OK
uint8_t ccamac(uint8_t cr,uint8_t nr,uint8_t ar,uint8_t fr)
{
  wcnaf(cr,nr,ar,fr);
  return(waitk()); 
}

// Initialise controller K331. cr number of 'summa' crate.
// return code of error. 0 - if OK
int init_K331(uint8_t cr)
{
  ioperm(basaddr,16,1);
  setcr(cr);
  instrc();
  if(instrz()!=2) return(4);
  return(0);
}


