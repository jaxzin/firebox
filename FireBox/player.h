#ifndef PLAYER_H
#define PLAYER_H

#include <QtOpenGL>

#include <GL/glu.h>

#include "mathutil.h"

class Player
{

public:

    Player();

    void Pos(float x, float y, float z);
    void Pos(const QVector3D & p);
    QVector3D Pos() const;
    void Dir(const QVector3D & d);
    QVector3D Dir() const;
    QVector3D WalkDir() const;

    void Height(const float h);
    float Height() const;

    void FlyTo(const QVector3D & p, const QVector3D & d, const float duration);

    void Flying(const bool b);
    bool Flying() const;

    void SpinView(float f);
    void TiltView(float f);

    void WalkForward(bool b);
    void WalkBack(bool b);
    void WalkLeft(bool b);
    void WalkRight(bool b);

    void SetViewGL(const float half_ipd, const double rot[16]);

    QVector3D VelocityVector();
    void Update();

private:

    void UpdateDir();

    QVector3D pos;
    QVector3D vel;
    float maxvel;

    QVector3D dir;
    QVector3D walk_dir;
    float theta;
    float phi;    

    float height;

    bool walkForward;
    bool walkBack;
    bool walkLeft;
    bool walkRight;

    QTime time;

    bool flying;
    float flyinterp;
    float flyduration;
    QVector3D flypos_start;
    QVector3D flypos_end;
    QVector3D flydir_start;
    QVector3D flydir_end;

};

#endif // PLAYER_H
