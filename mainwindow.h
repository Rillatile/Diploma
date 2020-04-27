#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QStandardItemModel>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QStandardItemModel* model;
    QMap<QString, QString> modulesDir;
    void initialization();

private slots:
    void exitAction();
    void aboutProgramAction();
    void addExistingModule();
    void deleteModule();
};
#endif // MAINWINDOW_H
