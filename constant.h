#ifndef CONSTANT_H
#define CONSTANT_H

#include <QString>

struct Constant
{
    QString name;
    QString description;
    QString value;

    Constant(QString name,
             QString description = "No description",
             QString value = "")
    {
        this->name = name;
        this->description = description;
        this->value = value;
    }
};

#endif // CONSTANT_H
