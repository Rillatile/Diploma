#ifndef GUIBUILDER_H
#define GUIBUILDER_H

#include "variable.h"

#include <QObject>
#include <QList>
#include <QWidget>

class GUIBuilder : public QObject
{
    Q_OBJECT

public:
    static QWidget* build(QList<Variable>& variables,
                          const QString& moduleName,
                          QWidget *parent = nullptr);
    static GUIBuilder *getInstance();

signals:
    void startExecute(QList<Variable> vars);

private:
    GUIBuilder();
    static QWidget *window;
    static GUIBuilder *instance;
    static QList<Variable> vars;


private slots:
    void updateVariables();
};

#endif // GUIBUILDER_H
