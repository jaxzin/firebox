#include "glwidget.h"

GLWidget::GLWidget(Game * g) :
    game(g),
    calib_grid(false)
{    

    message_str = QString("FireBox Release 5.  Controls - URLs:[Tab,Enter] Walk:[W,A,S,D] Fly:[F] Grid:[G,1-8] FullScreen:[F11] Quit:[Escape]");

    //grabMouse();
    setCursor( QCursor( Qt::BlankCursor ) );

}

void GLWidget::initializeGL()
{

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_LINE_SMOOTH);
    //glEnable(GL_POLYGON_SMOOTH);

    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    //glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST);

    rift_render.initializeGL();
    game->initializeGL();

    if (rift_render.IsRiftDetected()) {
        message_str += QString("Rift detected.");
    }
    else {
        message_str += QString("Rift not detected.");
    }

}

void GLWidget::resizeGL(int w, int h)
{

    glViewport(0, 0, w, h);   
    rift_render.SetViewportSize(w, h);
    rift_render.initializeGL();

}

void GLWidget::mouseMoveEvent(QMouseEvent * e)
{    

    QPoint cent(width()/2, height()/2);
    QPoint globalCent = mapToGlobal(pos()) + cent;

    if (e->globalPos().x() == globalCent.x() && e->globalPos().y() == globalCent.y()) {

        mouse_pos = e->pos();
        return;

    }

    game->mouseMoveEvent(float(e->pos().x() - mouse_pos.x()), float(e->pos().y() - mouse_pos.y()));
    mouse_pos = e->pos();

    QCursor::setPos(globalCent);

}

void GLWidget::mousePressEvent(QMouseEvent * e)
{
    game->mousePressEvent(e);
}

void GLWidget::mouseReleaseEvent(QMouseEvent * e)
{
    game->mouseReleaseEvent(e);
}

void GLWidget::keyPressEvent(QKeyEvent * e)
{
    game->keyPressEvent(e);
}

void GLWidget::keyReleaseEvent(QKeyEvent * e)
{

    if (game->keyReleaseEvent(e)) {
        return;
    }

    switch (e->key()) { 

    case Qt::Key_1:

        rift_render.SetKappa(0, rift_render.GetKappa(0) + 0.001f);
        break;

    case Qt::Key_2:

        rift_render.SetKappa(0, rift_render.GetKappa(0) - 0.001f);
        break;

    case Qt::Key_3:

        rift_render.SetKappa(1, rift_render.GetKappa(1) + 0.01f);
        break;

    case Qt::Key_4:

        rift_render.SetKappa(1, rift_render.GetKappa(1) - 0.01f);
        break;

    case Qt::Key_5:

        rift_render.SetKappa(2, rift_render.GetKappa(2) + 0.1f);
        break;

    case Qt::Key_6:

        rift_render.SetKappa(2, rift_render.GetKappa(2) - 0.1f);
        break;

    case Qt::Key_7:

        rift_render.SetKappa(3, rift_render.GetKappa(3) + 1.0f);
        break;

    case Qt::Key_8:

        rift_render.SetKappa(3, rift_render.GetKappa(3) - 1.0f);
        break;

    case Qt::Key_G:

        calib_grid = !calib_grid;
        break;

    }

    //qDebug() << "kappas" << rift_render.GetKappa(0) << rift_render.GetKappa(1) << rift_render.GetKappa(2) << rift_render.GetKappa(3);

}

void GLWidget::paintGL()
{   

    QFont font;
    font.setPointSize(14);

    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //game->DrawGL();

    /*
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, 640, 800);
    game->DrawGL();

    glViewport(640, 0, 640, 800);
    game->DrawGL();
    */   

    QVector3D right, up, forward;
    rift_render.GetOrientation(right, up, forward);

    rift_render.RenderClear();

    rift_render.RenderLeftEye();
    game->DrawGL(-0.1, right, up, forward);
    if (calib_grid) {
        rift_render.DrawCalibrationGrid(40);
    }

    rift_render.RenderRightEye();
    game->DrawGL(0.1f, right, up, forward);
    if (calib_grid) {
        rift_render.DrawCalibrationGrid(40);
    }

    rift_render.RenderToRift();

    glClear(GL_DEPTH_BUFFER_BIT);    
    glColor3f(0.25f, 0.25f, 0.25f);

    if (calib_grid) {

        const QString kappas = rift_render.GetKappasString();
        const QString fps = QString("FPS:") + QString::number(rift_render.FPS());

        this->renderText(0.2, 0.51, 0, kappas, font);
        this->renderText(0.2, 0.41, 0, fps, font);

        this->renderText(0.6, 0.51, 0, kappas, font);
        this->renderText(0.2, 0.41, 0, fps, font);
    }


    this->renderText(0.01, 0.01, 0, message_str, font);

}
