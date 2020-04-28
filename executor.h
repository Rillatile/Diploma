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
    void execute(QList<Variable> vars);

private:
    Executor();
    static bool isExecuting;
    static QString currentModuleName;
    static Executor *instance;
    static Parsed lastParsed;
    void successfulExecution();
};

#endif // EXECUTOR_H
