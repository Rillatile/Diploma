#ifndef GUIBUILDER_H
#define GUIBUILDER_H

#include "parsed.h"

#include <QObject>
#include <QList>
#include <QWidget>
#include <QObject>

class GUIBuilder : public QObject
{
    Q_OBJECT

public:
    static QWidget* build(Parsed& parsed,
                          QWidget *parent = nullptr);
    static GUIBuilder *getInstance();

signals:
    void startExecute(QList<Variable> vars, int number);

public slots:
    void updateVariables();
    void changeBlockNumber(int number);

private:
    GUIBuilder();
    static QWidget *window;
    static GUIBuilder *instance;
    static QList<Variable> vars;
};

#endif // GUIBUILDER_H
