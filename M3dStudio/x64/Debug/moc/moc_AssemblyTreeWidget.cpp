/****************************************************************************
** Meta object code from reading C++ file 'AssemblyTreeWidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.11.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../AssemblyTreeWidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AssemblyTreeWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.11.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_AssemblyTreeWidget_t {
    QByteArrayData data[10];
    char stringdata0[134];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_AssemblyTreeWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_AssemblyTreeWidget_t qt_meta_stringdata_AssemblyTreeWidget = {
    {
QT_MOC_LITERAL(0, 0, 18), // "AssemblyTreeWidget"
QT_MOC_LITERAL(1, 19, 15), // "deleteComponent"
QT_MOC_LITERAL(2, 35, 0), // ""
QT_MOC_LITERAL(3, 36, 15), // "renameComponent"
QT_MOC_LITERAL(4, 52, 18), // "repostureComponent"
QT_MOC_LITERAL(5, 71, 18), // "curItemChangedSlot"
QT_MOC_LITERAL(6, 90, 16), // "QTreeWidgetItem*"
QT_MOC_LITERAL(7, 107, 7), // "current"
QT_MOC_LITERAL(8, 115, 8), // "previous"
QT_MOC_LITERAL(9, 124, 9) // "onTimeout"

    },
    "AssemblyTreeWidget\0deleteComponent\0\0"
    "renameComponent\0repostureComponent\0"
    "curItemChangedSlot\0QTreeWidgetItem*\0"
    "current\0previous\0onTimeout"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_AssemblyTreeWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   39,    2, 0x08 /* Private */,
       3,    0,   40,    2, 0x08 /* Private */,
       4,    0,   41,    2, 0x08 /* Private */,
       5,    2,   42,    2, 0x08 /* Private */,
       9,    0,   47,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 6, 0x80000000 | 6,    7,    8,
    QMetaType::Void,

       0        // eod
};

void AssemblyTreeWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        AssemblyTreeWidget *_t = static_cast<AssemblyTreeWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->deleteComponent(); break;
        case 1: _t->renameComponent(); break;
        case 2: _t->repostureComponent(); break;
        case 3: _t->curItemChangedSlot((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QTreeWidgetItem*(*)>(_a[2]))); break;
        case 4: _t->onTimeout(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject AssemblyTreeWidget::staticMetaObject = {
    { &QTreeWidget::staticMetaObject, qt_meta_stringdata_AssemblyTreeWidget.data,
      qt_meta_data_AssemblyTreeWidget,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *AssemblyTreeWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AssemblyTreeWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_AssemblyTreeWidget.stringdata0))
        return static_cast<void*>(this);
    return QTreeWidget::qt_metacast(_clname);
}

int AssemblyTreeWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTreeWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
