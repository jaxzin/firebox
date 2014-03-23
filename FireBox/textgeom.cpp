#include "textgeom.h"

QVector <ObjFile *> TextGeom::objs;

TextGeom::TextGeom() :    
    do_fixed_size(false),
    maxx(1.0f),
    maxy(1.0f),
    len(0.0f),
    pos(0, 0, 0),
    dir(1, 0, 0),
    disp_list(0),
    rebuild_disp_list(false)
{   
    if (objs.empty()) {

        objs.resize(128);
        for (int i=0; i<128; ++i) {

            objs[i] = NULL;

            QString path = QString("./assets/asciigeom/") + QString::number(i) + QString(".obj");

            QFile infile( path );
            if (!infile.exists()) {
                //qDebug() << "Warning - Couldn't find file" << path;
                continue;
            }

            objs[i] = new ObjFile(path);
            objs[i]->SetUseMaterials(false);

        }

    }

}

TextGeom::~TextGeom()
{
    DeleteDisplayList();
}

void TextGeom::MaxSize(float x, float y)
{
    maxx = x;
    maxy = y;
}

void TextGeom::Text(const QString & s)
{

    rebuild_disp_list = true;
    text = s.toUpper().toLatin1();

    const char * data = text.data();

    len = 0.0f;    
    height = 0.0f;
    for (int i=0; i<text.length(); ++i) {

        if (objs[data[i]] == NULL)
            len += 2.5f;
        else {

            if (do_fixed_size) {
                len += objs[data[i]]->MaxX() * fixed_size;
            }
            else {
                len += objs[data[i]]->MaxX() + 0.1f;
            }

            height = qMax(objs[data[i]]->MaxY(), height);
        }

    }

}

QString TextGeom::Text() const
{
    return QString(text);
}

float TextGeom::TextLength() const
{
    return len;
}

void TextGeom::SetFixedSize(const bool b, const float f)
{

    do_fixed_size = b;
    fixed_size = f;
    rebuild_disp_list = true;

}

void TextGeom::DrawGL()
{

    if (disp_list > 0 && !rebuild_disp_list) {

        glCallList(disp_list);

    }
    else {

        rebuild_disp_list = false;

        const char * data = text.data();
        float s;

        if (do_fixed_size) {
            s = fixed_size;
        }
        else {
            s = qMin(maxx / len, maxy / height);
        }

        DeleteDisplayList();
        disp_list = glGenLists(1);
        glNewList(disp_list, GL_COMPILE_AND_EXECUTE);

        glPushMatrix();

        glScalef(s, s, 1);
        glTranslatef(-len / 2.0f, 0, 0);

        for (int i=0; i<text.length(); ++i) {

            if (objs[data[i]] == NULL) {
                glTranslatef(2.5f, 0.0f, 0.0f);
            }
            else {
                objs[data[i]]->Draw();
                glTranslatef(objs[data[i]]->MaxX() + 0.1f, 0.0f, 0.0f);

            }

        }

        glPopMatrix();

        glEndList();

    }


}

void TextGeom::DeleteDisplayList()
{

    if (disp_list > 0) {
        glDeleteLists(disp_list, 1);
    }
    disp_list = 0;

}
