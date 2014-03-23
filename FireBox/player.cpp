#include "player.h"

Player::Player() :
    pos(0, 0, 0),
    vel(0, 0, 0),
    maxvel(8.0f),
    theta(0.0f),
    phi(90.0f),
    walkForward(false),
    walkBack(false),
    walkLeft(false),
    walkRight(false),
    flying(false)
{

    UpdateDir();
    time.start();

}

QVector3D Player::Pos() const
{
    return pos;
}

void Player::Pos(const QVector3D & p)
{
    pos = p;
}

void Player::Pos(float x, float y, float z)
{
    pos = QVector3D(x, y, z);
}

void Player::Dir(const QVector3D & d)
{

    dir = d;
    dir.normalize();

    MathUtil::NormCartesianToSphere(dir, theta, phi);

}

QVector3D Player::Dir() const
{
    return dir;
}

void Player::UpdateDir()
{    
    MathUtil::NormSphereToCartesian(theta, phi, dir);
}

void Player::SpinView(float f)
{

    theta += f;

    while (theta > 360.0f) theta -= 360.0f;
    while (theta < 0.0f) theta += 360.0f;

    //UpdateDir();

}

void Player::TiltView(float f)
{

    phi += f;

    if (phi <= 5.0f)
        phi = 5.0f;
    if (phi > 175.0f)
        phi = 175.0f;

    //UpdateDir();

}

void Player::SetViewGL(const float half_ipd, const QVector3D & right, const QVector3D & up, const QVector3D & forward)
{


    /*
    dir = QVector3D(rot[4], rot[5], rot[6]);
    //dir.setY(-dir.y()); //JAMES CHANGE
    dir = -dir;

    QVector3D v = dir;
    QVector3D u = QVector3D(rot[8], -rot[9], rot[10]); //JAMES CHANGE
    //QVector3D u = QVector3D(rot[8], rot[9], rot[10]);

    //rotate both view and up vector by theta
    const float theta_rad = MathUtil::DegToRad(theta);
    v = MathUtil::GetRotatedAxis(-theta_rad, v, QVector3D(0, 1, 0));
    u = MathUtil::GetRotatedAxis(-theta_rad, u, QVector3D(0, 1, 0));

    u.normalize();

    walk_dir = v;

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();    
    glTranslatef(-half_ipd, 0, 0);      
    v += pos;
    gluLookAt(pos.x(), pos.y(), pos.z(), v.x(), v.y(), v.z(), u.x(), u.y(), u.z());
    */

    const float theta_rad = MathUtil::DegToRad(theta);

    dir = MathUtil::GetRotatedAxis(-theta_rad, forward, QVector3D(0, 1, 0));
    const QVector3D up_rotated = MathUtil::GetRotatedAxis(-theta_rad, up, QVector3D(0, 1, 0));
    const QVector3D right_rotated = MathUtil::GetRotatedAxis(-theta_rad, right, QVector3D(0, 1, 0));

    QVector3D cent = pos + dir;

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glTranslatef(-half_ipd, 0, 0);
    gluLookAt(pos.x(), pos.y(), pos.z(), cent.x(), cent.y(), cent.z(), up_rotated.x(), up_rotated.y(), up_rotated.z());

    //qDebug() << rot[0] << rot[1] << rot[2] << rot[4] << rot[5] << rot[6] << rot[8] << rot[9] << rot[10];


    /*
    GLdouble model_view[16];
    glGetDoublev(GL_MODELVIEW_MATRIX, model_view);

    qDebug() << "modelview:" << QVector3D(model_view[0], model_view[1], model_view[2]).length()
            << QVector3D(model_view[4], model_view[5], model_view[6]).length()
            << QVector3D(model_view[8], model_view[9], model_view[10]).length();
            */

}

void Player::WalkForward(bool b)
{
    walkForward = b;
    //vel = QVector3D(0, 0, 0);
}

void Player::WalkBack(bool b)
{
    walkBack = b;
    //vel = QVector3D(0, 0, 0);
}

void Player::WalkLeft(bool b)
{
    walkLeft = b;
}

void Player::WalkRight(bool b)
{
    walkRight = b;
}

void Player::Height(const float h)
{
    height = h;
}

float Player::Height() const
{
    return height;
}

void Player::FlyTo(const QVector3D & p, const QVector3D & d, const float duration)
{

    flying = true;
    flyinterp = 0.0f;
    flypos_start = pos;
    flydir_start = dir;
    flypos_end = p;
    flydir_end = d;
    flyduration = duration;
    time.start();

}

void Player::Flying(const bool b)
{
    flying = b;
}

bool Player::Flying() const
{
    return flying;
}

QVector3D Player::VelocityVector()
{
    return vel;
}

void Player::Update()
{

    if (flying) {

        flyinterp = float(time.elapsed()) / flyduration;

        if (flyinterp >= 1.0f || pos == flypos_end) {

            flying = false;
            Pos(flypos_end);

            //Dir(flydir_end);

        }
        else {
            Pos(MathUtil::CosInterp(flypos_start, flypos_end, flyinterp));
            //Dir(MathUtil::Slerp(flydir_start, flydir_end, flyinterp));
        }

        //qDebug() << "Flying " << flyinterp << pos << dir;

    }
    else {

        float deltat = float(time.restart()) / 1000.0f;

        QVector3D walk_dir = dir;
        walk_dir.setY(0.0f);
        QVector3D c = QVector3D::crossProduct(dir, QVector3D(0, 1, 0));
        c.normalize();

        vel = QVector3D(0, 0, 0);

        if (walkForward) {
            vel += walk_dir * maxvel;
        }
        if (walkBack) {
            vel += walk_dir * maxvel * (-1.0f);
        }
        if (walkLeft) {
            vel += c * maxvel * (-1.0f);
        }
        if (walkRight) {
            vel += c * maxvel;
        }

        vel.setY(0.0f);
        if (vel.length() > maxvel) {
            vel.normalize();
            vel *= maxvel;
        }

        vel *= deltat;

        //if (walkForward || walkBack || walkLeft || walkRight) {
            //pos += vel * deltat;
        //}

    }

    //qDebug() << walkBack << walkForward << vel;

}
