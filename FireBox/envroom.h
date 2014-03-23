#ifndef ENVROOM_H
#define ENVROOM_H

#include "htmlpage.h"
#include "spinanimation.h"
#include "envcell.h"
#include "envimg.h"
#include "envtext.h"
#include "enventrance.h"

class EnvEntrance;

class EnvRoom
{

public:

    EnvRoom(const int i);

    void SetBounds(int x1, int y1, int x2, int y2);
    void GetBounds(int & x1, int & y1, int & x2, int & y2) const;

    void AddEnvImage(float x, float y, CellDir d, const QImage & image);
    void AddEnvImage(float x, float y, CellDir d, QString url);

    void AddEnvText(int x, int y, CellDir d, QString text);

    bool Inside(const QVector3D & p) const;

    int RoomIndex() const;

    void ParentEntrance(EnvEntrance * e);
    EnvEntrance * ParentEntrance();
    void AddChildEntrance(EnvEntrance * e);
    QVector <EnvEntrance *> ChildEntrances();

    void DrawGL();

private:  

    int bx1, bx2, by1, by2;
    int room_index;

    QVector <EnvImg *> envimages;
    QVector <EnvText *> envtexts;

    EnvEntrance * parententrance;
    QVector <EnvEntrance *> childentrances;

};

#endif // ENVROOM_H
