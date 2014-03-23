#ifndef ENVTITLE_H
#define ENVTITLE_H

#include "envobject.h"
#include "textgeom.h"
#include "mathutil.h"

class EnvTitle : public EnvObject
{

public:

    EnvTitle(int s);

    void Title(const QString & s);

    void DrawGL();

private:

    const int maxlen;
    int size;
    TextGeom textgeom;

};

#endif // ENVTITLE_H
