#include "envcell.h"

QVector <QVector3D> EnvCell::geom_fvs;
QVector <QVector3D> EnvCell::geom_cvs;
QVector <QVector3D> EnvCell::geom_rvs;
QVector <QVector3D> EnvCell::geom_lvs;
QVector <QVector3D> EnvCell::geom_uvs;
QVector <QVector3D> EnvCell::geom_dvs;
QVector <QVector2D> EnvCell::geom_ts;

EnvCell::EnvCell() :
    state(EMPTY),
    x(0),
    y(0),
    owner(-1),
    left(NULL),
    right(NULL),
    up(NULL),
    down(NULL),
    upleft(NULL),
    upright(NULL),
    downleft(NULL),
    downright(NULL)

{

    if (geom_fvs.empty()) {

        InitGeometry();

    }

}

void EnvCell::InitGeometry()
{

    const float b = 0.1f;
    const float fhb = b;
    const float chb = -b;

    geom_fvs.push_back(QVector3D(0, 0, 0));
    geom_fvs.push_back(QVector3D(1, 0, 0));
    geom_fvs.push_back(QVector3D(1, 0, 1));
    geom_fvs.push_back(QVector3D(0, 0, 1));
    geom_fvs.push_back(QVector3D(0+b, fhb, 0+b));
    geom_fvs.push_back(QVector3D(1.0f-b, fhb, 0+b));
    geom_fvs.push_back(QVector3D(1.0f-b, fhb, 1.0f-b));
    geom_fvs.push_back(QVector3D(0+b, fhb, 1.0f-b));

    geom_cvs.push_back(QVector3D(0, 0, 0));
    geom_cvs.push_back(QVector3D(1, 0, 0));
    geom_cvs.push_back(QVector3D(1, 0, 1));
    geom_cvs.push_back(QVector3D(0, 0, 1));
    geom_cvs.push_back(QVector3D(0+b, chb, 0+b));
    geom_cvs.push_back(QVector3D(1.0f-b, chb, 0+b));
    geom_cvs.push_back(QVector3D(1.0f-b, chb, 1.0f-b));
    geom_cvs.push_back(QVector3D(0+b, chb, 1.0f-b));

    geom_lvs.push_back(QVector3D(0, 0, 1));
    geom_lvs.push_back(QVector3D(0, 0, 0));
    geom_lvs.push_back(QVector3D(0, 1, 0));
    geom_lvs.push_back(QVector3D(0, 1, 1));
    geom_lvs.push_back(QVector3D(0+b ,0+b, 1-b));
    geom_lvs.push_back(QVector3D(0+b, 0+b, 0+b));
    geom_lvs.push_back(QVector3D(0+b, 1-b, 0+b));
    geom_lvs.push_back(QVector3D(0+b, 1-b, 1-b));

    geom_uvs.push_back(QVector3D(0, 0, 0));
    geom_uvs.push_back(QVector3D(1, 0, 0));
    geom_uvs.push_back(QVector3D(1, 1, 0));
    geom_uvs.push_back(QVector3D(0, 1, 0));
    geom_uvs.push_back(QVector3D(0+b, 0+b, 0+b));
    geom_uvs.push_back(QVector3D(1-b, 0+b, 0+b));
    geom_uvs.push_back(QVector3D(1-b, 1-b, 0+b));
    geom_uvs.push_back(QVector3D(0+b, 1-b, 0+b));

    geom_rvs.push_back(QVector3D(1, 0, 0));
    geom_rvs.push_back(QVector3D(1, 0, 1));
    geom_rvs.push_back(QVector3D(1, 1, 1));
    geom_rvs.push_back(QVector3D(1, 1, 0));
    geom_rvs.push_back(QVector3D(1-b, 0+b, 0+b));
    geom_rvs.push_back(QVector3D(1-b, 0+b, 1-b));
    geom_rvs.push_back(QVector3D(1-b, 1-b, 1-b));
    geom_rvs.push_back(QVector3D(1-b, 1-b, 0+b));

    geom_dvs.push_back(QVector3D(1, 0, 1));
    geom_dvs.push_back(QVector3D(0, 0, 1));
    geom_dvs.push_back(QVector3D(0, 1, 1));
    geom_dvs.push_back(QVector3D(1, 1, 1));
    geom_dvs.push_back(QVector3D(1-b, 0+b, 1-b));
    geom_dvs.push_back(QVector3D(0+b, 0+b, 1-b));
    geom_dvs.push_back(QVector3D(0+b, 1-b, 1-b));
    geom_dvs.push_back(QVector3D(1-b, 1-b, 1-b));

    geom_ts.push_back(QVector2D(0, 0));
    geom_ts.push_back(QVector2D(1, 0));
    geom_ts.push_back(QVector2D(1, 1));
    geom_ts.push_back(QVector2D(0, 1));
    geom_ts.push_back(QVector2D(0+b, 0+b));
    geom_ts.push_back(QVector2D(1.0f-b, 0+b));
    geom_ts.push_back(QVector2D(1.0f-b, 1.0f-b));
    geom_ts.push_back(QVector2D(0+b, 1.0f-b));

}

