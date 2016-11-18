/****************************************************************************
** Meta object code from reading C++ file 'spectrumwidget.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../inc/Widgets/spectrumwidget.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'spectrumwidget.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_spectrumWidget_t {
    QByteArrayData data[12];
    char stringdata0[119];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_spectrumWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_spectrumWidget_t qt_meta_stringdata_spectrumWidget = {
    {
QT_MOC_LITERAL(0, 0, 14), // "spectrumWidget"
QT_MOC_LITERAL(1, 15, 7), // "realFFT"
QT_MOC_LITERAL(2, 23, 0), // ""
QT_MOC_LITERAL(3, 24, 11), // "QByteArray*"
QT_MOC_LITERAL(4, 36, 7), // "iBuffer"
QT_MOC_LITERAL(5, 44, 16), // "slotMaxDbChanged"
QT_MOC_LITERAL(6, 61, 2), // "mb"
QT_MOC_LITERAL(7, 64, 16), // "slotRangeChanged"
QT_MOC_LITERAL(8, 81, 2), // "rg"
QT_MOC_LITERAL(9, 84, 9), // "getParams"
QT_MOC_LITERAL(10, 94, 9), // "setParams"
QT_MOC_LITERAL(11, 104, 14) // "slotAvgChanged"

    },
    "spectrumWidget\0realFFT\0\0QByteArray*\0"
    "iBuffer\0slotMaxDbChanged\0mb\0"
    "slotRangeChanged\0rg\0getParams\0setParams\0"
    "slotAvgChanged"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_spectrumWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   44,    2, 0x0a /* Public */,
       5,    1,   47,    2, 0x08 /* Private */,
       7,    1,   50,    2, 0x08 /* Private */,
       9,    0,   53,    2, 0x08 /* Private */,
      10,    0,   54,    2, 0x08 /* Private */,
      11,    1,   55,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, QMetaType::Int,    6,
    QMetaType::Void, QMetaType::Int,    8,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Double,    2,

       0        // eod
};

void spectrumWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        spectrumWidget *_t = static_cast<spectrumWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->realFFT((*reinterpret_cast< QByteArray*(*)>(_a[1]))); break;
        case 1: _t->slotMaxDbChanged((*reinterpret_cast< qint32(*)>(_a[1]))); break;
        case 2: _t->slotRangeChanged((*reinterpret_cast< qint32(*)>(_a[1]))); break;
        case 3: _t->getParams(); break;
        case 4: _t->setParams(); break;
        case 5: _t->slotAvgChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject spectrumWidget::staticMetaObject = {
    { &QFrame::staticMetaObject, qt_meta_stringdata_spectrumWidget.data,
      qt_meta_data_spectrumWidget,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *spectrumWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *spectrumWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_spectrumWidget.stringdata0))
        return static_cast<void*>(const_cast< spectrumWidget*>(this));
    return QFrame::qt_metacast(_clname);
}

int spectrumWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFrame::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
