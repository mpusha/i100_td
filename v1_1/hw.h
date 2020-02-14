/*
 * hw.h
 *
 */

#ifndef HW_H
#define HW_H

#include <QtCore>

#include "summa.h"

#define WAIT_INTERVAL 11  // sec for wait summa operation

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

class THw :public QThread
{
  Q_OBJECT
  public:
    THw();
    ~THw();
// system function
    void setAbort(bool ab) { abort=ab; }
    void startMeas_Req(void){ mutex.lock();condition.wakeOne();mutex.unlock();}
// user function and variables
    int initHW(void);
    int measure(void);
    QString readMeasErrorStr(void) { return err; }
    uint32_t readMeasErrorInt(void) { return measErr; }
    uint32_t readMeasSts(void) { return stsMeas; }
    double adcData[maxADCs][maxPointOnCh*adcCh];
    bool adcMap[maxADCs]; // eneble or disable adc
    uint32_t adcChPresent[maxADCs]; //count of work chanels on adc (begin from 1 to 16)

signals:  void endMeasure(void) ;

protected:
  void run();

private:
// system variables
    bool abort;
    QMutex mutex;
    QWaitCondition condition;
// user functions and variables
    uint8_t cr,sync_N,adc_N[maxADCs];

    double kADC[maxADCs][adcCh];
    QString err; // error of measure
    uint32_t measErr; // error of measure return function measure()
    uint32_t stsMeas; // status of measure (1 in progress)

    int readSettings(QString fname);
    void clearAdc(void);
    void readAdc(uint8_t,uint8_t nr,double *adcData); // fill adcData[][]
};

#endif /* HW_H */
