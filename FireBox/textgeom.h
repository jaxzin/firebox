#ifndef TEXTGEOM_H
#define TEXTGEOM_H

#include <QtOpenGL>

#include "objfile.h"
#include "mathutil.h"

class TextGeom
{

public:

    TextGeom();
    ~TextGeom();

    void MaxSize(float x, float y);

    void Text(const QString & s);
    QString Text() const;

    float TextLength() const;

    void SetFixedSize(const bool b, const float f);

    void DrawGL();

private:

    void DeleteDisplayList();

    static QVector <ObjFile *> objs;
    QByteArray text;

    bool do_fixed_size;
    float fixed_size;

    float maxx;
    float maxy;
    float len;
    float height;
    float textlength;

    QVector3D pos;
    QVector3D dir;

    int disp_list;
    bool rebuild_disp_list;

};

#endif // TEXTGEOM_H
