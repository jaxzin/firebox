#include "envtitle.h"

EnvTitle::EnvTitle(int s) :
    maxlen(30),
    size(s)
{
    textgeom.MaxSize(size + 0.75f, 0.25f);
}

void EnvTitle::Title(const QString & s)
{

    if (s.length() < maxlen) {
        textgeom.Text(s);
    }
    else {
        textgeom.Text(s.left(maxlen) + QString("..."));
    }
}

void EnvTitle::DrawGL()
{

    const float w = float(size + 1) / 2.0f;

    glPushMatrix();

    MathUtil::FacePosDirGL(pos, dir);   

    glColor3f(1, 1, 1);
    glBegin(GL_QUADS);
    glVertex3f(-w, 3.5f, -0.5f);
    glVertex3f(w, 3.5f, -0.5f);
    glVertex3f(w, 4.0f, -0.5f);
    glVertex3f(-w, 4.0f, -0.5f);
    glEnd();


    glColor3f(.5,.5,.5);
    glTranslatef(0.0f, 3.6f, -0.45f);
    textgeom.DrawGL();

    glPopMatrix();

}
