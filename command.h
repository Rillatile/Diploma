#ifndef COMMAND_H
#define COMMAND_H

#include <QString>

struct Command
{
    QString command;
    QString description;
    QString resultVariable;

    Command(QString command,
            QString description = "No description",
            QString resultVariable = "")
    {
        this->command = command;
        this->description = description;
        this->resultVariable = resultVariable;
    }
};

#endif // COMMAND_H
