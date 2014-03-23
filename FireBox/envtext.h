#ifndef ENVTEXT_H
#define ENVTEXT_H

#include <QtCore>

#include "envobject.h"
#include "mathutil.h"

class EnvText : public EnvObject
{

public:

    EnvText(const QString & t);
    ~EnvText();   

    void DrawGL();    

    static bool WorthyOfTextBox(const QString & t);

private:

    QString text;

    QImage img;
    GLuint tex;

    int tex_width;
    int tex_height;

    QTime scroll_time;

};

#endif // ENVTEXT_H
