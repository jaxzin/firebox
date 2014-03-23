#ifndef SPINANIMATION_H
#define SPINANIMATION_H

#include <QtOpenGL>

#include "mathutil.h"
#include "textgeom.h"

class SpinAnimation
{

public:

    SpinAnimation();

    void Pos(const QVector3D & v);
    void Pos(float x, float y, float z);
    QVector3D Pos() const;

    void Dir(const QVector3D & v);
    void Dir(float x, float y, float z);
    QVector3D Dir() const;

    void Scale(const float f);
    float Scale() const;

    void Animate(bool b);
    bool Animate() const;

    void DrawGL();

private:

    static void DrawSpinnerGL();

    QTime time;
    TextGeom loadinggeom;    
    QVector3D pos;
    QVector3D dir;
    float scale;
    bool animate;

    static GLuint displist;

};

#endif // SPINANIMATION_H
