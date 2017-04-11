# AutoUpdate
Qt在线更新程序，该程序是增量更新

TODO：所有的更新文件的筛选工作都在服务器端，该程序只是一个客户端，接受来自服务器的更新包；

使用方式：
qDebug()<<"启动在线更新的程序";
QString program = qApp->applicationDirPath()+"/AutoUpdate.exe";
QString targetName = "SensorMonitor.exe";
QStringList a;
a<<qApp->applicationDirPath()<<(m_needUpdate? "1":"0")<<m_url<<targetName;
QString params = a.join(" ");
qDebug()<<params;
const wchar_t  *pars = reinterpret_cast<const wchar_t *>(params.utf16());
const wchar_t * name = reinterpret_cast<const wchar_t *>(program.utf16());
const wchar_t * type = reinterpret_cast<const wchar_t *>(QString("open").utf16());
ShellExecute(NULL, type, name,pars, NULL, 0);
