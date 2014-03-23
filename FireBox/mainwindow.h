#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtOpenGL>
#include <QtGui>

#include "glwidget.h"
#include "game.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void keyPressEvent(QKeyEvent * e);
    void keyReleaseEvent(QKeyEvent * e);

public slots:

    void timeOut();

private:
    GLWidget * glWidget;
    Game * game;
    QTimer * timer;        
};

#endif // MAINWINDOW_H
