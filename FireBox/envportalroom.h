#ifndef ENVPORTALROOM_H
#define ENVPORTALROOM_H

#include <QtCore>

#include "envroomtemplate.h"
#include "envimg.h"
#include "envtext.h"
#include "envportal.h"

class EnvPortalRoom
{

public:

    EnvPortalRoom();

    void SetRoomTemplate(EnvRoomTemplate * t);

    void SetColour(const QVector3D & c);
    QVector3D GetColour() const;

    void SetTitle(const QString & s);
    QString GetTitle() const;

    void SetURL(const QString & s);
    QString GetURL() const;

    void AddEnvImage(const QImage & image);
    void AddEnvImage(const QString & url);

    void AddEnvText(const QString & text);

    int NumMountPointsFree();

    void SetParentPortal(const int portal_index, QVector3D & pos, QVector3D & dir);
    void AddChildPortal(const int portal_index, QVector3D & pos, QVector3D & dir);

    void DrawGL();

    int GetParentPortal() const;
    int GetNumChildPortals() const;
    QList <int> & GetChildPortals();

    QVector3D ClipPlayerTrajectory(const QList <EnvPortal> & portals, const QVector3D & pos, const QVector3D & vel) const;

private:

    int cur_mount;

    QVector3D colour;
    QString title;
    QString url_str;

    EnvRoomTemplate * room_template;
    QList <EnvImg *> envimages;
    QList <EnvText *> envtexts;

    int parent_portal;
    QList <int> child_portals;

};

#endif // ENVPORTALROOM_H
