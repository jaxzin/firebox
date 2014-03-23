#ifndef ENVOBJECT_H
#define ENVOBJECT_H

#include <QtOpenGL>

class EnvObject
{

public:

    EnvObject();
    virtual ~EnvObject();

    void Pos(const float x, const float y, const float z);
    void Pos(const QVector3D & p);
    QVector3D Pos() const;

    void Dir(const QVector3D & p);
    QVector3D Dir() const;

    virtual void DrawGL() = 0;

    static GLuint LoadTexture(const QImage & img, const bool downscale);

protected:

    QVector3D pos;
    QVector3D dir;

};

#endif // ENVOBJECT_H
