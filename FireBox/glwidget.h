#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QtOpenGL>
#include <QtGui>
#include <QGLWidget>

#include <GL/glu.h>

#include "game.h"
#include "riftrenderer.h"

class GLWidget : public QGLWidget
{
public:
    GLWidget(Game * g);       

    void mouseMoveEvent(QMouseEvent * e);
    void mousePressEvent(QMouseEvent * e);
    void mouseReleaseEvent(QMouseEvent * e);
    void keyPressEvent(QKeyEvent * e);
    void keyReleaseEvent(QKeyEvent * e);

    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

private:

    QPoint mouse_pos;
    Game * game;
    RiftRenderer rift_render;

    QString message_str;
    bool calib_grid;

};

#endif // GLWIDGET_H
