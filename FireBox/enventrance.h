#ifndef ENVENTRANCE_H
#define ENVENTRANCE_H

#include "htmlpage.h"
#include "spinanimation.h"
#include "envroom.h"

class EnvRoom;

class EnvEntrance
{

public:

    EnvEntrance(const QString & u, const int s);

    void StartURLRequest();

    void SetBounds(const int x1, const int y1, const int x2, const int y2);
    void GetBounds(int & x1, int & y1, int & x2, int & y2) const;
    QVector3D BoundsMid() const;

    bool Loaded() const;
    bool StartedURLRequest() const;
    bool Processed() const;
    void Processed(bool b);

    QVector3D Dir() const;
    void SetDir(const QVector3D & d);
    CellDir CDir() const;
    void SetCDir(const CellDir & d);

    bool Near(const QVector3D & p, const float dist) const;

    void ParentRoom(EnvRoom * r);
    EnvRoom * ParentRoom() const;

    void ChildRoom(EnvRoom * r);
    EnvRoom * ChildRoom() const;

    void DrawGL();

    HTMLPage * Page() const;

private:

    const int maxlen;
    int bx1, bx2, by1, by2;
    int size;

    QUrl url;
    bool startedurlrequest;
    bool processed;

    QVector3D dir;
    CellDir celldir;

    HTMLPage * page;

    SpinAnimation anim;
    TextGeom textgeom;

    EnvRoom * parentroom;
    EnvRoom * childroom;

};

#endif // ENVENTRANCE_H
