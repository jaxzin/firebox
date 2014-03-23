#include "envportal.h"

float EnvPortal::portal_spacing = 0.1f;

EnvPortal::EnvPortal() :
    parent_room(-1),
    child_room(-1),
    startedurlrequest(false),
    processed(false),    
    page(NULL)
{    
}

void EnvPortal::SetURL(const QString & s)
{

    const int maxlen = 30;
    QString url_str = QUrl::fromPercentEncoding(s.toLatin1());
    url = QUrl(url_str);

    page = new HTMLPage();
    page->SetURL(url);

    url_str.remove(0, 7); //ditch the "http://"
    if (url_str.length() < maxlen) {
        textgeom_parenturl.Text(url_str);
    }
    else {
        textgeom_parenturl.Text(url_str.left(maxlen/2) + QString("...") + url_str.right(maxlen/2));
    }

    textgeom_parenturl.MaxSize(1.9f, 0.1f);

}

void EnvPortal::SetParentTitle(const QString & s) {

    const int maxlen = 30;

    if (s.length() < maxlen) {
        textgeom_parenttitle.Text(s);
    }
    else {
        textgeom_parenttitle.Text(s.left(maxlen) + QString("..."));
    }

    textgeom_parenttitle.MaxSize(1.9f, 0.1f);

}

void EnvPortal::SetChildTitle(const QString & s)
{
    const int maxlen = 30;

    if (s.length() < maxlen) {
        textgeom_childtitle.Text(s);
    }
    else {
        textgeom_childtitle.Text(s.left(maxlen) + QString("..."));
    }

    textgeom_childtitle.MaxSize(1.9f, 0.1f);
}

void EnvPortal::SetChildURL(const QString & s)
{

    const int maxlen = 30;

    child_url_str = QUrl::fromPercentEncoding(s.toLatin1());
    child_url_str.remove(0, 7); //ditch the "http://"

    if (child_url_str.length() < maxlen) {
        textgeom_childurl.Text(child_url_str);
    }
    else {
        textgeom_childurl.Text(child_url_str.left(maxlen/2) + QString("...") + child_url_str.right(maxlen/2));
    }

    textgeom_childurl.MaxSize(1.9f, 0.1f);

}

void EnvPortal::StartURLRequest()
{

    page->Request();
    startedurlrequest = true;

    anim.Scale(0.6f);
    anim.Animate(true);

}

bool EnvPortal::Loaded() const
{    
    return page->Loaded();
}

bool EnvPortal::StartedURLRequest() const
{
    return startedurlrequest;
}

bool EnvPortal::Processed() const
{
    return processed;
}

void EnvPortal::Processed(bool b)
{
    processed = b;
}

HTMLPage * EnvPortal::Page() const
{
    return page;
}


void EnvPortal::SetParentRoom(const int i)
{
    parent_room = i;
}

int EnvPortal::GetParentRoom() const
{
    return parent_room;
}

void EnvPortal::SetParentPos(const QVector3D & p)
{
    parent_pos = p;
}

QVector3D EnvPortal::GetParentPos() const
{
    return parent_pos;
}

void EnvPortal::SetParentDir(const QVector3D & d)
{
    parent_t = MathUtil::GetRotatedAxis(MathUtil::_PI_OVER_2, d, QVector3D(0, 1, 0));
    parent_n = d;
    parent_b = QVector3D(0, 1, 0);

    anim.Dir(d);
}

QVector3D EnvPortal::GetParentDir() const
{
    return parent_n;
}

void EnvPortal::SetChildRoom(const int i)
{
    child_room = i;
}

int EnvPortal::GetChildRoom() const
{
    return child_room;
}

void EnvPortal::SetChildPos(const QVector3D & p)
{
    child_pos = p;
}

QVector3D EnvPortal::GetChildPos() const
{
    return child_pos;
}

void EnvPortal::SetChildDir(const QVector3D & d)
{
    child_t = MathUtil::GetRotatedAxis(MathUtil::_PI_OVER_2, d, QVector3D(0, 1, 0));
    child_n = d;
    child_b = QVector3D(0, 1, 0);
}

QVector3D EnvPortal::GetChildDir() const
{
    return child_n;
}

void EnvPortal::SetParentColour(const QVector3D & c)
{
    parent_col = c;
}

