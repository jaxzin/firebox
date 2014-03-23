/*
 * My own awesome OBJ class.
 *
 * James McCrae, April 12, 2011
 */

#ifndef OBJFILE_H
#define OBJFILE_H

#include <QtOpenGL>

#include <QtCore>
#include <cfloat>

struct Face {
    QVector <int> vi;
    QVector <int> ni;
    QVector <int> uvi;

    int mati;
};

struct Material {
    QString name;
    GLfloat * kd;
    GLfloat * ka;
    QString map_kd;
    GLuint texindex;
};

class ObjFile
{

public:

    ObjFile(const QString path);
    void Draw();
    void SetUseMaterials(bool b);

    static GLuint LoadTexture(QString file, int * width = NULL, int * height = NULL);

    float MaxX();
    float MaxY();

private:

    void LoadMaterials(QString mtlpath);

    QString m_basePath;

    QVector <QVector3D> m_verts;
    QVector <QVector3D> m_norms;
    QVector <QVector2D> m_uvs;

    QVector <Face> m_faces;

    QVector <Material> m_mats;

    float maxx;
    float maxy;

    bool usemat;

};

#endif // OBJFILE_H
