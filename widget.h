#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>
#include <QFile>
#include <QEvent>
#include <QTemporaryDir>

#ifndef CONFIG_FILE
#define CONFIG_FILE   tr("configUpdate.txt")
#endif
namespace Ui {
class Widget;
}
class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
    void startDownload(QUrl url);
    void setNeedUpdate(int is);
    void setUrl(QString url);

    void unCompress(QString filename);

    void setUpdateDir(QString path);
    void setExecTargetName(QString name);

    struct FileInfo{
        QString name;
        QString path;
        FileInfo(QString n,QString p="", QString t="" ){
            name = n;
            path = p;
        }
        FileInfo(){}
    };

private slots:
    void updateProgressBar(qint64 c,qint64 t);
    void hasFinished();
    void readSth();
    void on_pushButtonClose_clicked();
    void on_pushButton_Update_clicked();
protected:
    void paintEvent(QPaintEvent *event);
    void mouseMoveEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);
    bool nativeEvent(const QByteArray &eventType, void *message, long *result);
private:
    QNetworkAccessManager *m_manager;
    QNetworkReply *m_reply;
    QUrl m_downloadUrl;
    QFile m_downloadFile;
    Ui::Widget *ui;
    QPoint mousePos;
    int m_shadowSize;

    QString m_execTarget;
    QString m_destPath;
    QDir *m_tmpDir;
    QVector<FileInfo> m_files;
    QVector<FileInfo> m_oldFiles;
    int m_fileIndex;
};

#endif // WIDGET_H
