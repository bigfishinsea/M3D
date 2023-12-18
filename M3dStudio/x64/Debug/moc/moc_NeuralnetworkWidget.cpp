/****************************************************************************
** Meta object code from reading C++ file 'NeuralnetworkWidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.11.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../NeuralnetworkWidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'NeuralnetworkWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.11.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_NeuralnetworkWidget_t {
    QByteArrayData data[8];
    char stringdata0[139];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_NeuralnetworkWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_NeuralnetworkWidget_t qt_meta_stringdata_NeuralnetworkWidget = {
    {
QT_MOC_LITERAL(0, 0, 19), // "NeuralnetworkWidget"
QT_MOC_LITERAL(1, 20, 21), // "ClickButtonSelCsvPath"
QT_MOC_LITERAL(2, 42, 0), // ""
QT_MOC_LITERAL(3, 43, 18), // "ClickButtonReadCsv"
QT_MOC_LITERAL(4, 62, 15), // "ClickButtonSave"
QT_MOC_LITERAL(5, 78, 22), // "ClickButtonGenTrainSet"
QT_MOC_LITERAL(6, 101, 18), // "ClickButtonGenFANN"
QT_MOC_LITERAL(7, 120, 18) // "ClickButtonGenPyNN"

    },
    "NeuralnetworkWidget\0ClickButtonSelCsvPath\0"
    "\0ClickButtonReadCsv\0ClickButtonSave\0"
    "ClickButtonGenTrainSet\0ClickButtonGenFANN\0"
    "ClickButtonGenPyNN"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_NeuralnetworkWidget[] = {

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
       1,    0,   44,    2, 0x08 /* Private */,
       3,    0,   45,    2, 0x08 /* Private */,
       4,    0,   46,    2, 0x08 /* Private */,
       5,    0,   47,    2, 0x08 /* Private */,
       6,    0,   48,    2, 0x08 /* Private */,
       7,    0,   49,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void NeuralnetworkWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        NeuralnetworkWidget *_t = static_cast<NeuralnetworkWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->ClickButtonSelCsvPath(); break;
        case 1: _t->ClickButtonReadCsv(); break;
        case 2: _t->ClickButtonSave(); break;
        case 3: _t->ClickButtonGenTrainSet(); break;
        case 4: _t->ClickButtonGenFANN(); break;
        case 5: _t->ClickButtonGenPyNN(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject NeuralnetworkWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_NeuralnetworkWidget.data,
      qt_meta_data_NeuralnetworkWidget,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *NeuralnetworkWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *NeuralnetworkWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_NeuralnetworkWidget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int NeuralnetworkWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_CheckBoxDelegateNetwork_t {
    QByteArrayData data[1];
    char stringdata0[24];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CheckBoxDelegateNetwork_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CheckBoxDelegateNetwork_t qt_meta_stringdata_CheckBoxDelegateNetwork = {
    {
QT_MOC_LITERAL(0, 0, 23) // "CheckBoxDelegateNetwork"

    },
    "CheckBoxDelegateNetwork"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CheckBoxDelegateNetwork[] = {

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

void CheckBoxDelegateNetwork::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject CheckBoxDelegateNetwork::staticMetaObject = {
    { &QStyledItemDelegate::staticMetaObject, qt_meta_stringdata_CheckBoxDelegateNetwork.data,
      qt_meta_data_CheckBoxDelegateNetwork,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *CheckBoxDelegateNetwork::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CheckBoxDelegateNetwork::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CheckBoxDelegateNetwork.stringdata0))
        return static_cast<void*>(this);
    return QStyledItemDelegate::qt_metacast(_clname);
}

int CheckBoxDelegateNetwork::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QStyledItemDelegate::qt_metacall(_c, _id, _a);
    return _id;
}
struct qt_meta_stringdata_CheckBoxDelegateControl_t {
    QByteArrayData data[1];
    char stringdata0[24];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CheckBoxDelegateControl_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CheckBoxDelegateControl_t qt_meta_stringdata_CheckBoxDelegateControl = {
    {
QT_MOC_LITERAL(0, 0, 23) // "CheckBoxDelegateControl"

    },
    "CheckBoxDelegateControl"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CheckBoxDelegateControl[] = {

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

void CheckBoxDelegateControl::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject CheckBoxDelegateControl::staticMetaObject = {
    { &QStyledItemDelegate::staticMetaObject, qt_meta_stringdata_CheckBoxDelegateControl.data,
      qt_meta_data_CheckBoxDelegateControl,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *CheckBoxDelegateControl::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CheckBoxDelegateControl::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CheckBoxDelegateControl.stringdata0))
        return static_cast<void*>(this);
    return QStyledItemDelegate::qt_metacast(_clname);
}

int CheckBoxDelegateControl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QStyledItemDelegate::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
