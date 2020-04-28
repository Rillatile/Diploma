#include "guibuilder.h"
#include "executor.h"

#include <QDesktopWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

GUIBuilder::GUIBuilder()
{

}

QList<Variable> GUIBuilder::vars = QList<Variable>();

void GUIBuilder::updateVariables()
{
    auto children = window->children();

    for (int i = 2; i < children.length() - 1; i++)
    {
        QString value = static_cast<QLineEdit*>(children[i]->children().at(1)->children().at(2))->text();

        vars[i - 2].value = value;
    }

    emit startExecute(vars);
}

GUIBuilder *GUIBuilder::instance = new GUIBuilder();
QWidget *GUIBuilder::window = nullptr;

QWidget *createVarInput(const Variable& variable)
{
    QWidget *varInput = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout();
    QWidget *widget = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout();

    layout->addWidget(new QLabel(variable.name));
    layout->addWidget(new QLineEdit(variable.value));
    widget->setLayout(layout);
    mainLayout->addWidget(widget);

    QLabel *desc = new QLabel(variable.description);

    desc->setAlignment(Qt::AlignHCenter);
    desc->setStyleSheet("font-style: italic;");
    mainLayout->addWidget(desc);
    varInput->setLayout(mainLayout);

    return varInput;
}

QWidget *GUIBuilder::build(QList<Variable>& variables,
                           const QString& moduleName,
                           QWidget *parent)
{
    vars = variables;
    window = new QWidget(parent);
    QVBoxLayout *layout = new QVBoxLayout();

    layout->setSizeConstraint(QLayout::SetFixedSize);
    window->setWindowTitle("Исполнение модуля");

    QLabel *name = new QLabel(moduleName);

    name->setStyleSheet("font-weight: bold;");
    name->setAlignment(Qt::AlignHCenter);
    layout->addWidget(name);

    for (int i = 0; i < variables.length(); i++)
    {
        if (!variables[i].isNotEntered)
            layout->addWidget(createVarInput(variables[i]));
    }

    QPushButton *startBtn = new QPushButton("Исполнить");

    connect(startBtn, &QPushButton::clicked, getInstance(), &GUIBuilder::updateVariables);

    layout->addWidget(startBtn);
    window->setLayout(layout);

    return window;
}

GUIBuilder *GUIBuilder::getInstance()
{
    return instance;
}
