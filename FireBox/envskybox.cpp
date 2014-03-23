#include "envskybox.h"

EnvSkybox::EnvSkybox()
{

}

EnvSkybox::~EnvSkybox()
{

    DeleteTextures();

}

void EnvSkybox::LoadTextures()
{

    QImage img0("assets/skybox/miramar_dn.png");
    QImage img1("assets/skybox/miramar_ft.png");
    QImage img2("assets/skybox/miramar_lf.png");
    QImage img3("assets/skybox/miramar_bk.png");
    QImage img4("assets/skybox/miramar_rt.png");
    QImage img5("assets/skybox/miramar_up.png");

    tex[0] = EnvObject::LoadTexture(img0);
    tex[1] = EnvObject::LoadTexture(img1);
    tex[2] = EnvObject::LoadTexture(img2);
    tex[3] = EnvObject::LoadTexture(img3);
    tex[4] = EnvObject::LoadTexture(img4);
    tex[5] = EnvObject::LoadTexture(img5);

}

void EnvSkybox::DrawGL()
{

    //qDebug() << "drawing";
    //faces 0 - down, 1 - front, 2 - left, 3 - back, 4 - right, 5 - up

    glDepthMask(GL_FALSE);

    GLdouble model_view[16];
    glGetDoublev(GL_MODELVIEW_MATRIX, model_view);
    //qDebug() << model_view[13] << model_view[14] << model_view[15];

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    model_view[12] = 0.0f; //zero out the translation (in stereo, skybox seems at infinity then)
    model_view[13] = 0.0f;
    model_view[14] = 0.0f;
    glLoadMatrixd(model_view);

    glColor3f(1, 1, 1);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex[0]);

    glBegin(GL_QUADS);
    glTexCoord2i(0, 0);
    glVertex3f(-1, -1, -1);
    glTexCoord2i(1, 0);
    glVertex3f(1, -1, -1);
    glTexCoord2i(1, 1);
    glVertex3f(1, -1, 1);
    glTexCoord2i(0, 1);
    glVertex3f(-1, -1, 1);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, tex[1]);

    glBegin(GL_QUADS);
    glTexCoord2i(0, 0);
    glVertex3f(-1, -1, -1);
    glTexCoord2i(1, 0);
    glVertex3f(1, -1, -1);
    glTexCoord2i(1, 1);
    glVertex3f(1, 1, -1);
    glTexCoord2i(0, 1);
    glVertex3f(-1, 1, -1);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, tex[2]);

    glBegin(GL_QUADS);
    glTexCoord2i(0, 0);
    glVertex3f(1, -1, -1);
    glTexCoord2i(0, 1);
    glVertex3f(1, 1, -1);
    glTexCoord2i(1, 1);
    glVertex3f(1, 1, 1);
    glTexCoord2i(1, 0);
    glVertex3f(1, -1, 1);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, tex[3]);

    glBegin(GL_QUADS);
    glTexCoord2i(1, 0);
    glVertex3f(-1, -1, 1);
    glTexCoord2i(0, 0);
    glVertex3f(1, -1, 1);
    glTexCoord2i(0, 1);
    glVertex3f(1, 1, 1);
    glTexCoord2i(1, 1);
    glVertex3f(-1, 1, 1);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, tex[4]);

    glBegin(GL_QUADS);
    glTexCoord2i(0, 0);
    glVertex3f(-1, -1, 1);
    glTexCoord2i(0, 1);
    glVertex3f(-1, 1, 1);
    glTexCoord2i(1, 1);
    glVertex3f(-1, 1, -1);
    glTexCoord2i(1, 0);
    glVertex3f(-1, -1, -1);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, tex[5]);

    glBegin(GL_QUADS);
    glTexCoord2i(1, 0);
    glVertex3f(-1, 1, -1);
    glTexCoord2i(1, 1);
    glVertex3f(1, 1, -1);
    glTexCoord2i(0, 1);
    glVertex3f(1, 1, 1);
    glTexCoord2i(0, 0);
    glVertex3f(-1, 1, 1);
    glEnd();

    //glDisable(GL_TEXTURE_2D);

    glPopMatrix();

    glDepthMask(GL_TRUE);

}

void EnvSkybox::DeleteTextures()
{

    glDeleteTextures(6, tex);

}
