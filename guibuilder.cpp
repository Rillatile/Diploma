#include "guibuilder.h"
#include "executor.h"

#include <QDesktopWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QStackedWidget>
#include <QComboBox>
#include <QListWidget>
#include <QScrollArea>

GUIBuilder::GUIBuilder()
{

}

QList<Variable> GUIBuilder::vars = QList<Variable>();
int currentBlock = 0;

void GUIBuilder::changeBlockNumber(int number)
{
    currentBlock = number;
}

void GUIBuilder::updateVariables()
{
    if (window->children().length() >= 4)
    {
        auto children = window->children();

        for (int i = 3; i < children.length() - 1; i++)
        {
            if (children[i]->property("isVariable") != QVariant(true))
                break;

            QString name = static_cast<QLabel*>(children[i]->children().at(1)->children().at(1))->text();
            QString value = static_cast<QLineEdit*>(children[i]->children().at(1)->children().at(2))->text();

            for (int j = 0; j < vars.length(); j++)
            {
                if (vars[j].name == name)
                {
                    vars[j].value = value;
                    break;
                }
            }
        }

        emit startExecute(vars, currentBlock);
    }
}

GUIBuilder *GUIBuilder::instance = new GUIBuilder();
QWidget *GUIBuilder::window = nullptr;

QWidget *createVarInput(const Variable& variable)
{
    QWidget *varInput = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout();
    QWidget *widget = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout();

    QLabel *varName = new QLabel(variable.name);

    varName->setAlignment(Qt::AlignHCenter);
    varName->setStyleSheet("font-weight: bold;");

    layout->addWidget(varName);
    layout->addWidget(new QLineEdit(variable.value));
    widget->setLayout(layout);
    mainLayout->addWidget(widget);

    QLabel *desc = new QLabel(variable.description);

    desc->setAlignment(Qt::AlignHCenter);
    desc->setStyleSheet("font-style: italic;");
    mainLayout->addWidget(desc);
    varInput->setLayout(mainLayout);
    varInput->setProperty("isVariable", QVariant(true));

    return varInput;
}

QWidget *showConstValue(const Constant& constant)
{
    QWidget *constValue = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout();
    QWidget *widget = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout();

    QLabel *constName = new QLabel(constant.name);

    constName->setAlignment(Qt::AlignHCenter);
    constName->setStyleSheet("font-weight: bold;");

    layout->addWidget(constName);

    QLabel *constVal = new QLabel(constant.value);

    constVal->setAlignment(Qt::AlignHCenter);

    layout->addWidget(constVal);
    widget->setLayout(layout);
    mainLayout->addWidget(widget);

    QLabel *desc = new QLabel(constant.description);

    desc->setAlignment(Qt::AlignHCenter);
    desc->setStyleSheet("font-style: italic;");
    mainLayout->addWidget(desc);
    constValue->setLayout(mainLayout);


    return constValue;
}

QWidget *createBlockPage(const Block& block)
{
    QWidget *blockView = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout();
    QListWidget *commadsView = new QListWidget();

    QLabel *desc = new QLabel(block.description);

    desc->setAlignment(Qt::AlignHCenter);
    desc->setStyleSheet("font-style: italic;");

    mainLayout->addWidget(desc);

    for (int i = 0; i < block.commands.length(); i++)
        commadsView->addItem(block.commands[i].command);

    QPushButton *startBtn = new QPushButton("Исполнить");

    QObject::connect(startBtn, &QPushButton::clicked,
                     GUIBuilder::getInstance(), &GUIBuilder::updateVariables);

    QLabel *commandsLabel = new QLabel("Команды:");

    commandsLabel->setAlignment(Qt::AlignHCenter);
    commandsLabel->setStyleSheet("font-weight: bold;");

    mainLayout->addWidget(commandsLabel);
    mainLayout->addWidget(commadsView);
    mainLayout->addWidget(startBtn);
    blockView->setLayout(mainLayout);

    return blockView;
}

QWidget *showBlocks(const QList<Block>& blocks)
{
    QWidget *widget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout();
    QStackedWidget *blocksView = new QStackedWidget();
    QComboBox *comboBlocks = new QComboBox();

    for (int i = 0; i < blocks.length(); i++)
    {
        comboBlocks->addItem(blocks[i].name);
        blocksView->addWidget(createBlockPage(blocks[i]));
    }

    QObject::connect(comboBlocks, QOverload<int>::of(&QComboBox::activated),
                blocksView, &QStackedWidget::setCurrentIndex);

    QObject::connect(comboBlocks, QOverload<int>::of(&QComboBox::activated),
                GUIBuilder::getInstance(), &GUIBuilder::changeBlockNumber);

    layout->addWidget(comboBlocks);
    layout->addWidget(blocksView);
    widget->setLayout(layout);

    return widget;
}

bool hasInput(const QList<Variable>& variables)
{
    bool result = false;

    for (int i = 0; i < variables.length(); i++)
        if (!variables[i].isNotEntered)
        {
            result = true;
            return result;
        }

    return result;
}

QWidget *GUIBuilder::build(Parsed& parsed,
                           QWidget *parent)
{
    vars = parsed.variables;
    window = new QWidget(parent);
    QVBoxLayout *layout = new QVBoxLayout();

    layout->setSizeConstraint(QLayout::SetFixedSize);
    window->setWindowTitle("Исполнение модуля");

    QLabel *name = new QLabel(parsed.moduleName);

    name->setStyleSheet("font-weight: bold;text-decoration: underline;");
    name->setAlignment(Qt::AlignHCenter);
    layout->addWidget(name);

    if (hasInput(parsed.variables))
    {
        QLabel *varLabel = new QLabel("Переменные:");

        varLabel->setStyleSheet("font-weight: bold;");
        varLabel->setAlignment(Qt::AlignHCenter);
        layout->addWidget(varLabel);

        for (int i = 0; i < parsed.variables.length(); i++)
        {
            if (!parsed.variables[i].isNotEntered)
                layout->addWidget(createVarInput(parsed.variables[i]));
        }
    }

    if (parsed.constants.length() > 0)
    {
        QLabel *constLabel = new QLabel("Константы:");

        constLabel->setStyleSheet("font-weight: bold;");
        constLabel->setAlignment(Qt::AlignHCenter);
        layout->addWidget(constLabel);

        for (int i = 0; i < parsed.constants.length(); i++)
            layout->addWidget(showConstValue(parsed.constants[i]));
    }

    if (parsed.blocks.length() > 0)
    {
        QLabel *blocksLabel = new QLabel("Блоки:");

        blocksLabel->setStyleSheet("font-weight: bold;");
        blocksLabel->setAlignment(Qt::AlignHCenter);
        layout->addWidget(blocksLabel);

        layout->addWidget(showBlocks(parsed.blocks));
    }

    window->setLayout(layout);

    QScrollArea *area = new QScrollArea();

    area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    area->setFixedSize(window->size());
    area->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
    area->setWidget(window);

    return area;
}

GUIBuilder *GUIBuilder::getInstance()
{
    return instance;
}