void EnvPortal::SetChildColour(const QVector3D & c)
{
    child_col = c;
}

void EnvPortal::DrawParentGL()
{

    QVector3D p = parent_pos + QVector3D(0, 1.5, 0) + parent_n * portal_spacing;
    float angle = MathUtil::GetSignedAngleBetweenRadians(QVector3D(0, 0, 1), parent_n) * MathUtil::_180_OVER_PI;

    glPushMatrix();

    glTranslatef(p.x(), p.y(), p.z());
    glRotatef(angle, 0, 1, 0);   

    DrawFrameGL(child_col);

    glTranslatef(0, 0, portal_spacing/2.0f);
    if (!processed && anim.Animate()) {
        anim.DrawGL();
    }

    glTranslatef(0, 1.25f, 0);

    if (processed) {
        glColor4f(0.6f, 0.6f, 1.0f, 1.0f);
    }
    else {
        glColor3f(0.2f, 0.2f, 0.4f);
    }

    textgeom_parenturl.DrawGL();

    if (textgeom_parenttitle.TextLength() > 0) {
        glTranslatef(0, 0.1f, 0);
        glColor4f(1, 1, 1, 1.0f);
        textgeom_parenttitle.DrawGL();
    }

    glPopMatrix();

}

void EnvPortal::DrawChildGL()
{

    QVector3D p = child_pos + QVector3D(0, 1.5, 0) + child_n * portal_spacing;
    float angle = MathUtil::GetSignedAngleBetweenRadians(QVector3D(0, 0, 1), child_n) * MathUtil::_180_OVER_PI;

    glPushMatrix();

    glTranslatef(p.x(), p.y(), p.z());
    glRotatef(angle, 0, 1, 0);

    DrawFrameGL(parent_col);

    glTranslatef(0, 1.25f, portal_spacing/2.0f);

    if (processed) {
        glColor4f(0.6f, 0.6f, 1.0f, 1.0f);
    }
    else {
        glColor3f(0.2f, 0.2f, 0.4f);
    }

    textgeom_childurl.DrawGL();

    if (textgeom_childtitle.TextLength() > 0) {
        glTranslatef(0, 0.1f, 0);
        glColor4f(1, 1, 1, 1.0f);
        textgeom_childtitle.DrawGL();
    }

    glPopMatrix();

}

void EnvPortal::DrawParentStencilGL() const
{
    DrawStencilGL(parent_pos, parent_t, parent_n, parent_b);
}

void EnvPortal::DrawChildStencilGL() const
{
    DrawStencilGL(child_pos, child_t, child_n, child_b);
}

bool EnvPortal::GetPlayerFacingParent(const QVector3D & player_dir) const
{
    return MathUtil::GetAngleBetweenRadians(player_dir, -parent_n) < MathUtil::_PI_OVER_4; //45 degree angle
}

bool EnvPortal::GetPlayerFacingChild(const QVector3D & player_dir) const
{
    return MathUtil::GetAngleBetweenRadians(player_dir, -child_n) < MathUtil::_PI_OVER_4; //45 degree angle
}

bool EnvPortal::GetPlayerNearParent(const QVector3D & player_pos) const
{
    QVector2D local = GetParentLocal(player_pos);
    return (local.x() >= -1.0f && local.x() <= 1.0f && local.y() >= -1.0f && local.y() <= 3.0f);
}

bool EnvPortal::GetPlayerAtParent(const QVector3D & player_pos) const
{
    QVector2D local = GetParentLocal(player_pos);
    return (local.x() >= -1.0f && local.x() <= 1.0f && local.y() >= -1.0f && local.y() <= 1.0f);
}

bool EnvPortal::GetPlayerAtChild(const QVector3D & player_pos) const
{
    QVector2D local = GetChildLocal(player_pos);
    return (local.x() >= -1.0f && local.x() <= 1.0f && local.y() >= -1.0f && local.y() <= 1.0f);
}

QVector2D EnvPortal::GetParentLocal(const QVector3D & p) const
{
    QVector3D local = QVector3D(p.x() - parent_pos.x(), 0.0, p.z() - parent_pos.z());
    return QVector2D(QVector3D::dotProduct(local, parent_t), QVector3D::dotProduct(local, parent_n));
}

