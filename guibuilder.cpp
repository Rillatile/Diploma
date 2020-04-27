#include "guibuilder.h"

#include <QDesktopWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

GUIBuilder::GUIBuilder()
{

}

QWidget *GUIBuilder::build(const QList<Variable>& variables, QWidget *parent)
{
    QWidget *window = new QWidget(parent);
    QVBoxLayout *layout = new QVBoxLayout();

    window->setWindowTitle("Исполнение модуля");

    for (int i = 0; i < variables.length(); i++)
    {
        layout->addWidget(new QLabel(variables[i].name));
    }

    QPushButton *startBtn = new QPushButton("Выполнить модуль");

    layout->addWidget(startBtn);
    window->setLayout(layout);

    QDesktopWidget desktop;
    QRect rect = desktop.availableGeometry(desktop.primaryScreen());
    QPoint center = rect.center();

    window->setGeometry(0, 0, 500, 500);
    center.setX(center.x() - window->width() / 2);
    center.setY(center.y() - window->height() / 2);
    window->move(center);

    return window;
}
