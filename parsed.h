#ifndef PARSED_H
#define PARSED_H

#include "block.h"
#include "command.h"
#include "constant.h"
#include "variable.h"

struct Parsed
{
    QList<Variable> variables;
    QList<Constant> constants;
    QList<Block> blocks;

    Parsed(QList<Variable> variables,
           QList<Constant> constants,
           QList<Block> blocks)
    {
        this->variables = variables;
        this->constants = constants;
        this->blocks = blocks;
    }
};

#endif // PARSED_H
