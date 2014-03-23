#include "glwidget.h"

GLWidget::GLWidget() :    
    instruct_tex(0),
    calib_grid(false)
{       

    message_str = QString("FireBox Release 8. FullScreen:[F11] Walk:[W,A,S,D] Fly:[F] Reset:[R] URLs:[Tab,Enter] Grid:[G,1-8] Mute:[M] Quit:[Escape].  ");
    setCursor( QCursor( Qt::BlankCursor ) );
    QCursor::setPos(GetWinCentre());

}

void GLWidget::initializeGL()
{

    SoundManager::LoadSounds();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_LINE_SMOOTH);
    //glEnable(GL_POLYGON_SMOOTH);

    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    //glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glClearStencil(0);

    glEnable(GL_DEPTH_TEST);   

    game.initializeGL();

    //qDebug() << rift_render.IsRiftDetected();

    if (rift_render.IsRiftDetected()) {
        rift_render.initializeGL();
        message_str += QString("Rift detected.");
        SoundManager::Play(1, false);
    }
    else {
        message_str += QString("Rift not detected.");
        SoundManager::Play(2, false);
    }

    SoundManager::Play(0, true);

    SetupInstructions();

}

void GLWidget::resizeGL(int w, int h)
{

    QCursor::setPos(GetWinCentre());
    glViewport(0, 0, w, h);
    if (rift_render.IsRiftDetected()) {
        rift_render.SetViewportSize(w, h);
        rift_render.initializeGL();
    }

}

QPoint GLWidget::GetWinCentre()
{
    QPoint cent(width()/2, height()/2);
    QPoint win_pos = mapToGlobal(pos());
    return win_pos + cent;
}

void GLWidget::mouseMoveEvent(QMouseEvent * e)
{    

    QPoint win_cent = GetWinCentre();
    QPoint win_pos = mapToGlobal(pos());
    mouse_pos = win_pos  + e->pos();

    if (mouse_pos != win_cent) {
        game.mouseMoveEvent(float(mouse_pos.x() - win_cent.x()), float(mouse_pos.y() - win_cent.y()));
        QCursor::setPos(win_cent);
    }

}

void GLWidget::mousePressEvent(QMouseEvent * e)
{
    game.mousePressEvent(e);
}

void GLWidget::mouseReleaseEvent(QMouseEvent * e)
{
    game.mouseReleaseEvent(e);
}

void GLWidget::keyPressEvent(QKeyEvent * e)
{
    game.keyPressEvent(e);
}

void GLWidget::keyReleaseEvent(QKeyEvent * e)
{

    if (game.keyReleaseEvent(e)) {
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

    case Qt::Key_M:
        SoundManager::ToggleMute();
        break;

    }

    //qDebug() << "kappas" << rift_render.GetKappa(0) << rift_render.GetKappa(1) << rift_render.GetKappa(2) << rift_render.GetKappa(3);

}

void GLWidget::UpdateGame()
{
    game.Update();
}

void GLWidget::paintGL()
{   

    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //game->DrawGL();

    /*
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, 640, 800);
    game->DrawGL();

    glViewport(640, 0, 640, 800);
    game->DrawGL();
    */   

    if (rift_render.IsRiftDetected()) {

        const float game_ipd = 0.05f;

        QVector3D right, up, forward;
        rift_render.GetOrientation(right, up, forward);

        rift_render.RenderClear();

        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        rift_render.RenderLeftEye();
        game.DrawGL(-game_ipd, up, forward);
        if (calib_grid) {
            rift_render.DrawCalibrationGrid(40);
        }

        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        rift_render.RenderRightEye();
        game.DrawGL(game_ipd, up, forward);
        if (calib_grid) {
            rift_render.DrawCalibrationGrid(40);
        }

        rift_render.RenderToRift();

    }
    else {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, width(), height());

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(60.0f, float(width()) / float(height()), 0.005f, 100.0f);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        game.DrawGL(0.0, QVector3D(0, 1, 0), QVector3D(0, 0, -1));

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(0, 1, 0, 1);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

    }

    glClear(GL_DEPTH_BUFFER_BIT);    
    glColor3f(0.25f, 0.25f, 0.25f);

    /*
    if (calib_grid) {

        const QString kappas = rift_render.GetKappasString();
        const QString fps = QString("FPS:") + QString::number(rift_render.FPS());

        this->renderText(0.2, 0.51, 0, kappas, font);
        this->renderText(0.2, 0.41, 0, fps, font);

        this->renderText(0.6, 0.51, 0, kappas, font);
        this->renderText(0.2, 0.41, 0, fps, font);
    }
    */

    //this->renderText(0.01, 0.01, 0, message_str, font);
    //qDebug() << "test";

    /*
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    drawInstructions(&painter);
    painter.end();
    */

    DrawInstructions();

}

void GLWidget::SetupInstructions()
{

    const int tex_width = 1024;
    const int tex_height = 16;

    QImage img = QImage(tex_width, tex_height, QImage::Format_ARGB32);
    img.fill(0x00000000);

    QTextOption option;
    option.setWrapMode(QTextOption::WordWrap);

    QPainter painter(&img);
    //painter.fillRect(QRectF(0, 0, 128, 128), Qt::white);
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 10));
    //painter.drawLine(5, 5, 25, 25);
    painter.drawText(QRectF(2, 0, tex_width, tex_height), message_str, option);
    painter.end();

    QImage glimg;
    glimg = QGLWidget::convertToGLFormat( img );

    glGenTextures(1, &instruct_tex);
    glBindTexture(GL_TEXTURE_2D, instruct_tex);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, 4, glimg.width(), glimg.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, glimg.bits());
    glBindTexture(GL_TEXTURE_2D, 0);

}

void GLWidget::DrawInstructions()
{

    glColor3f(0.4f, 0.4f, 0.4f);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, instruct_tex);

    glBegin(GL_QUADS);
    glTexCoord2i(0, 0);
    glVertex2f(0, 0);
    glTexCoord2i(1, 0);
    glVertex2f(1, 0);
    glTexCoord2i(1, 1);
    glVertex2f(1, 0.025);    
    glTexCoord2i(0, 1);
    glVertex2f(0, 0.025);
    glEnd();

    glDisable(GL_TEXTURE_2D);

}