bool EnvCell::IsDrawingLeftWall()
{
    return left == NULL || left->owner == -1; //(left->owner != owner && left->state != ENTRANCE && state != ENTRANCE);
}

bool EnvCell::IsDrawingRightWall()
{
    return right == NULL || right->owner == -1; // (right->owner != owner && right->state != ENTRANCE && state != ENTRANCE);
}

bool EnvCell::IsDrawingUpWall()
{
    return up == NULL || up->owner == -1; // (up->owner != owner && up->state != ENTRANCE && state != ENTRANCE);
}

bool EnvCell::IsDrawingDownWall()
{
    return down == NULL || down->owner == -1; // (down->owner != owner && down->state != ENTRANCE && state != ENTRANCE);
}

void EnvCell::DrawGL()
{

    if (owner == -1) {

        if (EightNeighbourOwned()) {

            QVector3D bright(0.75f, 0.75f, 0.75f);
            QVector3D dark(0.5f, 0.5f, 0.5f);
            DrawTileGL(geom_fvs, geom_ts, QVector3D(x, 1, y), bright, bright, bright, bright);
            DrawTileGL(geom_lvs, geom_ts, QVector3D(x+1, 0, y), dark, dark, bright, bright);
            DrawTileGL(geom_rvs, geom_ts, QVector3D(x-1, 0, y), dark, dark, bright, bright);
            DrawTileGL(geom_uvs, geom_ts, QVector3D(x, 0, y+1), dark, dark, bright, bright);
            DrawTileGL(geom_dvs, geom_ts, QVector3D(x, 0, y-1), dark, dark, bright, bright);

        }

        return;
    }

    /*
    if (animate) {
        h = float(time.elapsed()) / 200.0f - timeoffset;
        //h = (h > 0.0f) ? h : 0.0f;
        //h = expf( -(h+4) ) * 1000.0f;
        h = sinf(h) * 0.05f;

        //if (h < 0.0001f) {
        //    animate = false;
        //}
    }
    */

    //draws floor tile
    QList <QVector3D> ss = ShadeFloor();

    switch (state) {

    case EMPTY:
    case ENTRANCE:
    case OCCUPIED:

    {

        //glColor3f(1, 1, 1);
        DrawTileGL(geom_fvs, geom_ts, QVector3D(x, 0, y), ss[0], ss[1], ss[2], ss[3]);
        //DrawTileGL(geom_cvs, geom_ts, QVector3D(x, 4, y), ss[0], ss[1], ss[2], ss[3]);

        break;

    }

    default:
        break;

    }   

    return;

    //draws wall tile(s)

    QVector3D bright;
    QVector3D dark;

    if (state == ENTRANCE) {

        bright = QVector3D(0.5f, 1.0f, 0.5f);
        dark = QVector3D(0.4f, 0.9f, 0.4f);

    }
    else {

        bright = QVector3D(1, 1, 1);
        dark = QVector3D(0.75f, 0.75f, 0.75f);

    }

    switch (state) {

    case EMPTY:
    case OCCUPIED:
    case ENTRANCE:

    {

        //draw wall for different neighbour
        if (IsDrawingLeftWall() && IsDrawingUpWall()) {
            DrawTileGL(geom_lvs, geom_ts, QVector3D(x, 0, y), dark, dark, dark, bright);
            //DrawTileGL(geom_lvs, geom_ts, QVector3D(x, 1, y), bright, dark, dark, bright);
            //DrawTileGL(geom_lvs, geom_ts, QVector3D(x, 2, y), bright, dark, dark, bright);
            //DrawTileGL(geom_lvs, geom_ts, QVector3D(x, 3, y), bright, dark, dark, dark);

            DrawTileGL(geom_uvs, geom_ts, QVector3D(x, 0, y), dark, dark, bright, dark);
            //DrawTileGL(geom_uvs, geom_ts, QVector3D(x, 1, y), dark, bright, bright, dark);
            //DrawTileGL(geom_uvs, geom_ts, QVector3D(x, 2, y), dark, bright, bright, dark);
            //DrawTileGL(geom_uvs, geom_ts, QVector3D(x, 3, y), dark, bright, dark, dark);
        }
        else if (IsDrawingRightWall() && IsDrawingUpWall()) {
            DrawTileGL(geom_uvs, geom_ts, QVector3D(x, 0, y), dark, dark, dark, bright);
            //DrawTileGL(geom_uvs, geom_ts, QVector3D(x, 1, y), bright, dark, dark, bright);
            //DrawTileGL(geom_uvs, geom_ts, QVector3D(x, 2, y), bright, dark, dark, bright);
            //DrawTileGL(geom_uvs, geom_ts, QVector3D(x, 3, y), bright, dark, dark, dark);

            DrawTileGL(geom_rvs, geom_ts, QVector3D(x, 0, y), dark, dark, bright, dark);
            //DrawTileGL(geom_rvs, geom_ts, QVector3D(x, 1, y), bright, bright, bright, bright);
            //DrawTileGL(geom_rvs, geom_ts, QVector3D(x, 2, y), bright, bright, bright, bright);
            //DrawTileGL(geom_rvs, geom_ts, QVector3D(x, 3, y), dark, bright, dark, dark);
        }
        else if (IsDrawingRightWall() && IsDrawingDownWall()) {
            DrawTileGL(geom_rvs, geom_ts, QVector3D(x, 0, y), dark, dark, dark, bright);
            //DrawTileGL(geom_rvs, geom_ts, QVector3D(x, 1, y), bright, dark, dark, bright);
            //DrawTileGL(geom_rvs, geom_ts, QVector3D(x, 2, y), bright, dark, dark, bright);
            //DrawTileGL(geom_rvs, geom_ts, QVector3D(x, 3, y), bright, dark, dark, dark);

            DrawTileGL(geom_dvs, geom_ts, QVector3D(x, 0, y), dark, dark, bright, dark);
            //DrawTileGL(geom_dvs, geom_ts, QVector3D(x, 1, y), dark, bright, bright, dark);
            //DrawTileGL(geom_dvs, geom_ts, QVector3D(x, 2, y), dark, bright, bright, dark);
            //DrawTileGL(geom_dvs, geom_ts, QVector3D(x, 3, y), dark, bright, dark, dark);
        }
        else if (IsDrawingLeftWall() && IsDrawingDownWall()) {
            DrawTileGL(geom_dvs, geom_ts, QVector3D(x, 0, y), dark, dark, dark, bright);
            //DrawTileGL(geom_dvs, geom_ts, QVector3D(x, 1, y), bright, dark, dark, bright);
            //DrawTileGL(geom_dvs, geom_ts, QVector3D(x, 2, y), bright, dark, dark, bright);
            //DrawTileGL(geom_dvs, geom_ts, QVector3D(x, 3, y), bright, dark, dark, dark);

            DrawTileGL(geom_lvs, geom_ts, QVector3D(x, 0, y), dark, dark, bright, dark);
            //DrawTileGL(geom_lvs, geom_ts, QVector3D(x, 1, y), dark, bright, bright, dark);
            //DrawTileGL(geom_lvs, geom_ts, QVector3D(x, 2, y), dark, bright, bright, dark);
            //DrawTileGL(geom_lvs, geom_ts, QVector3D(x, 3, y), dark, bright, dark, dark);
        }
        else if (IsDrawingLeftWall()) {
            DrawTileGL(geom_lvs, geom_ts, QVector3D(x, 0, y), dark, dark, bright, bright);
            //DrawTileGL(geom_lvs, geom_ts, QVector3D(x, 1, y), bright, bright, bright, bright);
            //DrawTileGL(geom_lvs, geom_ts, QVector3D(x, 2, y), bright, bright, bright, bright);
            //DrawTileGL(geom_lvs, geom_ts, QVector3D(x, 3, y), bright, bright, dark, dark);
        }
        else if (IsDrawingRightWall()) {
            DrawTileGL(geom_rvs, geom_ts, QVector3D(x, 0, y), dark, dark, bright, bright);
            //DrawTileGL(geom_rvs, geom_ts, QVector3D(x, 1, y), bright, bright, bright, bright);
            //DrawTileGL(geom_rvs, geom_ts, QVector3D(x, 2, y), bright, bright, bright, bright);
            //DrawTileGL(geom_rvs, geom_ts, QVector3D(x, 3, y), bright, bright, dark, dark);
        }
        else if (IsDrawingUpWall()) {
            DrawTileGL(geom_uvs, geom_ts, QVector3D(x, 0, y), dark, dark, bright, bright);
            //DrawTileGL(geom_uvs, geom_ts, QVector3D(x, 1, y), bright, bright, bright, bright);
            //DrawTileGL(geom_uvs, geom_ts, QVector3D(x, 2, y), bright, bright, bright, bright);
            //DrawTileGL(geom_uvs, geom_ts, QVector3D(x, 3, y), bright, bright, dark, dark);
        }
        else if (IsDrawingDownWall()) {
            DrawTileGL(geom_dvs, geom_ts, QVector3D(x, 0, y), dark, dark, bright, bright);
            //DrawTileGL(geom_dvs, geom_ts, QVector3D(x, 1, y), bright, bright, bright, bright);
            //DrawTileGL(geom_dvs, geom_ts, QVector3D(x, 2, y), bright, bright, bright, bright);
            //DrawTileGL(geom_dvs, geom_ts, QVector3D(x, 3, y), bright, bright, dark, dark);
        }

    }

        break;

    default:
        break;

    }


}

