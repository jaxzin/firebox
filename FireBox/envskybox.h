#ifndef ENVSKYBOX_H
#define ENVSKYBOX_H

#include <QtOpenGL>
#include "envobject.h"

class EnvSkybox
{

public:

    EnvSkybox();
    ~EnvSkybox();

    void LoadTextures();

    void DrawGL();

private:

    void DeleteTextures();

    GLuint tex[6]; //cubemap textures

};

#endif // ENVSKYBOX_H
