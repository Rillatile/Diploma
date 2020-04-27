#include "executor.h"
#include "guibuilder.h"

#include <QWidget>

Executor::Executor()
{

}

bool Executor::isExecuting = false;
QString Executor::currentModuleName = "";

void Executor::executeModule(const QString& moduleName, const Parsed& parsed, QWidget *parent)
{
    if (isExecuting)
        throw std::runtime_error(QString("Another module is already executing: %1. Please wait for it to finish.")
                                 .arg(moduleName).toStdString());

    isExecuting = true;

    QWidget *window = GUIBuilder::build(parsed.variables, parent);

    window->show();
}
