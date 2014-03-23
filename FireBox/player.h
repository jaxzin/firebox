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

    void Height(const float h);
    float Height() const;

    void FlyTo(const QVector3D & p, const QVector3D & d, const float duration);

    void Flying(const bool b);
    bool Flying() const;

    void FlightMode(const bool b);
    bool FlightMode() const;

    float Theta() const;
    void SpinView(float f);
    void TiltView(float f);

    void WalkForward(bool b);
    void WalkBack(bool b);
    void WalkLeft(bool b);
    void WalkRight(bool b);

    void SetViewGL(const float half_ipd, const QVector3D & up, const QVector3D & forward);

    void Velocity(const QVector3D & v);
    QVector3D Velocity() const;
    void Update();

private:

    void UpdateDir();

    QVector3D pos;    
    QVector3D vel;
    float maxvel;     

    QVector3D dir;
    float theta;
    float phi;    

    float height;

    bool walkForward;
    bool walkBack;
    bool walkLeft;
    bool walkRight;

    QTime time;

    bool flight_mode;

    bool flying;    
    float flyinterp;
    float flyduration;
    QVector3D flypos_start;
    QVector3D flypos_end;
    QVector3D flydir_start;
    QVector3D flydir_end;

};

#endif // PLAYER_H
