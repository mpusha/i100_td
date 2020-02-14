/****************************************************************************
** Meta object code from reading C++ file 'srvBehav.h'
**
** Created: Fri Feb 14 12:49:57 2020
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../v1_1/srvBehav.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'srvBehav.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_TTDBehav[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      10,    9,    9,    9, 0x0a,
      23,    9,    9,    9, 0x08,
      43,    9,    9,    9, 0x08,
      64,   60,    9,    9, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_TTDBehav[] = {
    "TTDBehav\0\0endMeasure()\0slotNewConnection()\0"
    "slotReadClient()\0err\0"
    "slotError(QAbstractSocket::SocketError)\0"
};

const QMetaObject TTDBehav::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_TTDBehav,
      qt_meta_data_TTDBehav, 0 }
};

const QMetaObject *TTDBehav::metaObject() const
{
    return &staticMetaObject;
}

void *TTDBehav::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_TTDBehav))
        return static_cast<void*>(const_cast< TTDBehav*>(this));
    return QThread::qt_metacast(_clname);
}

int TTDBehav::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: endMeasure(); break;
        case 1: slotNewConnection(); break;
        case 2: slotReadClient(); break;
        case 3: slotError((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1]))); break;
        }
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
