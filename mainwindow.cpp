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
        //QMessageBox mb;

        try
        {
            Parsed parsed = ModuleParser::parse(Cryptography::xorDecrypt(stream.readAll(), "Roman"));
            Executor::executeModule(moduleName, parsed);
            /*QString msg;

            for (int i = 0; i < parsed.variables.length(); i++)
            {
                msg += QString("Variable %1: %2, %3, %4.\n")
                        .arg(i + 1)
                        .arg(parsed.variables[i].name)
                        .arg(parsed.variables[i].description)
                        .arg(parsed.variables[i].value);
            }

            msg += '\n';
            for (int i = 0; i < parsed.constants.length(); i++)
            {
                msg += QString("Constant %1: %2, %3, %4.\n")
                        .arg(i + 1)
                        .arg(parsed.constants[i].name)
                        .arg(parsed.constants[i].description)
                        .arg(parsed.constants[i].value);
            }

            for (int i = 0; i < parsed.blocks.length(); i++)
            {
                msg += QString("\nBlock %1: %2, %3, commands:\n")
                        .arg(i + 1)
                        .arg(parsed.blocks[i].name)
                        .arg(parsed.blocks[i].description);
                for (int j = 0; j < parsed.blocks[i].commands.length(); j++)
                {
                    msg += QString("Command %1: %2, %3, %4.\n")
                            .arg(j + 1)
                            .arg(parsed.blocks[i].commands[j].command)
                            .arg(parsed.blocks[i].commands[j].description)
                            .arg(parsed.blocks[i].commands[j].resultVariable);
                }
            }

            mb.setText(msg);*/

            /*QProcess process;

            process.start("/bin/bash");

            if (!process.waitForStarted())
                return;*/

            /*process.write(parsed.blocks[0].commands[0].command.toUtf8()
                    + ';'
                    + parsed.blocks[0].commands[1].command.toUtf8());*/
            /*process.write("if ls / | grep -q etc; then echo 1; fi");
            process.closeWriteChannel();

            if (!process.waitForFinished())
                return;

            QByteArray out = process.readAllStandardOutput();
            QByteArray err = process.readAllStandardError();
            QMessageBox mb;
            QString str = "Output: " + QString::fromStdString(out.toStdString())
                    + "\n\nError: "
                    + QString::fromStdString(err.toStdString());

            mb.setText(str);
            mb.exec();*/
        }
        catch (const std::exception& error)
        {
            //mb.setText(error.what());
            //mb.exec();
        }

        //mb.exec();
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
