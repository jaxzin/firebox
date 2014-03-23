#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{   

    glWidget.setMouseTracking(true);

    setCentralWidget(&glWidget);
    setMinimumSize(1280, 800);

    setWindowTitle("FireBox");
    //showFullScreen();

    connect( &timer, SIGNAL(timeout()), this, SLOT(timeOut()) );
    timer.start( 0 );

}

MainWindow::~MainWindow()
{

}

void MainWindow::timeOut()
{

    //glWidget->makeCurrent();
    glWidget.UpdateGame();
    glWidget.updateGL();

}

void MainWindow::keyPressEvent(QKeyEvent * e)
{

    if (e->key() == Qt::Key_F11) {

        if (windowState() & Qt::WindowFullScreen) {
            showNormal();
        }
        else {
            showFullScreen();
        }
    }
    else if (e->key() == Qt::Key_Escape) {
        exit(0);
    }

    glWidget.keyPressEvent(e);

}

void MainWindow::keyReleaseEvent(QKeyEvent * e)
{
    glWidget.keyReleaseEvent(e);
}
