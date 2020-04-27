#ifndef VARIABLE_H
#define VARIABLE_H

#include <QString>

struct Variable
{
    QString name;
    QString description;
    QString value;
    bool isNotEntered;

    Variable(QString name,
             QString description = "No description",
             QString value = "",
             bool isNotEntered = false)
    {
        this->name = name;
        this->description = description;
        this->value = value;
        this->isNotEntered = isNotEntered;
    }
};

#endif // VARIABLE_H