void EnvCell::State(const CellState & s)
{
    state = s;    
}

CellState EnvCell::State() const {
    return state;
}

void EnvCell::Dir(CellDir d)
{
    dir = d;
}

CellDir EnvCell::Dir() {
    return dir;
}

QVector3D EnvCell::CellDirToVector(CellDir d)
{
    switch (d) {

    case UP:
        return QVector3D(0, 0, -1);
    case DOWN:
        return QVector3D(0, 0, 1);
    case LEFT:
        return QVector3D(-1, 0, 0);
    case RIGHT:
    default:
        return QVector3D(1, 0, 0);

    }

}

QString EnvCell::CellDirToName(CellDir d)
{
    switch (d) {

    case UP:
        return QString("Up");
    case DOWN:
        return QString("Down");
    case LEFT:
        return QString("Left");
    case RIGHT:
        return QString("Right");
    }

    return QString("");
}

void EnvCell::X(int v) {
    x = v;
}

int EnvCell::X() {
    return x;
}

void EnvCell::Y(int v) {
    y = v;
}

int EnvCell::Y() {
    return y;
}

void EnvCell::Owner(int o) {
    owner = o;
}

int EnvCell::Owner() {
    return owner;
}

void EnvCell::Neighbours(EnvCell * l, EnvCell * r, EnvCell * u, EnvCell * d, EnvCell * ul, EnvCell * ur, EnvCell * dl, EnvCell * dr)
{
    left = l;
    right = r;
    up = u;
    down = d;

    upleft = ul;
    upright = ur;
    downleft = dl;
    downright = dr;
}

