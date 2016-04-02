#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QListView>
#include <QStringList>
#include <QFileSystemModel>
#include <QDebug>
#include <QDir>
#include <QPoint>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    textFile = NULL;

    if ((!QDir(dataFolder).exists()) | (QDir(dataFolder).entryList(QDir::Files).size() == 0)){
        QDir().mkdir(dataFolder);
        QFile *firstFile = new QFile("Data/placeholder.txt");
        firstFile->open(QIODevice::WriteOnly);
        firstFile->write("Your first file");
        firstFile->flush();
        firstFile->close();
        delete firstFile;
    }

    QFileSystemModel *model = new QFileSystemModel();
    model->setRootPath(QDir::currentPath());
    model->setReadOnly(false);

    ui->listView->setModel(model);
    ui->listView->setRootIndex(model->index("Data"));
    ui->listView->setEditTriggers(QAbstractItemView::SelectedClicked);
    contextMenu = new QMenu();

    newFileAct = new QAction(tr("&New"), this);
    newFileAct->setStatusTip("Create New .txt file");

    delFileAct = new QAction(tr("&Delete"), this);
    delFileAct->setStatusTip("Delete selected file");

    contextMenu->addAction(newFileAct);
    contextMenu->addAction(delFileAct);

    connect(ui->listView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
    connect(newFileAct, SIGNAL(triggered(bool)), this, SLOT(newFile()));
    connect(delFileAct, SIGNAL(triggered(bool)), this, SLOT(deleteFile()));

    QDir::setCurrent("Data");

    QList<int> startingSize;
    startingSize << 150 << 500;
    ui->splitter->setSizes(startingSize);
    ui->splitter->setStretchFactor(0,0);

}

void MainWindow::deleteFile()
{
    QFile toRemove(ui->listView->model()->data(ui->listView->currentIndex()).toString());
    toRemove.open(QIODevice::ReadWrite);
    qDebug() << toRemove.remove();
    toRemove.close();
}

void MainWindow::newFile()
{
    QString newFilename = QString::number(QDateTime::currentDateTime().toMSecsSinceEpoch()) + QString(".txt");
    qDebug() << newFilename;

    QFile newFile(newFilename);
    newFile.open(QIODevice::ReadWrite);
    newFile.write("");
    newFile.flush();
    newFile.close();
}


MainWindow::~MainWindow()
{
    delete ui;
    delete textFile;
    delete contextMenu;
    delete newFileAct;
    delete delFileAct;
}

void MainWindow::on_plainTextEdit_textChanged()
{
    if(textFile){
        QString fileContents = ui->plainTextEdit->toPlainText();
        QString saveMsg = QString("Saved to ") + QString(textFile->fileName());
        ui->statusBar->showMessage(tr(saveMsg.toStdString().c_str()), 2500);
        textFile->close();
        textFile->open(QIODevice::WriteOnly);
        textFile->write(fileContents.toStdString().c_str());
        textFile->flush();
        textFile->close();
    } else{

    }
}

void MainWindow::on_listView_doubleClicked(const QModelIndex &index)
{
    QString openMsg = QString("Opened ") + QString(ui->listView->model()->data(index).toString());
    ui->statusBar->showMessage(tr(openMsg.toStdString().c_str()), 2500);

    ui->plainTextEdit->blockSignals(true);

    textFile = new QFile(ui->listView->model()->data(index).toString());

    textFile->open(QIODevice::ReadOnly | QIODevice::Text);
    ui->plainTextEdit->setPlainText(QString::fromStdString(textFile->readAll().toStdString()));

    ui->plainTextEdit->blockSignals(false);
    textFile->close();
}

void MainWindow::showContextMenu(const QPoint& pos)
{
    QPoint globalPos = this->mapToGlobal(pos);
    QAction* selectedItem = contextMenu->exec(globalPos);
    if (selectedItem){

    }
}
