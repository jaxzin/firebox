#include "mathutil.h"

float MathUtil::_180_BY_PI = 180.0f / 3.14159f;
float MathUtil::_PI_BY_180 = 3.14159f / 180.0f;
float MathUtil::_PI = 3.14159f;
float MathUtil::_PI_OVER_2 = 3.14159f / 2.0f;

MathUtil::MathUtil()
{
}

float MathUtil::DegToRad(float f)
{
    return f * _PI_BY_180;
}

float MathUtil::RadToDeg(float f)
{
    return f * _180_BY_PI;
}

bool MathUtil::GetRayTriIntersect(const QVector3D & rayp, const QVector3D & rayd, const Triangle3D & tri, QVector3D & ipt)
{

    QVector3D e1 = tri.p2 - tri.p1;
    QVector3D e2 = tri.p3 - tri.p1;

    // Find the cross product of edge2 and the ray direction
    QVector3D s1 = QVector3D::crossProduct(rayd, e2);

    // Find the divisor, if its zero, return false as the triangle is degenerate
    float divisor = QVector3D::dotProduct(s1, e1);
    if (divisor == 0.0f) {
        return false;
    }

    // A inverted divisor, as multipling is faster then division
    float invDivisor = 1.0f / divisor;

    // Calculate the first barycentic coordinate. Barycentic coordinates
    // are between 0.0 and 1.0
    QVector3D distance = rayp - tri.p1;
    float barycCoord_1 = QVector3D::dotProduct(distance, s1) * invDivisor;

    if (barycCoord_1 < 0.0f || barycCoord_1 > 1.0f) {
        return false;
    }

    // Calculate the second barycentic coordinate
    QVector3D s2 = QVector3D::crossProduct(distance, e1);
    float barycCoord_2 = QVector3D::dotProduct(rayd, s2) * invDivisor;
    if (barycCoord_2 < 0.0f || (barycCoord_1 + barycCoord_2) > 1.0f) {
        return false;
    }

    // After doing the barycentic coordinate test we know if the ray hits or
    // not. If we got this far the ray hits.
    // Calculate the distance to the intersection point
    float intdist = QVector3D::dotProduct(e2, s2) * invDivisor;
    ipt = rayp + rayd * intdist;
    //rayp.Debug("rayp");
    //rayd.Debug("rayd");
    //qDebug() << "intdist" << intdist;
    return true;

}

void MathUtil::FacePosDirGL(QVector3D pos, QVector3D dir)
{

    QVector3D c = QVector3D::crossProduct(QVector3D(0, 0, 1), dir);
    if (c.length() < 0.001f)
        c = QVector3D(0, 1, 0);
    c.normalize();

    float angle = QVector3D::dotProduct(QVector3D(0, 0, 1), dir);
    angle = acosf(angle);
    angle = MathUtil::RadToDeg(angle);

    glTranslatef(pos.x(), pos.y(), pos.z());
    glRotatef(angle, c.x(), c.y(), c.z());

}

QVector3D MathUtil::Slerp(QVector3D p1, QVector3D p2, float i)
{

    p1.normalize();
    p2.normalize();

    QVector3D c = QVector3D::crossProduct(p1, p2);
    if (c.length() < 0.001f)
        c = QVector3D(0, 1, 0);
    c.normalize();

    float angle = acosf(QVector3D::dotProduct(p1, p2));

    return GetRotatedAxis(i * angle, p1, c);

}

QVector3D MathUtil::CosInterp(QVector3D p1, QVector3D p2, float i)
{

    float interp = (cosf(i * _PI) + 1.0f) / 2.0f;
    return p1 * interp + p2 * (1.0f - interp);

}


QVector3D MathUtil::GetRotatedAxis(const float anglerad, const QVector3D & vec, const QVector3D & axis) {

    if (anglerad == 0.0f)
        return vec;

    const float sinAngle = sinf(anglerad);
    const float cosAngle = cosf(anglerad);
    const float oneSubCos = 1.0f - cosAngle;

    const float x = axis.x();
    const float y = axis.y();
    const float z = axis.z();

    QVector3D R1, R2, R3;   

    R1.setX( x * x + cosAngle * (1.0f - x * x) );
    R1.setY( x * y * oneSubCos - sinAngle * z );
    R1.setZ( x * z * oneSubCos + sinAngle * y );

    R2.setX(  x * y * oneSubCos + sinAngle * z );
    R2.setY(  y * y + cosAngle * (1.0f - y * y) );
    R2.setZ(  y * z * oneSubCos - sinAngle * x );

    R3.setX(  x * z * oneSubCos - sinAngle * y );
    R3.setY(  y * z * oneSubCos + sinAngle * x );
    R3.setZ(  z * z + cosAngle * (1.0f - z * z) );

    return QVector3D(QVector3D::dotProduct(vec, R1),
                   QVector3D::dotProduct(vec, R2),
                   QVector3D::dotProduct(vec, R3));

}

void MathUtil::SphereToCartesian(const float thetadeg, const float phideg, const float r, QVector3D & p)
{

    const float thetarad = MathUtil::DegToRad(thetadeg);
    const float phirad= MathUtil::DegToRad(phideg);

    p.setX( r * sinf(phirad) * cosf(thetarad) );
    p.setY( r * cosf(phirad) );
    p.setZ( r * sinf(phirad) * sinf(thetarad) );

}

void MathUtil::CartesianToSphere(const QVector3D & p, float & thetadeg, float & phideg, float & r)
{

    thetadeg = MathUtil::RadToDeg(atan2f(p.z(), p.x()));
    phideg = MathUtil::RadToDeg(acosf(p.y() / p.length()));
    r = p.length();

}

void MathUtil::NormSphereToCartesian(const float thetadeg, const float phideg, QVector3D & p)
{

    const float thetarad = MathUtil::DegToRad(thetadeg);
    const float phirad= MathUtil::DegToRad(phideg);

    p.setX( sinf(phirad) * cosf(thetarad) );
    p.setY( cosf(phirad) );
    p.setZ( sinf(phirad) * sinf(thetarad) );

}

void MathUtil::NormCartesianToSphere(const QVector3D & p, float & thetadeg, float & phideg)
{

    thetadeg = MathUtil::RadToDeg(atan2f(p.z(), p.x()));
    phideg = MathUtil::RadToDeg(acosf(p.y() / p.length()));

}
