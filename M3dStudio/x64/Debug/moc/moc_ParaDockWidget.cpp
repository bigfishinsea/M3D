/****************************************************************************
** Meta object code from reading C++ file 'ParaDockWidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.11.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../ParaDockWidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ParaDockWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.11.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ParaDockWidget_t {
    QByteArrayData data[12];
    char stringdata0[144];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ParaDockWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ParaDockWidget_t qt_meta_stringdata_ParaDockWidget = {
    {
QT_MOC_LITERAL(0, 0, 14), // "ParaDockWidget"
QT_MOC_LITERAL(1, 15, 22), // "currentItemChangedSLOT"
QT_MOC_LITERAL(2, 38, 0), // ""
QT_MOC_LITERAL(3, 39, 17), // "QTableWidgetItem*"
QT_MOC_LITERAL(4, 57, 7), // "addline"
QT_MOC_LITERAL(5, 65, 7), // "delline"
QT_MOC_LITERAL(6, 73, 5), // "close"
QT_MOC_LITERAL(7, 79, 13), // "readParatable"
QT_MOC_LITERAL(8, 93, 1), // "b"
QT_MOC_LITERAL(9, 95, 13), // "saveParatable"
QT_MOC_LITERAL(10, 109, 13), // "LoadParatable"
QT_MOC_LITERAL(11, 123, 20) // "autoComputeParatable"

    },
    "ParaDockWidget\0currentItemChangedSLOT\0"
    "\0QTableWidgetItem*\0addline\0delline\0"
    "close\0readParatable\0b\0saveParatable\0"
    "LoadParatable\0autoComputeParatable"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ParaDockWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    2,   59,    2, 0x08 /* Private */,
       4,    0,   64,    2, 0x08 /* Private */,
       5,    0,   65,    2, 0x08 /* Private */,
       6,    0,   66,    2, 0x08 /* Private */,
       7,    1,   67,    2, 0x08 /* Private */,
       7,    0,   70,    2, 0x28 /* Private | MethodCloned */,
       9,    0,   71,    2, 0x08 /* Private */,
      10,    0,   72,    2, 0x08 /* Private */,
      11,    0,   73,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 3,    2,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    8,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void ParaDockWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ParaDockWidget *_t = static_cast<ParaDockWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->currentItemChangedSLOT((*reinterpret_cast< QTableWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QTableWidgetItem*(*)>(_a[2]))); break;
        case 1: _t->addline(); break;
        case 2: _t->delline(); break;
        case 3: _t->close(); break;
        case 4: _t->readParatable((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: _t->readParatable(); break;
        case 6: _t->saveParatable(); break;
        case 7: _t->LoadParatable(); break;
        case 8: _t->autoComputeParatable(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject ParaDockWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_ParaDockWidget.data,
      qt_meta_data_ParaDockWidget,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *ParaDockWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ParaDockWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ParaDockWidget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int ParaDockWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 9;
    }
    return _id;
}
struct qt_meta_stringdata_ListDelegate_t {
    QByteArrayData data[1];
    char stringdata0[13];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ListDelegate_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ListDelegate_t qt_meta_stringdata_ListDelegate = {
    {
QT_MOC_LITERAL(0, 0, 12) // "ListDelegate"

    },
    "ListDelegate"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ListDelegate[] = {

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

void ListDelegate::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject ListDelegate::staticMetaObject = {
    { &QStyledItemDelegate::staticMetaObject, qt_meta_stringdata_ListDelegate.data,
      qt_meta_data_ListDelegate,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *ListDelegate::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ListDelegate::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ListDelegate.stringdata0))
        return static_cast<void*>(this);
    return QStyledItemDelegate::qt_metacast(_clname);
}

int ListDelegate::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QStyledItemDelegate::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