QVector2D EnvPortal::GetChildLocal(const QVector3D & p) const
{
    QVector3D local = QVector3D(p.x() - child_pos.x(), 0.0, p.z() - child_pos.z());
    return QVector2D(QVector3D::dotProduct(local, child_t), QVector3D::dotProduct(local, child_n));
}

bool EnvPortal::GetPlayerCrossedParent(const QVector3D & player_pos, const QVector3D & player_vel) const
{
    QVector2D local1 = GetParentLocal(player_pos);
    QVector2D local2 = GetParentLocal(player_pos + player_vel);
    return ((local1.y() - portal_spacing) * (local2.y() - portal_spacing) < 0.0f) && GetPlayerAtParent(player_pos + player_vel);
}

bool EnvPortal::GetPlayerCrossedChild(const QVector3D & player_pos, const QVector3D & player_vel) const
{
    QVector2D local1 = GetChildLocal(player_pos);
    QVector2D local2 = GetChildLocal(player_pos + player_vel);
    return ((local1.y() - portal_spacing) * (local2.y() - portal_spacing) < 0.0f) && GetPlayerAtChild(player_pos + player_vel);
}

void EnvPortal::DrawFrameGL(const QVector3D & col) const
{

    //draw the portal frame
    float frame_thick;
    float frame_out;

    glDepthMask(GL_FALSE);
    glDisable(GL_CULL_FACE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_QUADS);

    for (int i=0; i<3; ++i) {

        //glow effect on 3 different planes
        if (i == 0) {
            frame_thick = 0.1f;
            frame_out = 0.05f;
        }
        else if (i == 1) {
            frame_thick = 0.075f;
            frame_out = 0.5f;
        }
        else {
            frame_thick = 0.05f;
            frame_out = 1.0f;
        }

        //if page behind the portal not loaded/processed, reduce the glowing effect
        if (!processed) {
            frame_thick *= 0.1f;
            frame_out *= 0.1f;
        }

        glColor4f(col.x(), col.y(), col.z(), 0.5f);
        glVertex3f(-0.9f, -1.5f, 0);
        glVertex3f(-0.9f, 1.5f, 0);
        glColor4f(col.x(), col.y(), col.z(), 0.0f);
        glVertex3f(-0.9f - frame_thick, 1.5f + frame_thick, frame_out);
        glVertex3f(-0.9f - frame_thick, -1.5f, frame_out);

        glColor4f(col.x(), col.y(), col.z(), 0.0f);
        glVertex3f(0.9f + frame_thick, -1.5f, frame_out);
        glVertex3f(0.9f + frame_thick, 1.5f + frame_thick, frame_out);
        glColor4f(col.x(), col.y(), col.z(), 0.5f);
        glVertex3f(0.9f, 1.5f, 0);
        glVertex3f(0.9f, -1.5f, 0);

        glColor4f(col.x(), col.y(), col.z(), 0.5f);
        glVertex3f(-0.9f , 1.5f, 0);
        glVertex3f(0.9f, 1.5f, 0);
        glColor4f(col.x(), col.y(), col.z(), 0.0f);
        glVertex3f(0.9f + frame_thick, 1.5f + frame_thick, frame_out);
        glVertex3f(-0.9f - frame_thick, 1.5f + frame_thick, frame_out);

    }

    glEnd();

    glDepthMask(GL_TRUE);
    glEnable(GL_CULL_FACE);
    glDisable(GL_BLEND);

}


void EnvPortal::DrawStencilGL(const QVector3D & p, const QVector3D & t, const QVector3D & n, const QVector3D & b) const
{

    const QVector3D p1 = p + n * portal_spacing - t * 0.9;
    const QVector3D p2 = p + n * portal_spacing + t * 0.9;
    const QVector3D p3 = p + n * portal_spacing + t * 0.9 + b * 3.0f;
    const QVector3D p4 = p + n * portal_spacing - t * 0.9 + b * 3.0f;

    //glColor3f(1, 1, 1);
    glBegin(GL_QUADS);
    glVertex3f(p1.x(), p1.y(), p1.z());
    glVertex3f(p2.x(), p2.y(), p2.z());
    glVertex3f(p3.x(), p3.y(), p3.z());
    glVertex3f(p4.x(), p4.y(), p4.z());
    glEnd();

}

float EnvPortal::GetPortalSpacing()
{
    return portal_spacing;
}
