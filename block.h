#ifndef BLOCK_H
#define BLOCK_H

#include "command.h"

#include <QList>

struct Block
{
    QString name;
    QString description;
    QList<Command> commands;

    Block(QString name,
          QString description = "No description",
          QList<Command> commands = QList<Command>())
    {
        this->name = name;
        this->description = description;
        this->commands = commands;
    }
};

#endif // BLOCK_H
