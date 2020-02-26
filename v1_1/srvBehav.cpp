
#include "srvBehav.h"

extern QWaitCondition conditions;

//-----------------------------------------------------------------------------
//--- Constructor
//-----------------------------------------------------------------------------



TTDBehav::TTDBehav()
{
  qDebug()<<"Start constructor of class TTDBehav";
//  workDir=QCoreApplication::applicationDirPath();
  abort=false;
  for(int i=0;i<ALLREQSTATES;i++) allStates[i]=IDLE;

  device=new THw;
  if(device) device->start(QThread::NormalPriority);

//... NetWork ...
  m_ptcpServer = new QTcpServer(this);
  clientConnected=0;
  m_nNextBlockSize=0;
  getDataFromAdc=0;

  if(!m_ptcpServer->listen(QHostAddress::Any, 9002)) {
    m_ptcpServer->close();
    syslog( LOG_INFO, "Can't setup TCP connection. Network error! Program terminated.");
    qDebug()<<"Can't setup TCP connection. Network error! Program terminated.";
  }
  connect(m_ptcpServer, SIGNAL(newConnection()), this, SLOT(slotNewConnection()));
  connect(device,SIGNAL(endMeasure(void)),this,SLOT(endMeasure(void)));
  qDebug()<<"Constructor of class TTDBehav was finished";
}

//-----------------------------------------------------------------------------
//--- Destructor
//-----------------------------------------------------------------------------
TTDBehav::~TTDBehav()
{
  qDebug()<<"Start destructor of class TTDBehav";
  setAbort(true); 
  map_ErrorStr.clear();
  map_ConnectedStatus.clear();
  map_AdcData.clear();
  map_AdcHeader.clear();
  map_MeasureStatus.clear();
  wait(2000);
  if(isRunning()) {
    terminate();
    wait(2000);
  }
  if(device) delete device;
  qDebug()<<"Destructor of class TTDBehav was finished";
}

// qDebug operator owerwrite for print states in debug mode
QDebug operator <<(QDebug dbg, const CPhase &t)
{
 // dbg.nospace() <<"\n---------------------------------\nSTATE=";
  dbg.nospace()<<"BEHAVE STATE=";
  switch(t){
  case IDLE: dbg.space() << "IDLE" ; break;
  case MEASURE_REQUEST_STATE: dbg.space() << "MEASURE_REQUEST_STATE"; break;
  case END_MEASURE_STATE: dbg.space()     << "END_MEASURE_STATE"; break;
  case GET_STATUS_STATE: dbg.space()      << "GET_STATUS_STATE" ; break;
  case GET_DATA_STATE: dbg.space()        << "GET_DATA_STATE" ; break;
  case GER_ERROR_STATE: dbg.space()       << "GET_ERROR_STATE" ; break;

  default:;
  }
  return dbg.nospace() ;//<< endl;;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Run thread cycle
void TTDBehav::run()
{
  CPhase deb=IDLE;//for debug only

  msleep(1000);
  qDebug()<<"Run cycle processing of object TTDBehav";
  phase = IDLE;
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
        msleep(10);
        break;
      }//end case IDLE:

      case MEASURE_REQUEST_STATE:{       
        device->startMeas_Req();
        phase=IDLE;
        break;
      }// end case MEASURE_REQUEST_STATE:
      case END_MEASURE_STATE: {
        sendToClient(pClient, MEASURE_REQUEST);
        phase=IDLE;
        break;
      } // end case END_MEASURE_STATE
      case GET_DATA_STATE:{
        map_AdcHeader.clear();
        map_AdcHeader[GETDATA_NUMADC]=getDataFromAdc;
        map_AdcHeader[GETDATA_ENADC]=(uint32_t)device->adcMap[getDataFromAdc];
        map_AdcHeader[GETDATA_ALLADC]=maxADCs;
        map_AdcHeader[GETDATA_POINTS]=maxPointOnCh;
        map_AdcHeader[GETDATA_ALLCH]=adcCh;
        map_AdcHeader[GETDATA_WORKCH]=device->adcChPresent[getDataFromAdc]; // change !!!!!!!!
        map_AdcHeader[GETDATA_STEP]=(uint32_t)(tStep*1e9);
        map_AdcData.clear();
        for(int i=0;i<maxPointOnCh*adcCh;i++) map_AdcData[i]=device->adcData[getDataFromAdc][i];

        sendToClient(pClient, GETDATA_REQUEST);
        phase=IDLE;
        break;
      }// end case GET_DATA_STATE:

      case GET_STATUS_STATE: {
        map_MeasureStatus.clear();
        map_MeasureStatus[STATUS_PROGRESS]=device->readMeasSts();
        map_MeasureStatus[STATUS_ERROR]=device->readMeasErrorInt();
        sendToClient(pClient, STATUS_REQUEST);
        phase=IDLE;
        break;
      }// end case GET_STATUS_STATE:
      case GER_ERROR_STATE: {
        map_ErrorStr.clear();
        map_ErrorStr[GETERROR_STR]=device->readMeasErrorStr();
        sendToClient(pClient, GETERROR_REQUEST);
        phase=IDLE;
        break;
      }

      default:;
    } // end switch
  }//end while

  qDebug()<<"Run() processing of class TTDBehav was finished";
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public slots
// get endMeasure signal from THw obect
void TTDBehav::endMeasure(void)
{
  allStates[END_MEASURE_STATE]=END_MEASURE_STATE;
}

