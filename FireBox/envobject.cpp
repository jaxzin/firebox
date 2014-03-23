#include "envobject.h"

EnvObject::EnvObject()
{
}

EnvObject::~EnvObject()
{

}

void EnvObject::Pos(const float x, const float y, const float z)
{
    pos = QVector3D(x, y, z);
}

void EnvObject::Pos(const QVector3D & p)
{
    pos = p;
}

QVector3D EnvObject::Pos() const
{
    return pos;
}

void EnvObject::Dir(const QVector3D & p)
{
    dir = p;
}

QVector3D EnvObject::Dir() const
{
    return dir;
}

GLuint EnvObject::LoadTexture(const QImage & img)
{

    if (img.isNull()) {
        qDebug() << "Image is null!";
    }

    QImage scaledimg = img;
    //scale the image
    if (scaledimg.width() > 512) {
        scaledimg = scaledimg.scaledToWidth(512, Qt::SmoothTransformation);
    }
    if (scaledimg.height() > 512) {
        scaledimg = scaledimg.scaledToHeight(512, Qt::SmoothTransformation);
    }

    QImage glimg;
    glimg = QGLWidget::convertToGLFormat( scaledimg );

    GLuint texid;

    glGenTextures(1, &texid);
    glBindTexture(GL_TEXTURE_2D, texid);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, 4, glimg.width(), glimg.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, glimg.bits());

    glBindTexture(GL_TEXTURE_2D, 0);

    return texid;

}
