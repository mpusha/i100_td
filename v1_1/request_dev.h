#ifndef REQUEST_DEV_H
#define REQUEST_DEV_H

//........................MEASURE_REQUEST..................................................
#define MEASURE_REQUEST     "MEASURE"

//........................GETDATA_REQUEST..................................................
#define GETDATA_REQUEST     "GETDATA"
#define GETDATA_NUMADC      "numadc"      // number of ADC from 0
#define GETDATA_ENADC       "enableadc"   // enable ADC  (0-disable)
#define GETDATA_ALLADC      "allmodules"  // all adc in system (12)
#define GETDATA_POINTS      "allpoints"   // all points (200)
#define GETDATA_ALLCH       "allchanels"  // all chanels on ADC (16)
#define GETDATA_WORKCH      "workchanel"  // all work chanell ADC from 0 to workchanel
#define GETDATA_STEP        "timestep"    // point to point step in ns (6400)

//............................STATUS_REQUEST...................................................
#define STATUS_REQUEST       "GETSTATUS"
#define STATUS_PROGRESS      "getprogress"
#define STATUS_ERROR         "geterrorint"

//............................GET_REQUEST...................................................
#define GETERROR_REQUEST     "GETERRORO"
#define GETERROR_STR         "geterrorstr"

//............................CONNECT_REQUEST...................................................
#define CONNECT_REQUEST       "ISCONNECTED"
#define CONNECT_STATUS        "status" // can be READY or BUSY
#endif // REQUEST_DEV_H

