/****************************************************************************
** Meta object code from reading C++ file 'RepostureDlg.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.11.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../RepostureDlg.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'RepostureDlg.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.11.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_reposturedialog_t {
    QByteArrayData data[7];
    char stringdata0[124];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_reposturedialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_reposturedialog_t qt_meta_stringdata_reposturedialog = {
    {
QT_MOC_LITERAL(0, 0, 15), // "reposturedialog"
QT_MOC_LITERAL(1, 16, 24), // "On_LineEdit_ValidChanged"
QT_MOC_LITERAL(2, 41, 0), // ""
QT_MOC_LITERAL(3, 42, 30), // "My_DoubleSpinBox_ValidChanged1"
QT_MOC_LITERAL(4, 73, 3), // "val"
QT_MOC_LITERAL(5, 77, 21), // "On_savebutton_clicked"
QT_MOC_LITERAL(6, 99, 24) // "On_restorebutton_clicked"

    },
    "reposturedialog\0On_LineEdit_ValidChanged\0"
    "\0My_DoubleSpinBox_ValidChanged1\0val\0"
    "On_savebutton_clicked\0On_restorebutton_clicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_reposturedialog[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x08 /* Private */,
       3,    1,   37,    2, 0x08 /* Private */,
       5,    0,   40,    2, 0x08 /* Private */,
       6,    0,   41,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::Double,    4,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void reposturedialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        reposturedialog *_t = static_cast<reposturedialog *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->On_LineEdit_ValidChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->My_DoubleSpinBox_ValidChanged1((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 2: _t->On_savebutton_clicked(); break;
        case 3: _t->On_restorebutton_clicked(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject reposturedialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_reposturedialog.data,
      qt_meta_data_reposturedialog,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *reposturedialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *reposturedialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_reposturedialog.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int reposturedialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
