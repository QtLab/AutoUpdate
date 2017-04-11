#include "widget.h"
#include "ui_widget.h"
#include <QMessageBox>
#include <QMouseEvent>
#include <windows.h>
#include <windowsx.h>
#include <QPainter>
#include <QJsonDocument>
#include <QJsonObject>
#include<QJsonArray>
#include <QProcess>
#include<tlhelp32.h>
#include <tchar.h>
#include <QTextCodec>
#include <JlCompress.h>
#include "shellapi.h"

BOOL FindAndKillProcessByName(LPCTSTR strProcessName)
{
    if(NULL == strProcessName)
    {
        return FALSE;
    }
    HANDLE handle32Snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (INVALID_HANDLE_VALUE == handle32Snapshot)
    {
        return FALSE;
    }

    PROCESSENTRY32 pEntry;
    pEntry.dwSize = sizeof( PROCESSENTRY32 );

    //Search for all the process and terminate it
    if(Process32First(handle32Snapshot, &pEntry))
    {
        BOOL bFound = FALSE;
        if (!wcscmp(pEntry.szExeFile, strProcessName))
        {
            bFound = TRUE;
        }
        while((!bFound)&&Process32Next(handle32Snapshot, &pEntry))
        {
            if (!wcscmp(pEntry.szExeFile, strProcessName))
            {
                bFound = TRUE;
            }
        }
        if(bFound)
        {
            CloseHandle(handle32Snapshot);
            HANDLE handLe =  OpenProcess(PROCESS_TERMINATE , FALSE, pEntry.th32ProcessID);
            BOOL bResult = TerminateProcess(handLe,0);
            return bResult;
        }
    }

    CloseHandle(handle32Snapshot);
    return FALSE;
}

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    m_manager = new QNetworkAccessManager(this);
    ui->progressBar->hide();
    this->setWindowFlags(Qt::FramelessWindowHint);
    m_shadowSize = 5;
    ui->pushButtonClose->setObjectName("closeButton");
    ui->pushButtonClose->setFixedSize(27,22);
    ui->pushButtonClose->setToolTip("关闭");
    ui->pushButton_Update->setObjectName("updateButton");
    ui->label_update->setObjectName("labelUpdate");
    ui->pushButton_Update->setProperty("type",QString("update"));
}

Widget::~Widget()
{
    if(m_downloadFile.isOpen())
        m_downloadFile.close();
    delete ui;
}

void Widget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter  painter(this);
    painter.setRenderHint(QPainter::Antialiasing,true);
    QRect titleRect = QRect(0,0, this->width() , 33    );
    painter.save();
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::white);
    painter.drawRect(0, 0, this->width(), this->height());
    painter.restore();
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(20, 150, 210));
    painter.drawRect(titleRect);
}

void Widget::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() &Qt::LeftButton)
        move(event->globalPos()-mousePos);
}

void Widget::mousePressEvent(QMouseEvent *event)
{
    if(event->buttons()&Qt::LeftButton)
        mousePos = event->pos();
}
bool Widget::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
    if (this->minimumHeight() >= this->maximumHeight()
            && this->minimumWidth() >= this->maximumWidth())
    {
        return QWidget::nativeEvent(eventType, message, result);
    }
    Q_UNUSED(eventType)


    MSG *param = static_cast<MSG*>(message);
    switch (param->message)
    {
    case WM_NCHITTEST:
    {
        HWND hWnd = (HWND)this->winId();
        int nX = GET_X_LPARAM(param->lParam) - this->geometry().x();
        int nY = GET_Y_LPARAM(param->lParam) - this->geometry().y();
        {
            *result = HTCAPTION;
        }
        //如果窗口最大化了，则不支持resize
        if (IsZoomed(hWnd))
            return false;
        if (isMaximized())
            return false;
        if (nX > 0 && nX < this->m_shadowSize)
            *result = HTLEFT;
        if (nX > this->width() - this->m_shadowSize && nX < this->width())
            *result = HTRIGHT;
        if (nY > 0 && nY < this->m_shadowSize)
            *result = HTTOP;
        if (nY > this->height() - this->m_shadowSize && nY < this->height())
            *result = HTBOTTOM;
        if (nX > 0 && nX < this->m_shadowSize && nY > 0 && nY < this->m_shadowSize)
            *result = HTTOPLEFT;
        if (nX > this->width() - this->m_shadowSize && nX < this->width() && nY > 0 && nY < this->m_shadowSize)
            *result = HTTOPRIGHT;
        if (nX > 0 && nX < this->m_shadowSize && nY > this->height() - this->m_shadowSize && nY < this->height())
            *result = HTBOTTOMLEFT;
        if (nX > this->width() - this->m_shadowSize && nX < this->width()
                && nY > this->height() - this->m_shadowSize && nY < this->height())
            *result = HTBOTTOMRIGHT;
        if(*result == HTCAPTION)
        {
            return false;
        }
        else
        {
            return true;
        }
    }
    }
    return false;

}

void Widget::setNeedUpdate(int is)
{
    if(is)
    {
        ui->label_update->setText("当前版本不是最新，请进行更新！");
        ui->progressBar->hide();
    }
    else
    {
        ui->label_update->setText("当前版本为最新，不需要更新！");
        ui->pushButton_Update->setVisible(false);
    }
}

