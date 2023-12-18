/****************************************************************************
** Meta object code from reading C++ file 'DigitaltwinsWidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.11.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../DigitaltwinsWidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'DigitaltwinsWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.11.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_DigitaltwinsWidget_t {
    QByteArrayData data[17];
    char stringdata0[295];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DigitaltwinsWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DigitaltwinsWidget_t qt_meta_stringdata_DigitaltwinsWidget = {
    {
QT_MOC_LITERAL(0, 0, 18), // "DigitaltwinsWidget"
QT_MOC_LITERAL(1, 19, 20), // "ClickButtonInitMatch"
QT_MOC_LITERAL(2, 40, 0), // ""
QT_MOC_LITERAL(3, 41, 21), // "ClickButtonCloseMatch"
QT_MOC_LITERAL(4, 63, 15), // "ShowReceivedMsg"
QT_MOC_LITERAL(5, 79, 25), // "ClickButtonUpdateVariable"
QT_MOC_LITERAL(6, 105, 19), // "ClickButtonOnlyShow"
QT_MOC_LITERAL(7, 125, 15), // "ClickButtonSave"
QT_MOC_LITERAL(8, 141, 12), // "slotTreeMenu"
QT_MOC_LITERAL(9, 154, 3), // "pos"
QT_MOC_LITERAL(10, 158, 14), // "MenuGeneCurves"
QT_MOC_LITERAL(11, 173, 19), // "ClickButtonNoUseNet"
QT_MOC_LITERAL(12, 193, 18), // "ClickButtonAddLine"
QT_MOC_LITERAL(13, 212, 18), // "ClickButtonDelLine"
QT_MOC_LITERAL(14, 231, 22), // "ClickButtonCheckConfig"
QT_MOC_LITERAL(15, 254, 22), // "currentItemChangedSLOT"
QT_MOC_LITERAL(16, 277, 17) // "QTableWidgetItem*"

    },
    "DigitaltwinsWidget\0ClickButtonInitMatch\0"
    "\0ClickButtonCloseMatch\0ShowReceivedMsg\0"
    "ClickButtonUpdateVariable\0ClickButtonOnlyShow\0"
    "ClickButtonSave\0slotTreeMenu\0pos\0"
    "MenuGeneCurves\0ClickButtonNoUseNet\0"
    "ClickButtonAddLine\0ClickButtonDelLine\0"
    "ClickButtonCheckConfig\0currentItemChangedSLOT\0"
    "QTableWidgetItem*"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DigitaltwinsWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   79,    2, 0x08 /* Private */,
       3,    0,   80,    2, 0x08 /* Private */,
       4,    0,   81,    2, 0x08 /* Private */,
       5,    0,   82,    2, 0x08 /* Private */,
       6,    0,   83,    2, 0x08 /* Private */,
       7,    0,   84,    2, 0x08 /* Private */,
       8,    1,   85,    2, 0x08 /* Private */,
      10,    0,   88,    2, 0x08 /* Private */,
      11,    0,   89,    2, 0x08 /* Private */,
      12,    0,   90,    2, 0x08 /* Private */,
      13,    0,   91,    2, 0x08 /* Private */,
      14,    0,   92,    2, 0x08 /* Private */,
      15,    2,   93,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QPoint,    9,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 16, 0x80000000 | 16,    2,    2,

       0        // eod
};

void DigitaltwinsWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        DigitaltwinsWidget *_t = static_cast<DigitaltwinsWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->ClickButtonInitMatch(); break;
        case 1: _t->ClickButtonCloseMatch(); break;
        case 2: _t->ShowReceivedMsg(); break;
        case 3: _t->ClickButtonUpdateVariable(); break;
        case 4: _t->ClickButtonOnlyShow(); break;
        case 5: _t->ClickButtonSave(); break;
        case 6: _t->slotTreeMenu((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 7: _t->MenuGeneCurves(); break;
        case 8: _t->ClickButtonNoUseNet(); break;
        case 9: _t->ClickButtonAddLine(); break;
        case 10: _t->ClickButtonDelLine(); break;
        case 11: _t->ClickButtonCheckConfig(); break;
        case 12: _t->currentItemChangedSLOT((*reinterpret_cast< QTableWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QTableWidgetItem*(*)>(_a[2]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject DigitaltwinsWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_DigitaltwinsWidget.data,
      qt_meta_data_DigitaltwinsWidget,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *DigitaltwinsWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DigitaltwinsWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_DigitaltwinsWidget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int DigitaltwinsWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 13;
    }
    return _id;
}
struct qt_meta_stringdata_DynamicCurveDisplayDlg_t {
    QByteArrayData data[4];
    char stringdata0[48];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DynamicCurveDisplayDlg_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DynamicCurveDisplayDlg_t qt_meta_stringdata_DynamicCurveDisplayDlg = {
    {
QT_MOC_LITERAL(0, 0, 22), // "DynamicCurveDisplayDlg"
QT_MOC_LITERAL(1, 23, 11), // "ParaChanged"
QT_MOC_LITERAL(2, 35, 0), // ""
QT_MOC_LITERAL(3, 36, 11) // "slotTimeout"

    },
    "DynamicCurveDisplayDlg\0ParaChanged\0\0"
    "slotTimeout"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DynamicCurveDisplayDlg[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   24,    2, 0x08 /* Private */,
       3,    0,   25,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void DynamicCurveDisplayDlg::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        DynamicCurveDisplayDlg *_t = static_cast<DynamicCurveDisplayDlg *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->ParaChanged(); break;
        case 1: _t->slotTimeout(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject DynamicCurveDisplayDlg::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_DynamicCurveDisplayDlg.data,
      qt_meta_data_DynamicCurveDisplayDlg,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *DynamicCurveDisplayDlg::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DynamicCurveDisplayDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_DynamicCurveDisplayDlg.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int DynamicCurveDisplayDlg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}
struct qt_meta_stringdata_DynamicChart_t {
    QByteArrayData data[1];
    char stringdata0[13];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DynamicChart_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DynamicChart_t qt_meta_stringdata_DynamicChart = {
    {
QT_MOC_LITERAL(0, 0, 12) // "DynamicChart"

    },
    "DynamicChart"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DynamicChart[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void DynamicChart::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject DynamicChart::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_DynamicChart.data,
      qt_meta_data_DynamicChart,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *DynamicChart::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DynamicChart::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_DynamicChart.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int DynamicChart::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
