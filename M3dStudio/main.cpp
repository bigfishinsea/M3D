#include "M3d.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <OSD_Environment.hxx>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(M3d);

    //加载插件，否则导致App启动失败
    TCollection_AsciiString aPlugindsDirName = OSD_Environment("QTDIR").Value();
    if (!aPlugindsDirName.IsEmpty())
        QApplication::addLibraryPath(QString(aPlugindsDirName.ToCString()) + "/plugins");

    QApplication app(argc, argv);
    QCoreApplication::setApplicationName("M3dApp");
    QCoreApplication::setOrganizationName("M3dProject");
    QCoreApplication::setApplicationVersion("0.1");
    QCommandLineParser parser;
    parser.setApplicationDescription("M3d主程序");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("file", "The file to open.");
    //parser.addPositionalArgument("new", "new file.");
       //通过QCommandLineOption类定义可能的命令行选项

    QCommandLineOption newOption(QStringList() << "new" << "-1", "new document", "1");
    parser.addOption(newOption);
    QCommandLineOption openOption(QStringList() << "open" << "*.m3dcom", "open file","t1.m3dcom");
    parser.addOption(openOption);

    parser.process(app);
    //trying to get the arguments into a list    
    QStringList cmdline_args = QCoreApplication::arguments();

    MainWindow mainWin;
    ////如果带参数运行，则打开文件
    //foreach(const QString & fileName, parser.positionalArguments())
    //    mainWin.openFile(fileName);
    mainWin.show();
    //解析命令行
    QString strNewValue = parser.value("new");
    QString strOpenValue = parser.value("open");

    //new doc
    if(strNewValue == "1")
    {
        mainWin.newFile(1);
    } else if (strNewValue == "2")
    {
        mainWin.newFile(2);
    }

    //open file
    if (!strOpenValue.isEmpty())
    {
        mainWin.openFile(strOpenValue);
    }

    return app.exec();
}
