#ifndef ORBBEHAV_H
#define ORBBEHAV_H

#include <QtCore>
#include <QTcpServer>
#include <QTcpSocket>
#include <QAbstractSocket>

#include <syslog.h>

#include "hw.h"
#include "request_dev.h"

typedef enum
{
  MEASURE_REQUEST_STATE,
  END_MEASURE_STATE,
  GET_STATUS_STATE,
  GET_DATA_STATE,
  GER_ERROR_STATE,
  ALLREQSTATES,
  IDLE
} CPhase;  // phases of state machine


class THw;
class TTDBehav : public QThread
{
  Q_OBJECT
public:
  TTDBehav();
  ~TTDBehav();
  void setAbort(bool a) { abort=a; }
  void createAdcData(int); // get mease from ADCs and sort data according axis and time points

  THw *device;

public slots:
  void endMeasure(void);
protected:
  void run();
private:
  CPhase phase;
  CPhase allStates[ALLREQSTATES];
  bool abort;
  QMutex mutex;

//.. NetWork ...
  int clientConnected;
  QTcpServer  *m_ptcpServer;
  quint32  m_nNextBlockSize;
  void sendToClient(QTcpSocket* pSocket, QString request);
  QTcpSocket *pClient;

  QMap<QString,QString> map_ErrorStr;
  QMap<QString,QString> map_ConnectedStatus;   // status of connected
  QMap<uint32_t,double> map_AdcData;
  QMap<QString,uint32_t> map_AdcHeader;
  QMap<QString,uint32_t> map_MeasureStatus;    // progress of measure and error
  uint32_t getDataFromAdc; // number adc 0..11 for get data

// network strings processing
private slots:
  virtual void slotNewConnection();
  void slotReadClient();
  void slotError(QAbstractSocket::SocketError err);
};

#endif // ORBBEHAV_H
