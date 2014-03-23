#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QtOpenGL>
#include <QtGui>
#include <QGLWidget>

#include <GL/glu.h>

#include "game.h"
#include "riftrenderer.h"
#include "soundmanager.h"

class GLWidget : public QGLWidget
{

public:
    GLWidget();

    void mouseMoveEvent(QMouseEvent * e);
    void mousePressEvent(QMouseEvent * e);
    void mouseReleaseEvent(QMouseEvent * e);
    void keyPressEvent(QKeyEvent * e);
    void keyReleaseEvent(QKeyEvent * e);

    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

    void UpdateGame();

private:

    QPoint GetWinCentre();
    void SetupInstructions();
    void DrawInstructions();

    QPoint mouse_pos;
    Game game;
    RiftRenderer rift_render;

    GLuint instruct_tex;

    QString message_str;
    bool calib_grid;

};

#endif // GLWIDGET_H
