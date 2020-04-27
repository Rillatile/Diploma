#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "parsed.h"

class Executor
{
public:
    static void executeModule(const QString& moduleName, const Parsed& parsed, QWidget *parent = nullptr);

private:
    Executor();
    static bool isExecuting;
    static QString currentModuleName;
};

#endif // EXECUTOR_H
