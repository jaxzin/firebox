#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{

    game = new Game();

    glWidget = new GLWidget(game);
    glWidget->setMouseTracking(true);

    setCentralWidget(glWidget);
    setMinimumSize(1280, 800);

    setWindowTitle("FireBox");
    //showFullScreen();

    timer = new QTimer( this );
    connect( timer, SIGNAL(timeout()), this, SLOT(timeOut()) );
    timer->start( 16 );



}

MainWindow::~MainWindow()
{

}

void MainWindow::timeOut()
{

    glWidget->makeCurrent();
    game->Update();

    glWidget->updateGL();

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

    glWidget->keyPressEvent(e);

}

void MainWindow::keyReleaseEvent(QKeyEvent * e)
{
    glWidget->keyReleaseEvent(e);
}
