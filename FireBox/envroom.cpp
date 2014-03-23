#include "envroom.h"

EnvRoom::EnvRoom(const int i) :
    room_index(i),
    parententrance(NULL)
{

}

void EnvRoom::GetBounds(int & x1, int & y1, int & x2, int & y2) const
{
    x1 = bx1;
    x2 = bx2;
    y1 = by1;
    y2 = by2;
}

void EnvRoom::SetBounds(int x1, int y1, int x2, int y2)
{
    bx1 = (x1 < x2) ? x1 : x2;
    bx2 = (x2 > x1) ? x2 : x1;
    by1 = (y1 < y2) ? y1 : y2;
    by2 = (y2 > y1) ? y2 : y1;
}

void EnvRoom::DrawGL()
{

    for (int i=0; i<envimages.size(); ++i) {
        envimages[i]->DrawGL();
    }

    for (int i=0; i<envtexts.size(); ++i) {
        envtexts[i]->DrawGL();
    }

}

void EnvRoom::AddEnvImage(float x, float y, CellDir d, const QImage & image)
{

    EnvImg * newimg = new EnvImg(image);
    newimg->Pos(x + 0.5f, 2.0f, y + 0.5f);
    newimg->Dir(EnvCell::CellDirToVector(d) * -1.0f);
    envimages.push_back(newimg);

}

void EnvRoom::AddEnvImage(float x, float y, CellDir d, QString url)
{

    QUrl theurl(url);

    EnvImg * newimg = new EnvImg(theurl);
    newimg->Pos(x + 0.5f, 2.0f, y + 0.5f);
    newimg->Dir(EnvCell::CellDirToVector(d) * -1.0f);
    envimages.push_back(newimg);

}

void EnvRoom::AddEnvText(int x, int y, CellDir d, QString text)
{
    EnvText * newtext = new EnvText(text);
    newtext->Pos(x+0.5f, 2.0f, y + 0.5f);
    newtext->Dir(EnvCell::CellDirToVector(d) * -1.0f);
    envtexts.push_back(newtext);
}

void EnvRoom::ParentEntrance(EnvEntrance * e)
{
    parententrance = e;
}

EnvEntrance * EnvRoom::ParentEntrance()
{
    return parententrance;
}

void EnvRoom::AddChildEntrance(EnvEntrance * e)
{
    childentrances.push_back(e);
}

QVector <EnvEntrance *> EnvRoom::ChildEntrances()
{
    return childentrances;
}

bool EnvRoom::Inside(const QVector3D & p) const
{

    if (p.x() > bx1 && p.x() < bx2 && p.z() > by1 && p.z() < by2)
        return true;
    else
        return false;

}

int EnvRoom::RoomIndex() const
{

    return room_index;

}
