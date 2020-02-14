#ifndef ORBBEHAV_H
#define ORBBEHAV_H

#include <QtCore>
#include <QVector>

#include <syslog.h>

#include "progerror.h"
#include "hw.h"

#define SAMPLE_DEVICE 100 // time of request device
#define allAxis 10
typedef enum
{
  INITIAL_STATE,
  DEVICE_ERROR_STATE,
  DATA_READY_STATE,
  UPDATE_STATUS_STATE,
  GET_DATA_STATE,
  ALLREQSTATES,
  IDLE
} CPhase;  // phases of state machine


class THw;
class TTDBehav : public QThread,THw
{
  Q_OBJECT
public:
  TTDBehav();
  ~TTDBehav();
  void setAbort(bool a) { abort=a; }
  void createAdcData(int); // get mease from ADCs and sort data according axis and time points

  void initialDevice(void);


  int readCA(QString);

  void readMeasure(double orbitaR[][16]); //read data to DB
public slots:

  int getDataRequest(void);
protected:
  void run();
private:
  CPhase phase;
  CPhase allStates[ALLREQSTATES];
  int stErr; // errors of work with hw
  TDevErrors devError;  // hardware error in bits represantation

  bool abort;

  QString workDir;
  QMutex mutex;
  int wrFlag,dataReady,hwReady;


};

#endif // ORBBEHAV_H
