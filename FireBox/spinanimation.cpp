#include "spinanimation.h"

GLuint SpinAnimation::displist = 0;

SpinAnimation::SpinAnimation() :    
    pos(0, 0, 0),
    dir(0, 0, 1),
    scale(1.0f),
    animate(false)
{
    time.start();    
    loadinggeom.Text("loading...");    
    loadinggeom.MaxSize(0.5f, 0.25f);
}

void SpinAnimation::Pos(const QVector3D & v) {
    pos = v;
}

void SpinAnimation::Pos(float x, float y, float z)
{
    pos = QVector3D(x, y, z);
}

QVector3D SpinAnimation::Pos() const
{
    return pos;
}

void SpinAnimation::Dir(const QVector3D & v)
{
    dir = v;
}

void SpinAnimation::Dir(float x, float y, float z)
{
    dir = QVector3D(x, y, z);
}

QVector3D SpinAnimation::Dir() const
{
    return dir;
}

void SpinAnimation::Scale(const float f)
{
    scale = f;
}

float SpinAnimation::Scale() const
{
    return scale;
}

void SpinAnimation::Animate(bool b)
{
    animate = b;
}

bool SpinAnimation::Animate() const
{
    return animate;
}

void SpinAnimation::DrawGL()
{   

    if (!animate) {
        return;
    }

    float curtime = float(time.elapsed()) / 1000.0f;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //glColor4f(0.6f, 1, 0.6f, 1.0f);

    //SPINNER
    glPushMatrix();

    MathUtil::FacePosDirGL(pos, dir);
    glTranslatef(0, 0, -0.5f);
    loadinggeom.DrawGL();

    glRotatef(-curtime * 360.0f, 0, 0, 1);
    glScalef(scale, scale, scale);
    DrawSpinnerGL();

    glPopMatrix();

    glDisable(GL_BLEND);

}

void SpinAnimation::DrawSpinnerGL()
{

    if (displist == 0) {

        displist = glGenLists(1);
        glNewList(displist, GL_COMPILE_AND_EXECUTE);

        glBegin(GL_QUADS);
        const float fincr = MathUtil::_PI/60;

        for (float f=0.0f; f<MathUtil::_PI * 2.0f; f+=fincr) {

            glColor4f(0.8f, 1.0f, 0.8f, f / (MathUtil::_PI * 2.0f));

            glVertex2f(sinf(f), cosf(f));
            glVertex2f(sinf(f) * 0.5f, cosf(f) * 0.5f);
            glVertex2f(sinf(f + fincr) * 0.5f, cosf(f + fincr) * 0.5f);
            glVertex2f(sinf(f + fincr), cosf(f + fincr));
        }
        glEnd();

        glEndList();

    }
    else {
        glCallList(displist);
    }

}
