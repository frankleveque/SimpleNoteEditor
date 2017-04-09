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

    //create placeholder file on startup
    if ((!QDir(dataFolder).exists()) | (QDir(dataFolder).entryList(QDir::Files).size() == 0)){
        QDir().mkdir(dataFolder);
        QFile *firstFile = new QFile("Data/placeholder.txt");
        firstFile->open(QIODevice::WriteOnly);
        firstFile->write("Your first file");
        firstFile->flush();
        firstFile->close();
        delete firstFile;
    }

    //set up filesystem model
    QFileSystemModel *model = new QFileSystemModel();
    model->setRootPath(QDir::currentPath());
    model->setReadOnly(false);

    //attach it to listview
    ui->listView->setModel(model);
    ui->listView->setRootIndex(model->index("Data"));
    ui->listView->setEditTriggers(QAbstractItemView::EditKeyPressed|QAbstractItemView::SelectedClicked);

    //change to data folder
    QDir::setCurrent("Data");

    //create context menu for listview
    contextMenu = new QMenu();

    newFileAct = new QAction(tr("&New"), this);
    newFileAct->setStatusTip("Create New .txt file");

    renameFileAct = new QAction(tr("&Rename"), this);
    renameFileAct->setStatusTip("Rename file");

    delFileAct = new QAction(tr("&Delete"), this);
    delFileAct->setStatusTip("Delete selected file");

    //attach actions to context
    contextMenu->addAction(newFileAct);
    contextMenu->addAction(renameFileAct);
    contextMenu->addAction(delFileAct);

    //connect signals and slots
    connect(ui->listView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
    connect(newFileAct, SIGNAL(triggered(bool)), this, SLOT(newFile()));
    connect(renameFileAct, SIGNAL(triggered(bool)), this, SLOT(startRename()));
    connect(delFileAct, SIGNAL(triggered(bool)), this, SLOT(deleteFile()));
    connect(model, SIGNAL(fileRenamed(QString,QString,QString)), this, SLOT(reset()));

    //set initial pane sizes
    ui->splitter->setSizes(QList<int>({leftWidth, rightWidth}));
    ui->splitter->setStretchFactor(0,0);

    reset();

}

void MainWindow::reset(){

    textFile = NULL;

    ui->plainTextEdit->blockSignals(true);
    ui->plainTextEdit->clear();
    ui->plainTextEdit->setReadOnly(true);
    ui->plainTextEdit->setStyleSheet("background: #DADADA");

    ui->listView->clearSelection();
}


void MainWindow::on_listView_doubleClicked(const QModelIndex &index)
{
    ui->plainTextEdit->blockSignals(true);
    ui->plainTextEdit->clear();
    ui->plainTextEdit->setReadOnly(false);
    ui->plainTextEdit->setStyleSheet("");

    //open selected file
    textFile = new QFile(ui->listView->model()->data(index).toString());
    textFile->open(QIODevice::ReadOnly | QIODevice::Text);

    //populate plaintext with file contents
    ui->plainTextEdit->setPlainText(QString::fromStdString(textFile->readAll().toStdString()));

    textFile->close();

    //allow plaintext to signal
    ui->plainTextEdit->blockSignals(false);

    //show indefinite status
    QString openMsg = QString("Opened ") + QString(ui->listView->model()->data(index).toString());
    ui->statusBar->showMessage(tr(openMsg.toStdString().c_str()));

}

void MainWindow::on_plainTextEdit_textChanged()
{

    if(textFile){
        QString ptContents = ui->plainTextEdit->toPlainText();

        textFile->open(QIODevice::WriteOnly);
        textFile->write(ptContents.toStdString().c_str());
        textFile->flush();
        textFile->close();

        //deselect file
        ui->listView->clearSelection();

        //show indefinite status
        QString saveMsg = QString("Saved to ") + QString(textFile->fileName());
        ui->statusBar->showMessage(tr(saveMsg.toStdString().c_str()));

    }
}

void MainWindow::showContextMenu(const QPoint& pos)
{
    QPoint globalPos = this->mapToGlobal(pos);
    contextMenu->exec(globalPos);
}


void MainWindow::newFile()
{
    QString newFilename = QString::number(QDateTime::currentDateTime().toMSecsSinceEpoch()) + QString(".txt");

    QFile newFile(newFilename);
    newFile.open(QIODevice::ReadWrite);
    newFile.write("");
    newFile.close();

    qDebug() << "File created" << newFilename;

}

void MainWindow::startRename()
{
    ui->listView->edit(ui->listView->currentIndex());
}

void MainWindow::deleteFile()
{
    QFile toRemove(ui->listView->model()->data(ui->listView->currentIndex()).toString());
    toRemove.open(QIODevice::ReadWrite);
    if(toRemove.remove()){
        qDebug() << "File deleted" << toRemove.fileName();
    }
    toRemove.close();

    this->reset();
}



MainWindow::~MainWindow()
{
    delete ui;
    delete textFile;
    delete contextMenu;
    delete newFileAct;
    delete delFileAct;
}


