#ifndef MATHUTIL_H
#define MATHUTIL_H

#include <QtOpenGL>
#include <QVector3D>


struct Triangle3D
{
    QVector3D p1;
    QVector3D p2;
    QVector3D p3;
};

class MathUtil
{
public:

    MathUtil();

    static float DegToRad(float f);
    static float RadToDeg(float f);
    static bool GetRayTriIntersect(const QVector3D & rayp, const QVector3D & rayd, const Triangle3D & tri, QVector3D & ipt);

    static void FacePosDirGL(QVector3D pos, QVector3D dir);

    static QVector3D GetRotatedAxis(const float anglerad, const QVector3D & vec, const QVector3D & axis);

    static void SphereToCartesian(const float thetadeg, const float phideg, const float r, QVector3D & p);
    static void CartesianToSphere(const QVector3D & p, float & thetadeg, float & phideg, float & r);

    static void NormSphereToCartesian(const float thetadeg, const float phideg, QVector3D & p);
    static void NormCartesianToSphere(const QVector3D & p, float & thetadeg, float & phideg);

    static QVector3D Slerp(QVector3D p1, QVector3D p2, float i);
    static QVector3D CosInterp(QVector3D p1, QVector3D p2, float i);

    static float _180_BY_PI;
    static float _PI_BY_180;
    static float _PI;
    static float _PI_OVER_2;


};

#endif // MATHUTIL_H
