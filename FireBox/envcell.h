#ifndef ENVCELL_H
#define ENVCELL_H

#include <QtOpenGL>

#include "htmlpage.h"
#include "envobject.h"

enum CellDir {
    UP, // -y
    DOWN, // +y
    LEFT, // -x
    RIGHT // +x
};

enum CellState {
    EMPTY,
    OCCUPIED,
    ENTRANCE
};

class EnvCell
{

public:

    EnvCell();

    void InitGeometry();

    void DrawGL();

    void State(const CellState & s);
    CellState State() const;

    void Dir(CellDir d);
    CellDir Dir();
    static QVector3D CellDirToVector(CellDir d);
    static QString CellDirToName(CellDir d);

    void X(int v);
    int X();

    void Y(int v);
    int Y();

    void Owner(int o);
    int Owner();

    void Neighbours(EnvCell * l, EnvCell * r, EnvCell * u, EnvCell * d, EnvCell * ul, EnvCell * ur, EnvCell * dl, EnvCell * dr);
    bool EightNeighbourOwned();

    bool IsDrawingLeftWall();
    bool IsDrawingRightWall();
    bool IsDrawingUpWall();
    bool IsDrawingDownWall();

private:

    void ShadeFloor(QList <QVector3D> & ss);
    inline void DrawTileGL(const QVector <QVector3D> & vs, const QVector <QVector2D> & ts, const QVector3D & trans, const QVector3D & s1, const QVector3D & s2, const QVector3D & s3, const QVector3D & s4);
    inline void DrawQuad(const QVector3D & v1, const QVector3D & v2, const QVector3D & v3, const QVector3D & v4,
                         const QVector2D & t1, const QVector2D & t2, const QVector2D & t3, const QVector2D & t4,
                         const QVector3D & s1, const QVector3D & s2, const QVector3D & s3, const QVector3D & s4);
    inline void DrawVertex(const QVector3D & v, const QVector2D & t, const QVector3D & s);

    CellState state;
    CellDir dir;

    int x;
    int y;
    int owner;

    EnvCell * left;
    EnvCell * right;
    EnvCell * up;
    EnvCell * down;

    EnvCell * upleft;
    EnvCell * upright;
    EnvCell * downleft;
    EnvCell * downright;

    static QVector <QVector3D> geom_fvs; //floor tile
    static QVector <QVector3D> geom_cvs; //ceiling tile
    static QVector <QVector3D> geom_rvs; //right tile
    static QVector <QVector3D> geom_lvs; //left tile
    static QVector <QVector3D> geom_uvs; //up tile
    static QVector <QVector3D> geom_dvs; //down tile

    static QVector <QVector2D> geom_ts; //texcoords for tiles

};

#endif // ENVCELL_H
