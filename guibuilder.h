#ifndef GUIBUILDER_H
#define GUIBUILDER_H

#include "variable.h"

#include <QList>
#include <QWidget>

class GUIBuilder
{
public:
    static QWidget* build(const QList<Variable>& variables, QWidget *parent = nullptr);

private:
    GUIBuilder();
};

#endif // GUIBUILDER_H
