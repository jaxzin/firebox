#include "enventrance.h"

EnvEntrance::EnvEntrance(const QString & u, const int s) :
    maxlen(30),
    size(s),
    startedurlrequest(false),
    processed(false),
    page(NULL),
    parentroom(NULL),
    childroom(NULL)
{

    QString urlstr = QUrl::fromPercentEncoding(u.toAscii());
    url = QUrl(urlstr);        

    page = new HTMLPage(url);

    anim.Scale(1.0f);

    urlstr.remove(0, 7); //ditch the "http://"
    if (urlstr.length() < maxlen) {
        textgeom.Text(urlstr);
    }
    else {
        textgeom.Text(urlstr.left(maxlen/2) + QString("...") + urlstr.right(maxlen/2));
    }

    textgeom.MaxSize(s, 0.1f);

}

QVector3D EnvEntrance::Dir() const
{
    return dir;
}

void EnvEntrance::SetDir(const QVector3D & d)
{
    dir = d;
    anim.Dir(d * -1.0f);
}

CellDir EnvEntrance::CDir() const
{
    return celldir;
}

void EnvEntrance::SetCDir(const CellDir & d)
{
    celldir = d;
}

void EnvEntrance::SetBounds(const int x1, const int y1, const int x2, const int y2)
{
    bx1 = (x1 < x2) ? x1 : x2;
    bx2 = (x2 > x1) ? x2 : x1;
    by1 = (y1 < y2) ? y1 : y2;
    by2 = (y2 > y1) ? y2 : y1;

    anim.Pos(BoundsMid() + QVector3D(0, 2.0f, 0));

}

void EnvEntrance::GetBounds(int & x1, int & y1, int & x2, int & y2) const
{
    x1 = bx1;
    x2 = bx2;
    y1 = by1;
    y2 = by2;
}

QVector3D EnvEntrance::BoundsMid() const
{
    return QVector3D((bx1+bx2+1) * 0.5f, 0.0f, (by1+by2+1) * 0.5f);
}

bool EnvEntrance::Loaded() const
{
    return page->Loaded();
}

bool EnvEntrance::StartedURLRequest() const
{
    return startedurlrequest;
}

bool EnvEntrance::Processed() const
{
    return processed;
}

void EnvEntrance::Processed(bool b)
{
    processed = b;
}

HTMLPage * EnvEntrance::Page() const
{
    return page;
}

void EnvEntrance::StartURLRequest()
{

    page->Request();
    startedurlrequest = true;

    anim.Animate(true);

}

void EnvEntrance::DrawGL()
{

    if (processed) {
        return;
    }

    glColor3f(0.6f, 0.6f, 1.0f);

    if (anim.Animate()) {
        anim.DrawGL();
    }

    glPushMatrix();
    MathUtil::FacePosDirGL(BoundsMid() + QVector3D(0, 1.75, 0), dir * (-1.0f));
    glTranslatef(0, 0, -0.4f);
    textgeom.DrawGL();
    glPopMatrix();

}

bool EnvEntrance::Near(const QVector3D & p, const float dist) const
{

    QVector3D midpt(bx1+bx2+1, p.y(), by1+by2+1);
    midpt *= 0.5f;

    return (p-midpt).lengthSquared() < dist*dist;

}

void EnvEntrance::ParentRoom(EnvRoom * r)
{
    parentroom = r;
}

EnvRoom * EnvEntrance::ParentRoom() const
{
    return parentroom;
}

void EnvEntrance::ChildRoom(EnvRoom * r)
{
    childroom = r;
}

EnvRoom * EnvEntrance::ChildRoom() const
{
    return childroom;
}

