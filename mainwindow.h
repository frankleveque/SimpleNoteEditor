#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QMenu>
#include <QModelIndex>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    private:
        Ui::MainWindow *ui;
        QFile *textFile;
        QMenu *contextMenu;
        QAction *newFileAct;
        QAction *renameFileAct;
        QAction *delFileAct;
        const QString dataFolder = "Data";
        const int leftWidth = 200;
        const int rightWidth = 500;

    public:
        explicit MainWindow(QWidget *parent = 0);
        ~MainWindow();

        private slots:
        void on_plainTextEdit_textChanged();
        void on_listView_doubleClicked(const QModelIndex &index);
        void showContextMenu(const QPoint&);
        void newFile();
        void startRename();
        void deleteFile();
        void reset();
 };

#endif // MAINWINDOW_H
