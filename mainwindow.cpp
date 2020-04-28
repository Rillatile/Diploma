#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "cryptography.h"
#include "environment.h"
#include "executor.h"
#include "moduleparser.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    initialization();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initialization()
{
    ui->setupUi(this);
    this->setWindowTitle(tr("Diploma"));
    ui->menuFile->setTitle(tr("File"));
    ui->actionExit->setText(tr("Exit"));
    ui->menuHelp->setTitle(tr("Help"));
    ui->actionAboutProgram->setText(tr("About Diploma"));
    ui->addModuleBtn->setText(tr("Add an existing module"));
    ui->deleteModuleBtn->setText(tr("Delete a module"));
    ui->systemInfo->setText(tr("OS: ") + Environment::getSystemInfo().prettyName);

    model = new QStandardItemModel();
    QStringList horizontalHeader;

    horizontalHeader.append(tr("Name"));
    model->setHorizontalHeaderLabels(horizontalHeader);

    if (model->rowCount() == 0)
        ui->tableView->horizontalHeader()->hide();

    ui->tableView->setModel(model);
}

void MainWindow::exitAction()
{
    QApplication::exit();
}

void MainWindow::aboutProgramAction()
{
    QMessageBox mb;

    mb.setWindowTitle(tr("About Diploma"));
    mb.setText(tr("WIP."));
    mb.setStandardButtons(QMessageBox::Close);
    mb.exec();
}

void MainWindow::addExistingModule()
{
    QString moduleDir = QFileDialog::getOpenFileName(this, tr("Select a module"),
                                                     QDir::homePath(), tr("Module files (*.module)"));
    QString moduleName = moduleDir.mid(moduleDir.lastIndexOf("/") + 1);

    if (!moduleDir.isEmpty())
    {
        if (model->rowCount() == 0)
            ui->tableView->horizontalHeader()->show();

        modulesDir[moduleName] = moduleDir;
        model->setItem(model->rowCount(), 0, new QStandardItem(moduleName));
    }

    QFile module(moduleDir);

    if (module.open(QFile::ReadOnly))
    {
        QTextStream stream(&module);

        try
        {
            Parsed parsed = ModuleParser::parse(Cryptography::xorDecrypt(stream.readAll(), "Roman"));

            parsed.moduleName = moduleName;
            Executor::askInput(parsed);
        }
        catch (const std::exception& error)
        {
            QMessageBox mb;

            mb.setText(error.what());
            mb.exec();
        }

        module.close();
    }
}

void MainWindow::deleteModule()
{
    QString moduleDir = QFileDialog::getOpenFileName(this, tr("Select a module"),
                                                     QDir::homePath(), tr("Module files (*.module)"));

    QFile module(moduleDir);

    if (module.open(QFile::ReadOnly))
    {
        QTextStream stream(&module);

        QFile cryptModule(moduleDir.replace(".module", "CRYPT.module"));

        if (cryptModule.open(QFile::WriteOnly))
        {
            QTextStream out(&cryptModule);

            out << Cryptography::xorEncrypt(stream.readAll(), "Roman");
        }

        cryptModule.close();
        module.close();
    }
}
