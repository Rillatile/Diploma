#ifndef PARSED_H
#define PARSED_H

#include "block.h"
#include "command.h"
#include "constant.h"
#include "variable.h"

struct Parsed
{
    QString moduleName;
    QList<Variable> variables;
    QList<Constant> constants;
    QList<Block> blocks;

    Parsed(QList<Variable> variables,
           QList<Constant> constants,
           QList<Block> blocks,
           QString moduleName = "")
    {
        this->moduleName = moduleName;
        this->variables = variables;
        this->constants = constants;
        this->blocks = blocks;
    }

    Parsed()
    {
        this->moduleName = "";
        this->variables = QList<Variable>();
        this->constants = QList<Constant>();
        this->blocks = QList<Block>();
    }
};

#endif // PARSED_H
