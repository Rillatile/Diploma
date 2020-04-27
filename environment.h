#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <QString>
#include <QSysInfo>

struct SystemInfo
{
    QString name;
    QString version;
    QString prettyName;

    SystemInfo(QString name, QString version, QString prettyName)
    {
        this->name = name;
        this->version = version;
        this->prettyName = prettyName;
    }
};

class Environment
{

public:
    static SystemInfo getSystemInfo();

private:
    Environment();
    static SystemInfo systemInfo;
};

#endif // ENVIRONMENT_H
