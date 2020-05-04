#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "parsed.h"

#include <QObject>

class Executor : public QObject
{
    Q_OBJECT

public:
    static Executor *getInstance();
    static void askInput(Parsed& parsed, QWidget *parent = nullptr);

public slots:
    void execute(QList<Variable> vars, int blockNumber);

private:
    Executor();
    static bool isExecuting;
    static QString currentModuleName;
    static Executor *instance;
    static Parsed lastParsed;
    void successfulExecution();
    void executeBlock(QList<Variable> vars, int blockNumber);
    QPair<bool, QString> executeCommand(QString command);
    QString transformCommand(QString command);
    QString getVarOrConstValue(QString name);
    bool isVariable(QString name);
};

#endif // EXECUTOR_H