bool EnvCell::EightNeighbourOwned()
{
    return (left != NULL && left->Owner() >= 0) ||
            (right != NULL && right->Owner() >= 0) ||
            (up != NULL && up->Owner() >= 0) ||
            (down != NULL && down->Owner() >= 0) ||
            (upleft != NULL && upleft->Owner() >= 0) ||
            (upright != NULL && upright->Owner() >= 0) ||
            (downleft != NULL && downleft->Owner() >= 0) ||
            (downright != NULL && downright->Owner() >= 0);

}

QList <QVector3D> EnvCell::ShadeFloor()
{

    QList <QVector3D> ss;

    QVector3D bright(1, 1, 1);
    QVector3D dark(0.75f, 0.75f, 0.75f);
    //QVector3D dark(0.5f, 0.5f, 0.5f);

    bool l = IsDrawingLeftWall();
    bool r = IsDrawingRightWall();
    bool u = IsDrawingUpWall();
    bool d = IsDrawingDownWall();
    bool ul = (left == NULL || up == NULL) ? false : left->IsDrawingUpWall() && up->IsDrawingLeftWall();
    bool dl = (left == NULL || down == NULL) ? false : left->IsDrawingDownWall() && down->IsDrawingLeftWall();
    bool ur = (right == NULL || up == NULL) ? false : right->IsDrawingUpWall() && up->IsDrawingRightWall();
    bool dr = (right == NULL || down == NULL) ? false : right->IsDrawingDownWall() && down->IsDrawingRightWall();

    if (state == ENTRANCE) {

        for (int i=0; i<4; ++i) {
            ss.push_back(QVector3D(0.5f, 1, 0.5f));
        }

    }
    else {    

        if (l && u) {
            ss.push_back(dark);
            ss.push_back(dark);
            ss.push_back(bright);
            ss.push_back(dark);
        }
        else if (u && r) {
            ss.push_back(dark);
            ss.push_back(dark);
            ss.push_back(dark);
            ss.push_back(bright);
        }
        else if (r && d) {
            ss.push_back(bright);
            ss.push_back(dark);
            ss.push_back(dark);
            ss.push_back(dark);
        }
        else if (d && l) {
            ss.push_back(dark);
            ss.push_back(bright);
            ss.push_back(dark);
            ss.push_back(dark);
        }
        else if (l) {
            ss.push_back(dark);
            ss.push_back(bright);
            ss.push_back(bright);
            ss.push_back(dark);
        }
        else if (u) {
            ss.push_back(dark);
            ss.push_back(dark);
            ss.push_back(bright);
            ss.push_back(bright);
        }
        else if (r) {
            ss.push_back(bright);
            ss.push_back(dark);
            ss.push_back(dark);
            ss.push_back(bright);
        }
        else if (d) {
            ss.push_back(bright);
            ss.push_back(bright);
            ss.push_back(dark);
            ss.push_back(dark);
        }        
        else if (ul) {
            ss.push_back(dark);
            ss.push_back(bright);
            ss.push_back(bright);
            ss.push_back(bright);
        }
        else if (ur) {
            ss.push_back(bright);
            ss.push_back(dark);
            ss.push_back(bright);
            ss.push_back(bright);
        }
        else if (dr) {
            ss.push_back(bright);
            ss.push_back(bright);
            ss.push_back(dark);
            ss.push_back(bright);
        }
        else if (dl) {
            ss.push_back(bright);
            ss.push_back(bright);
            ss.push_back(bright);
            ss.push_back(dark);
        }        
        else {
            ss.push_back(bright);
            ss.push_back(bright);
            ss.push_back(bright);
            ss.push_back(bright);
        }

    }

    return ss;

}

