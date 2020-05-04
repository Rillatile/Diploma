#include "executor.h"
#include "guibuilder.h"

#include <QWidget>
#include <QMessageBox>
#include <QProcess>
#include <QRegExp>

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

void Executor::executeBlock(QList<Variable> vars, int blockNumber)
{
    lastParsed.variables = vars;

    const Block& executableBlock = lastParsed.blocks[blockNumber];
    QList<QPair<bool, QString>> results;

    for (int i = 0; i < executableBlock.commands.length(); i++)
    {
        results.push_back(executeCommand(transformCommand(executableBlock.commands[i].command)));

        if (results.last().first)
            throw std::runtime_error(QString("Во время выполнения команды под номером %1 возникла ошибка: %2.")
                                     .arg(i + 1).arg(results.last().second).toStdString());
        else
        {
            if (!executableBlock.commands[i].resultVariable.isEmpty())
            {
                bool flag = false;

                for (int j = 0; j < lastParsed.variables.length(); j++)
                {
                    if (lastParsed.variables[j].name == executableBlock.commands[i].resultVariable)
                    {
                        lastParsed.variables[j].value = results.last().second;
                        flag = true;
                        break;
                    }
                }

                if (!flag)
                    throw std::runtime_error(QString("Указана результирующая переменная %1, но она не объявлена.")
                                             .arg(executableBlock.commands[i].resultVariable).toStdString());
            }
        }
    }
}

QPair<bool, QString> Executor::executeCommand(QString command)
{
    QProcess process;

    process.start("/bin/bash");

    if (!process.waitForStarted())
        throw std::runtime_error("Ошибка!");

    process.write(command.toUtf8());
    process.closeWriteChannel();

    if (!process.waitForFinished())
        throw std::runtime_error("Ошибка!");

    QString errorMsg = process.readAllStandardError();

    return (errorMsg.isEmpty() ? QPair<bool, QString>(false, process.readAllStandardOutput())
                                                     : QPair<bool, QString>(true, errorMsg));
}

QString Executor::getVarOrConstValue(QString name)
{
    QString value = "";
    bool flag = false;

    for (int i = 0; !flag && i < lastParsed.constants.length(); i++)
    {
        if (lastParsed.constants[i].name == name)
        {
            value = lastParsed.constants[i].value;
            flag = true;
        }
    }

    for (int i = 0; flag && i < lastParsed.variables.length(); i++)
    {
        if (lastParsed.variables[i].name == name)
        {
            value = lastParsed.variables[i].value;
            flag = true;
        }
    }

    if (!flag)
        throw std::runtime_error(QString("Переменной или константы с именем %1 не найдено.")
                                 .arg(name).toStdString());

    return value;
}

bool Executor::isVariable(QString name)
{
    for (int i = 0; i < lastParsed.variables.length(); i++)
    {
        if (lastParsed.variables[i].name == name)
            return true;
    }

    for (int i = 0; i < lastParsed.constants.length(); i++)
    {
        if (lastParsed.constants[i].name == name)
            throw std::runtime_error(QString("Нельзя присваивать константам новое значение: %1.")
                                     .arg(name).toStdString());
    }

    return false;
}

QString Executor::transformCommand(QString command)
{
    QRegExp name("\\$[_\\w][_\\w\\d]*");
    int pos = 0;
    QString newCommand = command;

    while ((pos = name.indexIn(newCommand, pos)) != -1)
    {
        try
        {
            int idx = 0;

            if (idx + name.cap(0).length() < newCommand.length() - 1
                    && newCommand[idx + name.cap(0).length()] == '=')
            {
                if (isVariable(name.cap(0)))
                {
                    ;
                }
            }
            else
                newCommand = newCommand.replace(name.cap(0),
                                                getVarOrConstValue(name.cap(0).remove(0, 1)));
        }
        catch (const std::exception& error)
        {
            QMessageBox mb;

            mb.setText(error.what());
            mb.exec();

            break;
        }

        pos += name.matchedLength();
    }

    return newCommand;
}

void Executor::execute(QList<Variable> vars, int blockNumber)
{
    try
    {
        executeBlock(vars, blockNumber);
    }
    catch (const std::exception& error)
    {
        QMessageBox mb;

        mb.setText(error.what());
        mb.exec();
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

    QWidget *window = GUIBuilder::build(parsed, parent);

    window->show();
}

void Executor::successfulExecution()
{
    isExecuting = false;
    currentModuleName = "";
}