//================= NETWORK =========================================================================================================================
//-------------------------------------------------------------------------------------------------
//--- create new connection
//-------------------------------------------------------------------------------------------------
void TTDBehav::slotNewConnection()
{
    map_ConnectedStatus.clear();
    QTcpSocket* pClientSocket = m_ptcpServer->nextPendingConnection();

    connect(pClientSocket, SIGNAL(disconnected()),
            pClientSocket, SLOT(deleteLater())
           );
    connect(pClientSocket, SIGNAL(readyRead()),
            this,          SLOT(slotReadClient())
           );
    connect(pClientSocket, SIGNAL(error(QAbstractSocket::SocketError)),
                    this,     SLOT(slotError(QAbstractSocket::SocketError)));

    qDebug()<<"Get request on connection from remote host wit IP: "<<pClientSocket->peerAddress().toString();
    if(clientConnected) {
      map_ConnectedStatus.insert(CONNECT_STATUS,"BUSY");
      qDebug()<<"The remote host can't connected. Server is busy of application with IP "<<pClientSocket->peerAddress().toString();
    }
    else {
      map_ConnectedStatus.insert(CONNECT_STATUS,"READY");
      clientConnected++;
    }
    sendToClient(pClientSocket,CONNECT_REQUEST);

    //after this another client can't set connection
}

//-------------------------------------------------------------------------------------------------
//--- error connection. May be lost
//-------------------------------------------------------------------------------------------------
void TTDBehav::slotError(QAbstractSocket::SocketError err)
{
    QString strError =
                    (err == QAbstractSocket::HostNotFoundError ?
                     tr("The host was not found.") :
                     err == QAbstractSocket::RemoteHostClosedError ?
                     tr("The remote host is closed.") :
                     err == QAbstractSocket::ConnectionRefusedError ?
                     tr("The connection was refused.") :
                     QString(m_ptcpServer->errorString())
                    );
    qDebug()<<strError;
    clientConnected--;
    if(clientConnected<0) clientConnected=0;
}

//-------------------------------------------------------------------------------------------------
//--- read from TCP socket
//-------------------------------------------------------------------------------------------------
void TTDBehav::slotReadClient()
{
  QTcpSocket* pClientSocket = (QTcpSocket*)sender();
  QDataStream in(pClientSocket);
  in.setVersion(QDataStream::Qt_4_4);
  QString request;

  for(;;) {
    if(!m_nNextBlockSize) {
      if(pClientSocket->bytesAvailable() < sizeof(quint32)) break;
        in >> m_nNextBlockSize;
    }

    if(pClientSocket->bytesAvailable() < m_nNextBlockSize) break;
    m_nNextBlockSize = 0;

    in >> request;
    if(!request.size()) break;

    qDebug()<<"--> "<<request;

//... Get status of device ......................................................
    if(request == STATUS_REQUEST) {
      pClient = pClientSocket;
      allStates[GET_STATUS_STATE]=GET_STATUS_STATE;
    }
 //... MEASURE .............................................................
   if(request == MEASURE_REQUEST) {
     pClient = pClientSocket;
     allStates[MEASURE_REQUEST_STATE]=MEASURE_REQUEST_STATE;
   }
   if(request == GETDATA_REQUEST) {
     in>>getDataFromAdc;
     pClient = pClientSocket;
     allStates[GET_DATA_STATE]=GET_DATA_STATE;
   }
   if(request == GETERROR_REQUEST) {
     pClient = pClientSocket;
     allStates[GER_ERROR_STATE]=GER_ERROR_STATE;
   }
 } // end for
}

//-------------------------------------------------------------------------------------------------
//--- answer to client
//-------------------------------------------------------------------------------------------------
void TTDBehav::sendToClient(QTcpSocket *pSocket, QString request)
{
  QByteArray  arrBlock;
  QDataStream out(&arrBlock, QIODevice::WriteOnly);
  out.setVersion(QDataStream::Qt_4_4);
  qDebug()<<"<-- "<<request;
  out << quint32(0) << request;

//---------------------------------------------------
//_1. CONNECT_REQUEST
  if(request ==CONNECT_REQUEST) out << map_ConnectedStatus;

//_2. STATUS_REQUEST
  if(request == STATUS_REQUEST) { out << map_MeasureStatus;  }

//_3. GETERROR_REQUEST
  if(request == GETERROR_REQUEST) out << map_ErrorStr;

//_4. MEASURE_Data
  if(request == GETDATA_REQUEST){ qDebug()<<map_AdcHeader; out << map_AdcHeader; out<<map_AdcData; }


//----------------------------------------------------

  out.device()->seek(0);
  out << quint32(arrBlock.size() - sizeof(quint32)); //place length of block for send
  pSocket->write(arrBlock);
}
