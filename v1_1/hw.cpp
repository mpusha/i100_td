
/*
 * hw.cpp
 *
 */
#include <QDir>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#include "hw.h"

/****************************************************************
 * Constructor THw
 *
 ****************************************************************/
THw::THw()
{
  qDebug()<<"Start constructor of class THw";
  setAbort(false);
  qDebug()<<"Constructor of class THw was finished";
}

/****************************************************************
 * destructor THw
 *
 ****************************************************************/
THw::~THw()
{
  qDebug()<<"Start destructor of class THw";
  mutex.lock();
  setAbort(true);
  condition.wakeOne();
  mutex.unlock();
  wait(2000);
  if(isRunning()) {
    terminate();
    wait(2000);
  }
  qDebug()<<"Destructor of class THw was finished";
}

/****************************************************************
 * Main cycle of thread
 ****************************************************************/
void THw::run()
{
  qDebug()<<"Run cycle process of object THw";
  stsMeas=0; // no measure
  measErr=0; // no error
  while(!abort){
    mutex.lock();
    condition.wait(&mutex);
    mutex.unlock();
    if(!abort){
      qDebug()<<"Start procedure of measure.";
      stsMeas=1; // measure in progress
      measErr=measure();
      stsMeas=0; // no measure
      qDebug()<<"Measure procedure was finished.";
      if(measErr)qDebug()<<"On measure fixed error: "<<err;
      emit endMeasure();
    }
  }
  qDebug()<<"Run() processing of class THw was finished";
}

/****************************************************************
 * readSettings
 ****************************************************************/
int THw::readSettings(QString fname)
{
  QSettings setupADCs(fname,QSettings::IniFormat) ;
  if(!QFile::exists(fname)) {
    for(int i=0;i<maxADCs;i++) adcMap[i]=false;
    return 1;
  }
  cr=setupADCs.value("crate",0).toInt();
  sync_N=setupADCs.value("sync_n",0).toInt();

  for(int i=0;i<maxADCs;i++) {
    adc_N[i]=setupADCs.value(QString("adc%1_n").arg(i+1),0).toInt();
    adcChPresent[i]=setupADCs.value(QString("adc%1_chpr").arg(i+1),adcCh).toInt();
    if(adc_N[i]==0) adcMap[i]=false; else adcMap[i]=true;
    int size=setupADCs.beginReadArray(QString("ADC%1").arg(i+1));
    for (int j = 0; j < size; ++j) {
      setupADCs.setArrayIndex(j);
      kADC[i][j]=setupADCs.value("kadc",1.0).toDouble();
      //qDebug()<<"kADC["<<i<<"]["<<j<<"]="<<kADC[i][j];
    }
    setupADCs.endArray();
  }
  return 0;
}

/****************************************************************
 * initialization of hardware
 ****************************************************************/
int THw::initHW(void)
{
  if(readSettings(QCoreApplication::applicationDirPath()+"/ini/setupHW.ini")) {
    err="Error! Can't read file ../ini/setupHW.ini";
    return 1;
  }
  if((cr==0)||(sync_N==0)) {
    err="Error! Can't read crate or module of sync number from file ../ini/setupHW.ini";
    return 1;
  }
  return 0;
}

/****************************************************************
 * Measuring
 ****************************************************************/
int THw::measure(void)
{

  err.clear();

  if(initHW()) return 1;
  // read all initialisation data OK
  // initialize controller & set crate number
  if(init_K331(cr))
  {
    err=QString("Can't open K331. Crate %1 is ").arg(cr)+"turn off";
    return 2;
  }

  // wait INT1 & INT2
  time_t bmeas,now;
  time(&bmeas);
  uint16_t ds,newc=0;
  do {
    if(rcam16(cr,sync_N,0,0,&ds)) { err="Module of synchr is absent" ; return 3; } // module of synchr error
    if((ds==1)&&(newc==0)){
      qDebug()<<"Find interrupt from pulse 1. Prepere ADCs.";
      newc=1;
      // clear ADCs and get enable ADC's map
      clearAdc();
    }
    if((ds==2)&&(newc==1)) {
    qDebug()<<"Find interrupt from pulse 2.";
    sleep(1);
    qDebug()<<"Converting was compleated. Reading data From ADCs.";
    // can read data from ADC's
    for(int i=0;i<maxADCs;i++)
      for(int j=0;j<adcCh*maxPointOnCh;j++)
        adcData[i][j]=0;
    for(int i=0;i<maxADCs;i++){
      if(adcMap[i]){
        qDebug()<<"Find adc number"<<i+1<<"on place"<<adc_N[i];
        readAdc(adc_N[i],i,adcData[i]);
      }
    }
    return 0; // all meas OK
    }
    time(&now);
  } while((now-bmeas)<11);
  err="Timeout operation on bus of SUMMA.";
  return 4; // timeout
}

/****************************************************************
 * Clear ADCs
 ****************************************************************/
void THw::clearAdc(void)
{
  for(int i=0;i<maxADCs;i++){
    if(adc_N[i]){
      if(ccamac(cr,adc_N[i],0,9))
        adcMap[i]=false;
      else
        adcMap[i]=true;
    }
    else {
      adcMap[i]=false;
    }
  }
}

/****************************************************************
 * Read data adcData[][] from ADC number numADC on place nr
 ****************************************************************/
void THw::readAdc(uint8_t nr,uint8_t numADC,double *adcData)
{
  int i,j;
  uint16_t d,dch[adcCh];

  ccamac(cr,nr,0,9) ;
  ccamac(cr,nr,1,9) ;
  for(i=0;i<=17;i++)
  {
    if(rcam16(cr,nr,0,0,&d)) return;
  }
  j=0 ;
  while(j<maxPointOnCh)
  {
    for(int m=0;m<adcCh;m++) dch[m]=0;
    for(i=0;i<16;i++)
    {
      for(int m=0;m<adcCh;m++) dch[m]<<=1 ;
      if(rcam16(cr,nr,0,0,&d)) return ;
      for(int m=0;m<adcCh;m++) dch[m]|=(d>>m)&(1);
    }
    for(int m=0;m<adcCh;m++) { dch[m]>>=2 ; *adcData=kADC[numADC][m]*((dch[m]&0xfff)*5/4095.0-5.0);adcData++;}
    // data adc on one measure point 16 chanels
    //ch0P0,ch1P0..ch14P0,ch15P0,ch0P1,ch1P1..ch14P1,ch15P1..ch0(PMAX-1),ch1(PMAX-1)..ch15(PMAX-1)
    j++;
  }
}

