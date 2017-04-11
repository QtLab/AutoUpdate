#include "widget.h"
#include <QApplication>
#include <QMutex>
/*!
    日志输出功能
*/
void crashingMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    static QMutex mutex;
        mutex.lock();

        QString text;
        switch(type)
        {
        case QtDebugMsg:
            text = QString("Debug:");
            break;

        case QtWarningMsg:
            text = QString("Warning:");
            break;

        case QtCriticalMsg:
            text = QString("Critical:");
            break;

        case QtFatalMsg:
            text = QString("Fatal:");
        }

        QString context_info = QString("File:(%1) Line:(%2)").arg(QString(context.file)).arg(context.line);
        QString current_date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd");
        QString current_date = QString("(%1)").arg(current_date_time);
        QString message = QString("%1: %2 %3-> %4").arg(current_date).arg(text).arg(context_info).arg(msg);

        QFile file("logAutoUpdate.txt");
        file.open(QIODevice::WriteOnly | QIODevice::Append);
        QTextStream text_stream(&file);
        text_stream << message << "\r\n";
        file.flush();
        file.close();

        mutex.unlock();
}


int main(int argc, char *argv[])
{
    //日志输出
    qInstallMessageHandler(crashingMessageHandler);
    QApplication a(argc, argv);
    Widget w;
    QString dir = QString(argv[1]);
    dir.replace(QString("#"),QString(" "));
    w.setUpdateDir(dir);
    qDebug()<<"a1:"<<QString(dir);
    qDebug()<<"a2:"<<QString(argv[2]);
    qDebug()<<"downloadURL:"<<QString(argv[3]);
    qDebug() << "exec EXE:" << QString(argv[4]);
    int isNeedUpdate = QString(argv[2]).toInt();
    QString url = QString(argv[3]);
    w.setUrl(url);
    w.setNeedUpdate(isNeedUpdate);
    w.setExecTargetName(QString(argv[4]));
    //测试
//    qDebug() << "dest Path:"<< QApplication::applicationDirPath();
//    w.setUpdateDir(QApplication::applicationDirPath());
//    w.setUrl("http://app.thunics.org/static/code/smartbow-ma/1.02--1.03.zip");
//    w.setNeedUpdate(1);
//    w.setExecTargetName("SensorMonitor.exe");

    w.show();
    QFile style(":/qss/Auto.qss");
    if (!style.open(QIODevice::ReadOnly))
    {
        qWarning("Can't open the style sheet file.");
    }
    a.setStyleSheet(style.readAll());
    style.close();
    return a.exec();
}
