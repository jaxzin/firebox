#ifndef ENVIRONMENT2_H
#define ENVIRONMENT2_H

#include "mathutil.h"

#include "envportal.h"
#include "envportalroom.h"
#include "envskybox.h"
#include "envroomtemplate.h"

#include "player.h"
#include "soundmanager.h"

class Environment2
{

public:    

    Environment2();
    ~Environment2();

    void initializeGL();
    void DrawGL(Player & player);

    void Reset();

    void AddImage(const int room_index, const QImage & img);
    void AddPortal(const int room_index, const QString & url);

    void Update(Player & player);
    QVector3D ClipPlayerTrajectory(const QVector3D & pos, const QVector3D & vel) const;

    int PlayerCurRoom() const;

    bool PlayerCanGoBack() const;
    QVector3D PlayerBackPos() const;
    QVector3D PlayerBackDir() const;

private:

    bool Update_CrossPortals(Player & player);

    void MovePlayer(const int portal_index, const bool go_parent_to_child, Player & player);
    void DrawRoomWithinPortalStencilGL(const int portal_index, const bool within_parent_room);

    void LoadRoomTemplates();
    QVector3D huecycle(double val);

    EnvSkybox skybox;

    QList <EnvPortalRoom> rooms;
    QList <EnvPortal> portals;
    QList <EnvRoomTemplate> room_templates;

    QSet <QString> alllinks;

    int player_curroom;
    int nTemplates;

};

#endif // ENVIRONMENT2_H