void Widget::setUrl(QString url)
{
    m_downloadUrl = url;
}

void Widget::setUpdateDir(QString path)
{
    if(path.isEmpty())
    {
        m_destPath = ".";
    }
    else
    {
        m_destPath = path;
    }
    m_tmpDir = new QDir(m_destPath);
    bool exist = m_tmpDir->exists("temp");
    if(!exist)
    {
        bool ok = m_tmpDir->mkdir("temp");
        if(ok)
            qDebug()<<"创建临时目录成功！";
    }
    m_tmpDir->cd("temp");
}

void Widget::setExecTargetName(QString name)
{
    m_execTarget = name;
}

void Widget::startDownload(QUrl url)
{
    ui->pushButton_Update->setEnabled(false);
    QNetworkRequest request(url);
    m_reply = m_manager->get(request);
    connect(m_reply,SIGNAL(finished()),this,SLOT(hasFinished()));
    connect(m_reply,SIGNAL(readyRead()),this,SLOT(readSth()));
    connect(m_reply,SIGNAL(downloadProgress(qint64,qint64)),this,SLOT(updateProgressBar(qint64,qint64)));
}
void Widget::readSth()
{
    m_downloadFile.write(m_reply->readAll());
}

void Widget::updateProgressBar(qint64 c, qint64 t)
{
    ui->progressBar->setMaximum(t);
    ui->progressBar->setValue(c);
}
void Widget::hasFinished()
{
    qDebug()<<m_downloadFile.fileName()<<"下载完成";

    if(m_downloadFile.isOpen())
        m_downloadFile.close(); //此处保存文件

    //1,把文件解压到临时目录
    ui->label_update->setText(QString("正在解压...."));
    unCompress(m_downloadFile.fileName());
    ui->label_update->setText(QString("下载完成"));
    ui->pushButton_Update->setEnabled(true);
    //2,删除zip压缩包
    m_downloadFile.remove();

    ui->label_update->setText(QString("更新完成,共更新%1个文件。").arg(m_files.count()));
    ui->pushButton_Update->setText("重启软件");
    ui->pushButton_Update->setProperty("type",QString("reboot"));
}

void Widget::unCompress(QString FileName)
{
    //解压到临时目录
    QStringList files = JlCompress::extractDir(FileName,m_tmpDir->absolutePath());
    QString tmp = m_tmpDir->absolutePath()+"/";
//    qDebug() << "tmp:" << tmp;
    foreach (QString file, files) {
        qDebug() << "download file:" << file;
        QFileInfo f = QFileInfo(file);
        if(f.isFile())
        {
            FileInfo info;
            info.name = f.fileName();
            info.path = f.filePath().remove(tmp).remove(info.name);
//            qDebug()<<info.name;
//            qDebug()<<info.path;
            m_files.append(info);
        }
    }
}

void Widget::on_pushButtonClose_clicked()
{
    qApp->quit();
}

void Widget::on_pushButton_Update_clicked()
{
    QString type = ui->pushButton_Update->property("type").toString();
    if(type == "update")
    {
        QString fileName = m_downloadUrl.fileName();
        if(fileName.isEmpty())
            qDebug()<<"文件名为空";
        m_downloadFile.setFileName(m_tmpDir->path()+"/"+fileName);
        if(m_downloadFile.open(QIODevice::WriteOnly))
        {
            startDownload(m_downloadUrl);
            ui->progressBar->setValue(0);
            ui->progressBar->show();
        }
        else
        {
            qDebug()<<"文件打开失败";
        }
    }
    else
    {
        //关闭主程序
        const wchar_t * encodedName = reinterpret_cast<const wchar_t *>(m_execTarget.utf16());
        FindAndKillProcessByName(encodedName);
        QTime t;
        t.start();
        ui->pushButton_Update->setEnabled(false);
        while(t.elapsed()<2000); //等待2秒

        //用新文件替换旧文件
        foreach(FileInfo file,m_files)
        {
//            qDebug() << "m_files Paths:" << file.path;
            QString newfile = m_tmpDir->absolutePath()+"/"+file.path+file.name;
            QString oldfile = m_destPath+"/"+file.path.remove("wst-cable/").remove("smart-ma/")+file.name;

            file.path.remove("/");
//            qDebug() << "after m_files Path:"  << file.path;
            QDir dir(m_destPath);
            if(!file.path.isEmpty())
            {
                if(!dir.exists(file.path))
                {
                    dir.mkpath(file.path);
                    qDebug()<<(m_destPath+"/"+file.path)<<"不存在";
                }
            }
            qDebug()<<"newfile:"<<newfile;
            qDebug()<<"oldfile:"<<oldfile;

            bool ok = QFile::remove(oldfile);
            if(ok)
                qDebug()<<"删除成功";
            else
                qDebug()<<"删除失败";
            ok = QFile::copy(newfile,oldfile);
            if(ok)
            {
                qDebug()<<"替换文件成功！";
            }
            else
            {
                qDebug()<<"替换失败";
            }
        }

        //删除临时目录
        bool s = m_tmpDir->removeRecursively();
        qDebug()<<"删除临时目录："<<s;
        //启用主程序
        QProcess *p1 = new QProcess();
        p1->setProgram(m_destPath+"/"+m_execTarget);
        p1->start();
        //关闭更新程序
        qApp->quit();
    }
}
