/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.11.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../mainwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.11.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[59];
    char stringdata0[637];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "MainWindow"
QT_MOC_LITERAL(1, 11, 6), // "newDoc"
QT_MOC_LITERAL(2, 18, 0), // ""
QT_MOC_LITERAL(3, 19, 4), // "open"
QT_MOC_LITERAL(4, 24, 5), // "Close"
QT_MOC_LITERAL(5, 30, 4), // "save"
QT_MOC_LITERAL(6, 35, 6), // "saveAs"
QT_MOC_LITERAL(7, 42, 6), // "import"
QT_MOC_LITERAL(8, 49, 23), // "updateRecentFileActions"
QT_MOC_LITERAL(9, 73, 14), // "openRecentFile"
QT_MOC_LITERAL(10, 88, 4), // "undo"
QT_MOC_LITERAL(11, 93, 4), // "redo"
QT_MOC_LITERAL(12, 98, 12), // "selectVertex"
QT_MOC_LITERAL(13, 111, 10), // "selectEdge"
QT_MOC_LITERAL(14, 122, 10), // "selectFace"
QT_MOC_LITERAL(15, 133, 10), // "selectBody"
QT_MOC_LITERAL(16, 144, 15), // "selectConnector"
QT_MOC_LITERAL(17, 160, 9), // "selectAny"
QT_MOC_LITERAL(18, 170, 14), // "DeleteSelected"
QT_MOC_LITERAL(19, 185, 7), // "Scaling"
QT_MOC_LITERAL(20, 193, 11), // "connScaling"
QT_MOC_LITERAL(21, 205, 7), // "Texture"
QT_MOC_LITERAL(22, 213, 15), // "showConsDockwin"
QT_MOC_LITERAL(23, 229, 13), // "regenGeometry"
QT_MOC_LITERAL(24, 243, 16), // "showCyberDockwin"
QT_MOC_LITERAL(25, 260, 11), // "addMachConn"
QT_MOC_LITERAL(26, 272, 11), // "addElecConn"
QT_MOC_LITERAL(27, 284, 11), // "addHydrConn"
QT_MOC_LITERAL(28, 296, 11), // "addContConn"
QT_MOC_LITERAL(29, 308, 10), // "defConnDlg"
QT_MOC_LITERAL(30, 319, 17), // "addMarkingSurfDlg"
QT_MOC_LITERAL(31, 337, 13), // "defDatumPoint"
QT_MOC_LITERAL(32, 351, 12), // "defDatumLine"
QT_MOC_LITERAL(33, 364, 9), // "systemSet"
QT_MOC_LITERAL(34, 374, 8), // "ShowOxyz"
QT_MOC_LITERAL(35, 383, 5), // "about"
QT_MOC_LITERAL(36, 389, 11), // "updateMenus"
QT_MOC_LITERAL(37, 401, 16), // "updateWindowMenu"
QT_MOC_LITERAL(38, 418, 8), // "SetColor"
QT_MOC_LITERAL(39, 427, 11), // "SetMaterial"
QT_MOC_LITERAL(40, 439, 10), // "undoEnable"
QT_MOC_LITERAL(41, 450, 1), // "b"
QT_MOC_LITERAL(42, 452, 6), // "insert"
QT_MOC_LITERAL(43, 459, 16), // "attributeSetting"
QT_MOC_LITERAL(44, 476, 8), // "assembly"
QT_MOC_LITERAL(45, 485, 6), // "adjust"
QT_MOC_LITERAL(46, 492, 7), // "setpara"
QT_MOC_LITERAL(47, 500, 11), // "setsimulate"
QT_MOC_LITERAL(48, 512, 10), // "connection"
QT_MOC_LITERAL(49, 523, 10), // "modelCheck"
QT_MOC_LITERAL(50, 534, 10), // "simulation"
QT_MOC_LITERAL(51, 545, 12), // "digitaltwins"
QT_MOC_LITERAL(52, 558, 13), // "neuralnetwork"
QT_MOC_LITERAL(53, 572, 9), // "animation"
QT_MOC_LITERAL(54, 582, 15), // "OnStatusChanged"
QT_MOC_LITERAL(55, 598, 7), // "MakeBox"
QT_MOC_LITERAL(56, 606, 10), // "MakeSpring"
QT_MOC_LITERAL(57, 617, 10), // "ExportBREP"
QT_MOC_LITERAL(58, 628, 8) // "repaint0"

    },
    "MainWindow\0newDoc\0\0open\0Close\0save\0"
    "saveAs\0import\0updateRecentFileActions\0"
    "openRecentFile\0undo\0redo\0selectVertex\0"
    "selectEdge\0selectFace\0selectBody\0"
    "selectConnector\0selectAny\0DeleteSelected\0"
    "Scaling\0connScaling\0Texture\0showConsDockwin\0"
    "regenGeometry\0showCyberDockwin\0"
    "addMachConn\0addElecConn\0addHydrConn\0"
    "addContConn\0defConnDlg\0addMarkingSurfDlg\0"
    "defDatumPoint\0defDatumLine\0systemSet\0"
    "ShowOxyz\0about\0updateMenus\0updateWindowMenu\0"
    "SetColor\0SetMaterial\0undoEnable\0b\0"
    "insert\0attributeSetting\0assembly\0"
    "adjust\0setpara\0setsimulate\0connection\0"
    "modelCheck\0simulation\0digitaltwins\0"
    "neuralnetwork\0animation\0OnStatusChanged\0"
    "MakeBox\0MakeSpring\0ExportBREP\0repaint0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      56,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,  294,    2, 0x08 /* Private */,
       3,    0,  295,    2, 0x08 /* Private */,
       4,    0,  296,    2, 0x08 /* Private */,
       5,    0,  297,    2, 0x08 /* Private */,
       6,    0,  298,    2, 0x08 /* Private */,
       7,    0,  299,    2, 0x08 /* Private */,
       8,    0,  300,    2, 0x08 /* Private */,
       9,    0,  301,    2, 0x08 /* Private */,
      10,    0,  302,    2, 0x08 /* Private */,
      11,    0,  303,    2, 0x08 /* Private */,
      12,    0,  304,    2, 0x08 /* Private */,
      13,    0,  305,    2, 0x08 /* Private */,
      14,    0,  306,    2, 0x08 /* Private */,
      15,    0,  307,    2, 0x08 /* Private */,
      16,    0,  308,    2, 0x08 /* Private */,
      17,    0,  309,    2, 0x08 /* Private */,
      18,    0,  310,    2, 0x08 /* Private */,
      19,    0,  311,    2, 0x08 /* Private */,
      20,    0,  312,    2, 0x08 /* Private */,
      21,    0,  313,    2, 0x08 /* Private */,
      22,    0,  314,    2, 0x08 /* Private */,
      23,    0,  315,    2, 0x08 /* Private */,
      24,    0,  316,    2, 0x08 /* Private */,
      25,    0,  317,    2, 0x08 /* Private */,
      26,    0,  318,    2, 0x08 /* Private */,
      27,    0,  319,    2, 0x08 /* Private */,
      28,    0,  320,    2, 0x08 /* Private */,
      29,    0,  321,    2, 0x08 /* Private */,
      30,    0,  322,    2, 0x08 /* Private */,
      31,    0,  323,    2, 0x08 /* Private */,
      32,    0,  324,    2, 0x08 /* Private */,
      33,    0,  325,    2, 0x08 /* Private */,
      34,    0,  326,    2, 0x08 /* Private */,
      35,    0,  327,    2, 0x08 /* Private */,
      36,    0,  328,    2, 0x08 /* Private */,
      37,    0,  329,    2, 0x08 /* Private */,
      38,    0,  330,    2, 0x08 /* Private */,
      39,    0,  331,    2, 0x08 /* Private */,
      40,    1,  332,    2, 0x08 /* Private */,
      42,    0,  335,    2, 0x08 /* Private */,
      43,    0,  336,    2, 0x08 /* Private */,
      44,    0,  337,    2, 0x08 /* Private */,
      45,    0,  338,    2, 0x08 /* Private */,
      46,    0,  339,    2, 0x08 /* Private */,
      47,    0,  340,    2, 0x08 /* Private */,
      48,    0,  341,    2, 0x08 /* Private */,
      49,    0,  342,    2, 0x08 /* Private */,
      50,    0,  343,    2, 0x08 /* Private */,
      51,    0,  344,    2, 0x08 /* Private */,
      52,    0,  345,    2, 0x08 /* Private */,
      53,    0,  346,    2, 0x08 /* Private */,
      54,    1,  347,    2, 0x08 /* Private */,
      55,    0,  350,    2, 0x08 /* Private */,
      56,    0,  351,    2, 0x08 /* Private */,
      57,    0,  352,    2, 0x08 /* Private */,
      58,    0,  353,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   41,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MainWindow *_t = static_cast<MainWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->newDoc(); break;
        case 1: _t->open(); break;
        case 2: _t->Close(); break;
        case 3: _t->save(); break;
        case 4: _t->saveAs(); break;
        case 5: _t->import(); break;
        case 6: _t->updateRecentFileActions(); break;
        case 7: _t->openRecentFile(); break;
        case 8: _t->undo(); break;
        case 9: _t->redo(); break;
        case 10: _t->selectVertex(); break;
        case 11: _t->selectEdge(); break;
        case 12: _t->selectFace(); break;
        case 13: _t->selectBody(); break;
        case 14: _t->selectConnector(); break;
        case 15: _t->selectAny(); break;
        case 16: _t->DeleteSelected(); break;
        case 17: _t->Scaling(); break;
        case 18: _t->connScaling(); break;
        case 19: _t->Texture(); break;
        case 20: _t->showConsDockwin(); break;
        case 21: _t->regenGeometry(); break;
        case 22: _t->showCyberDockwin(); break;
        case 23: _t->addMachConn(); break;
        case 24: _t->addElecConn(); break;
        case 25: _t->addHydrConn(); break;
        case 26: _t->addContConn(); break;
        case 27: _t->defConnDlg(); break;
        case 28: _t->addMarkingSurfDlg(); break;
        case 29: _t->defDatumPoint(); break;
        case 30: _t->defDatumLine(); break;
        case 31: _t->systemSet(); break;
        case 32: _t->ShowOxyz(); break;
        case 33: _t->about(); break;
        case 34: _t->updateMenus(); break;
        case 35: _t->updateWindowMenu(); break;
        case 36: _t->SetColor(); break;
        case 37: _t->SetMaterial(); break;
        case 38: _t->undoEnable((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 39: _t->insert(); break;
        case 40: _t->attributeSetting(); break;
        case 41: _t->assembly(); break;
        case 42: _t->adjust(); break;
        case 43: _t->setpara(); break;
        case 44: _t->setsimulate(); break;
        case 45: _t->connection(); break;
        case 46: _t->modelCheck(); break;
        case 47: _t->simulation(); break;
        case 48: _t->digitaltwins(); break;
        case 49: _t->neuralnetwork(); break;
        case 50: _t->animation(); break;
        case 51: _t->OnStatusChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 52: _t->MakeBox(); break;
        case 53: _t->MakeSpring(); break;
        case 54: _t->ExportBREP(); break;
        case 55: _t->repaint0(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow.data,
      qt_meta_data_MainWindow,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 56)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 56;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 56)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 56;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
