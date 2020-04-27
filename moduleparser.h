#ifndef MODULEPARSER_H
#define MODULEPARSER_H

#include "parsed.h"

class ModuleParser
{
public:
    static const Parsed parse(const QString& source);

private:
    ModuleParser();
};

#endif // MODULEPARSER_H
