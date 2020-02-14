
#include "srvBehav.h"

extern QWaitCondition conditions;

//-----------------------------------------------------------------------------
//--- Constructor
//-----------------------------------------------------------------------------



TTDBehav::TTDBehav() :THw()
{
  qDebug()<<"Start constructor of class TTDBehav";
  workDir=QCoreApplication::applicationDirPath();
  abort=false;
  for(int i=0;i<ALLREQSTATES;i++) allStates[i]=IDLE;
  phase = INITIAL_STATE;
  wrFlag=0;dataReady=0;hwReady=0;
  qDebug()<<"Constructor of class TTDBehav was finished";
}

//-----------------------------------------------------------------------------
//--- Destructor
//-----------------------------------------------------------------------------
TTDBehav::~TTDBehav()
{
  qDebug()<<"Start destructor of class TTDBehav";
  setAbort(true);
  wait(2000);
  if(isRunning()) {
    terminate();
    wait(2000);
  }
  qDebug()<<"Destructor of class TTDBehav was finished";
}

// qDebug operator owerwrite for print states in debug mode
QDebug operator <<(QDebug dbg, const CPhase &t)
{
 // dbg.nospace() <<"\n---------------------------------\nSTATE=";
  dbg.nospace()<<"ORB BEHAVE STATE=";
  switch(t){
  case IDLE: dbg.space() << "IDLE" ; break;
  case INITIAL_STATE: dbg.space() <<       "INITIAL_STATE" ; break;
  case DEVICE_ERROR_STATE: dbg.space() <<  "DEVICE_ERROR_STATE"; break;
  case DATA_READY_STATE: dbg.space() <<    "DATA_READY_STATE" ; break;
  case GET_DATA_STATE: dbg.space() <<      "GET_DATA_STATE" ; break;
  case UPDATE_STATUS_STATE: dbg.space() << "UPDATE_STATUS_STATE" ; break;
  default:;
  }
  return dbg.nospace() ;//<< endl;;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Run thread cycle
void TTDBehav::run()
{
  CPhase deb=IDLE;//for debug only

  QEventLoop *loop;
  loop=new QEventLoop();



  msleep(1000);
  qDebug()<<"Run cycle processing of object TTDBehav";
  phase = INITIAL_STATE;
  while(!abort){
    mutex.lock();
    if(phase==IDLE) {
      for(int i=0;i<ALLREQSTATES;i++) { // read all statese request and run if state!= READY state. high priority has low index
        if(allStates[i]!=IDLE){
          phase=allStates[i];
          allStates[i]=IDLE;
          break;
        }
      }
    }
    mutex.unlock();
    if(deb!=phase) { qDebug()<<phase;deb=phase;}
    switch(phase) {
      case IDLE: {

        msleep(50);
        break;
      }//end case IDLE:
// initial device on begin work
      case INITIAL_STATE: { //
        devError.clearDevError(); // clear device error
        syslog(LOG_INFO,"Initialization of device is begin.");
        qDebug()<<"Initialization of device is begin.";
        initialDevice();
        hwReady=1;
        stErr=devError.analyseError();
        if(stErr) {
          syslog(LOG_ERR,"Initialization of device error. Try again.");
          qDebug()<<"Initialization of device error. Try again.";
          abort=true;
        }
        else {
          allStates[INITIAL_STATE]=IDLE;
          phase = IDLE;  
          qDebug()<<"Initialization of device was finished successfully.";
        }
        break;
      }// end case INITIAL_STATE:

      case DATA_READY_STATE:{

        phase=IDLE;
        break;
      }// end case DATA_READY_STATE:
      case GET_DATA_STATE:{
        dataReady=0;
        phase=IDLE;
        break;
      }// end case GET_DATA_STATE:
      case UPDATE_STATUS_STATE: {

        phase=IDLE;
        break;
      }// end case UPDATE_STATUS_STATE:
// processing hardware errors in device
      case DEVICE_ERROR_STATE:{
        syslog(LOG_ERR,"Found device error!");
        qDebug()<<"Found device error!";

        QStringList err=devError.readDevTextErrorList();
        for(int i=0;i<err.size();i++){
           syslog(LOG_ERR,err.at(i).toAscii());
           qDebug()<<err.at(i);
        }
        phase = IDLE;//UPDATE_STATUS_STATE;
        break;
      }// end case DEVICE_ERROR_STATE
      default:;
    } // end switch
  }//end while

  qDebug()<<"Run() processing of object TTDBehav was finished";
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public functions
void TTDBehav::initialDevice(void)
{
/*  if(readCA(workDir+"/dat/")){
    devError.setDevError(TDevErrors::INFILE_ERROR);
    return ;
  }
  if(usr_initailHW()) {
    devError.setDevError(TDevErrors::GPIO_ERROR);
    return ;
  }
  if(!usr_configureFPGA((char*)QString(workDir+"/rbf/orbita_project.rbf").toStdString().c_str())){
    usr_rst(); //reset FPGA
    u_int32_t titmp[timePoints]={10,20,30,40,50,60,70,80,90,100,110,120,130,140,150,160};
    for(u_int32_t i=0;i<16;i++)  { usr_wrTimerVal(titmp[i],i); }
    usr_wrMode(0);
  }
  else{
    devError.setDevError(TDevErrors::FPGA_LOAD_ERROR);
  }
*/int m=0;
  if(measure()) qDebug()<<readErrors();
  //else for(int i=0;i<maxPointOnCh;i++) //for(int m=0;m<adcCh;m++)
  //qDebug()<<i<<kADC[1][15]<<adcData[1][i*adcCh+15];
}

// read coefficient,amendment (statical and dynamical), absent axis
int TTDBehav::readCA(QString workDirPath)
{
  int axis;
  bool ok;

// absent axis R read
/*  QFile fileAbsR(workDirPath+"/absR.dat");
  if(fileAbsR.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QTextStream inAbsR(&fileAbsR);
    while (!inAbsR.atEnd()) {
      axis=inAbsR.readLine().toInt(&ok);
      if(!ok) continue;
      if((axis>120)||(axis<=1)) continue; // illegal number of axis/ Must be in 2-120 range
      absAxisR[axis/2-1]=true;
    }
  }
  else {
    qDebug()<<"File absR.dat is absent.";
    syslog( LOG_INFO,"File absR.dat is absent.");
  }


  QString sAxis,saR,saZ,scR,scZ,sdaR,sdaZ;
  floatA tmp;
// Read dynamical amendment (add in minus sign!!!)
  QFile fileADR(workDirPath+"/adR.dat");
  if(fileADR.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QTextStream inADR(&fileADR);
    while (!inADR.atEnd()) {
      inADR >>sAxis>>sdaR;
      axis=sAxis.toInt(&ok);
      if(!ok) continue;
      if((axis>120)||(axis<=1)) continue; // illegal number of axis/ Must be in 2-120 range
      tmp=sdaR.toFloat(&ok);
      if(!ok) tmp=0;
      adR[axis/2-1]=tmp;
    }
  }
  else {
    qDebug()<<"File adR.dat is absent.";
    syslog( LOG_INFO,"File adR.dat is absent.");
  }
*/

  return 0;
}



//-----------Create ADC data from HW
void TTDBehav::createAdcData(int mode)
{

  //qDebug()<<"DEBUG ERR"<<"createAdcData exit";
}



void TTDBehav::readMeasure(double orbitaR[][16])
{

}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public slots
// Forming request for get data
// return 0 if all OK
int TTDBehav::getDataRequest(void)
{
  if(phase!=IDLE) return(1);
  allStates[GET_DATA_STATE]=GET_DATA_STATE;
  return(0);
}