void EnvCell::DrawTileGL(const QVector <QVector3D> & vs, const QVector <QVector2D> & ts, const QVector3D & trans, const QVector3D & s1, const QVector3D & s2, const QVector3D & s3, const QVector3D & s4)
{

    DrawQuad(vs[4] + trans, vs[5] + trans, vs[6] + trans, vs[7] + trans,
            ts[4], ts[5], ts[6], ts[7],
            s1, s2, s3, s4);
    DrawQuad(vs[0] + trans, vs[1] + trans, vs[5] + trans, vs[4] + trans,
            ts[0], ts[1], ts[5], ts[4],
            s1, s2, s2, s1);
    DrawQuad(vs[1] + trans, vs[2] + trans, vs[6] + trans, vs[5] + trans,
            ts[1], ts[2], ts[6], ts[5],
            s2, s3, s3, s2);
    DrawQuad(vs[2] + trans, vs[3] + trans, vs[7] + trans, vs[6] + trans,
            ts[2], ts[3], ts[7], ts[6],
            s3, s4, s4, s3);
    DrawQuad(vs[3] + trans, vs[0] + trans, vs[4] + trans, vs[7] + trans,
            ts[3], ts[0], ts[4], ts[7],
            s4, s1, s1, s4);

}

void EnvCell::DrawQuad(const QVector3D & v1, const QVector3D & v2, const QVector3D & v3, const QVector3D & v4,
                       const QVector2D & t1, const QVector2D & t2, const QVector2D & t3, const QVector2D & t4,
                       const QVector3D & s1, const QVector3D & s2, const QVector3D & s3, const QVector3D & s4)
{

    DrawVertex(v1, t1, s1);
    DrawVertex(v2, t2, s2);
    DrawVertex(v3, t3, s3);
    DrawVertex(v4, t4, s4);

}

void EnvCell::DrawVertex(const QVector3D & v, const QVector2D & t, const QVector3D & s)
{

    glColor3f(s.x(), s.y(), s.z());
    glTexCoord2f(t.x(), t.y());
    glVertex3f(v.x(), v.y(), v.z());

}


