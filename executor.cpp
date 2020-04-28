#include "executor.h"
#include "guibuilder.h"

#include <QWidget>
#include <QMessageBox>
#include <QProcess>

Executor::Executor()
{

}

bool Executor::isExecuting = false;
QString Executor::currentModuleName = "";
Executor *Executor::instance = new Executor();
Parsed Executor::lastParsed = {};

Executor *Executor::getInstance()
{
    return instance;
}

QString executeCommand(QString command)
{
    QProcess process;

    process.start("/bin/bash");

    if (!process.waitForStarted())
        throw std::runtime_error("Ошибка!");

    process.write(command.toUtf8());
    process.closeWriteChannel();

    if (!process.waitForFinished())
        throw std::runtime_error("Ошибка!");

    return process.readAllStandardOutput();
}

void Executor::execute(QList<Variable> vars)
{
    lastParsed.variables = vars;

    for (int i = 0; i < lastParsed.blocks.length(); i++)
    {
        for (int j = 0; j < lastParsed.blocks[i].commands.length(); j++)
        {
            QString res = executeCommand(lastParsed.blocks[i].commands[j].command);

            if (!lastParsed.blocks[i].commands[j].resultVariable.isEmpty())
            {
                for (int k = 0; k < lastParsed.variables.length(); k++)
                {
                    if (lastParsed.variables[k].name == lastParsed.blocks[i].commands[j].resultVariable)
                        lastParsed.variables[k].value = res;
                }
            }
        }
    }
}

void Executor::askInput(Parsed& parsed, QWidget *parent)
{
    connect(GUIBuilder::getInstance(), &GUIBuilder::startExecute, Executor::getInstance(), &Executor::execute);

    if (isExecuting)
        throw std::runtime_error(QString("Another module is already executing: %1. Please wait for it to finish.")
                                 .arg(parsed.moduleName).toStdString());

    isExecuting = true;
    currentModuleName = parsed.moduleName;
    lastParsed = parsed;

    QWidget *window = GUIBuilder::build(parsed.variables, parsed.moduleName, parent);

    window->show();
}

void Executor::successfulExecution()
{
    isExecuting = false;
    currentModuleName = "";
}
