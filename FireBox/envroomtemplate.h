#ifndef ENVROOMTEMPLATE_H
#define ENVROOMTEMPLATE_H

#include <QtCore>
#include <QtOpenGL>

#include "objfile.h"
#include "envobject.h"
#include "mathutil.h"

struct CircCollider {

    QPointF pos;
    float rad;
    bool stay_inside;

};

class EnvRoomTemplate
{

public:

    EnvRoomTemplate();

    void Load(const QString & obj_filename, const QString & tex_filename, const QString & data_filename);
    void DrawGL();

    QVector3D ClipPlayerTrajectory(const QVector3D & pos, const QVector3D & vel) const;

    int GetNumMounts() const;
    void GetMount(const int index, QVector3D & pt, QVector3D & dir) const;    

private:

    void LoadObj(const QString & filename);
    void LoadTexture(const QString & filename);
    void LoadData(const QString & filename);

    QList <QVector3D> obj_verts;
    QList <QVector2D> obj_texcoords;
    QList <int> obj_face_verts;
    QList <int> obj_face_texcoords;

    GLuint texture;

    QList <QRectF> colliders;
    QList <CircCollider> circ_colliders;
    QList <QVector3D> mount_pts;
    QList <QVector3D> mount_dirs;

    GLuint disp_list;

};

#endif // ENVROOMTEMPLATE_H
