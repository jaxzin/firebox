#ifndef ENVPORTAL_H
#define ENVPORTAL_H

#include <QString>
#include <QVector3D>

#include "htmlpage.h"
#include "spinanimation.h"
#include "mathutil.h"

class EnvPortal
{

public:

    EnvPortal();

    void SetURL(const QString & s);
    void SetParentTitle(const QString & s);
    void SetChildTitle(const QString & s);
    void SetChildURL(const QString & s);
    void StartURLRequest();

    bool Loaded() const;
    bool StartedURLRequest() const;
    bool Processed() const;
    void Processed(bool b);
    HTMLPage * Page() const;

    void SetParentRoom(const int i);
    int GetParentRoom() const;
    void SetParentPos(const QVector3D & p);
    QVector3D GetParentPos() const;
    void SetParentDir(const QVector3D & d);
    QVector3D GetParentDir() const;

    void SetChildRoom(const int i);
    int GetChildRoom() const;
    void SetChildPos(const QVector3D & p);
    QVector3D GetChildPos() const;
    void SetChildDir(const QVector3D & d);
    QVector3D GetChildDir() const;

    void SetParentColour(const QVector3D & c);
    void SetChildColour(const QVector3D & c);

    void DrawParentGL();
    void DrawChildGL();

    void DrawParentStencilGL() const;
    void DrawChildStencilGL() const;

    bool GetPlayerFacingParent(const QVector3D & player_pos) const;
    bool GetPlayerFacingChild(const QVector3D & player_pos) const;

    bool GetPlayerNearParent(const QVector3D & player_pos) const;        

    bool GetPlayerAtParent(const QVector3D & player_pos) const;
    bool GetPlayerAtChild(const QVector3D & player_pos) const;

    bool GetPlayerCrossedParent(const QVector3D & player_pos, const QVector3D & player_vel) const;
    bool GetPlayerCrossedChild(const QVector3D & player_pos, const QVector3D & player_vel) const;

    static float GetPortalSpacing();

private:

    void DrawFrameGL(const QVector3D & col) const;

    QVector2D GetParentLocal(const QVector3D & p) const;
    QVector2D GetChildLocal(const QVector3D & p) const;

    void DrawStencilGL(const QVector3D & p, const QVector3D & t, const QVector3D & n, const QVector3D & b) const;

    static float portal_spacing;
    int parent_room;
    int child_room;

    QUrl url;
    QString url_str;
    QString child_url_str;

    bool startedurlrequest;
    bool processed;

    SpinAnimation anim;
    TextGeom textgeom_parenturl;
    TextGeom textgeom_parenttitle;
    TextGeom textgeom_childurl;
    TextGeom textgeom_childtitle;
    HTMLPage * page;

    QVector3D parent_pos;
    QVector3D parent_t;
    QVector3D parent_n;
    QVector3D parent_b;
    QVector3D parent_col;

    QVector3D child_pos;
    QVector3D child_t;
    QVector3D child_n;
    QVector3D child_b;
    QVector3D child_col;    

};

#endif // ENVPORTAL_H
