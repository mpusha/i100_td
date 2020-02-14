/*
 * hw.h
 *
 */

#ifndef HW_H
#define HW_H

#include <QtCore>
#include <QMutex>
#include "summa.h"

#define WAIT_INTERVAL 11  // sec for wait summa operation
//#define MAX_POINTS    255 // maximal points of ADC for reading
//typedef QVector <float> floatA;

typedef float floatA;


const int32_t maxADCs=12;       // maximal counts of adc modules
const int32_t maxPointOnCh=200; // maximal point for graph plots (one point 6.4 us)
const int32_t adcCh=16;         // maximal adc chanels in one module
const double tStep=6.4e-6;      // step point to point

extern "C" void setcr(uint8_t cr);
extern "C" uint8_t instrc(void);
extern "C" uint8_t instrz(void);
extern "C" uint8_t rcam16(uint8_t cr,uint8_t nr,uint8_t ar,uint8_t fr,uint16_t *res);
extern "C" uint8_t wcam16(uint8_t cr,uint8_t nr,uint8_t ar,uint8_t fr, uint16_t data);
extern "C" uint8_t ccamac(uint8_t cr,uint8_t nr,uint8_t ar,uint8_t fr);
extern "C" int init_K331(uint8_t cr);



/****************************************************************
*  HW function
****************************************************************/

class THw
{
//  Q_OBJECT
  public:
    THw();
    ~THw();
    int initHW(void);

    int measure(void);
    QString readErrors(void) { return err; }
    double adcData[maxADCs][maxPointOnCh*adcCh];
  private:
    uint8_t cr,sync_N,adc_N[maxADCs];
    bool adcMap[maxADCs];

    double kADC[maxADCs][adcCh];
    QString err;

    int readSettings(QString fname);
    void clearAdc(void);
    void readAdc(uint8_t,uint8_t nr,double *adcData);
};

#endif /* HW_H */
