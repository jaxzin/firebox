#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "mathutil.h"
#include "envroom.h"
#include "envcell.h"
#include "enventrance.h"
#include "envtitle.h"
#include "envskybox.h"
#include "player.h"

class Environment
{
public:

    Environment(Player * p);
    ~Environment();

    void LoadTextures();

    int AddNewSpace(EnvEntrance * ent, int minarea, int minperim, int x1, int y1, int x2, int y2);
    void AddNewEntrance(const int roomind, const QString & url);
    void AddNewEnvImage(const int roomind, const QImage & image);
    void AddNewEnvImage(const int roomind, const QString & url);
    void AddNewEnvText(const int roomind, const QString & text);

    bool DoGrowRoom(int minarea, int minperim, int & x1, int & y1, int & x2, int & y2);

    int EvaluateRoom(const int minarea, const int minperim, const int x1, const int y1, const int x2, const int y2);
    int EvaluatePerimeter(int x1, int y1, int x2, int y2);
    int EvaluateArea(int x1, int y1, int x2, int y2);

    bool FindFreePerimeterPart(const int roomind, const int len, const int space, int & x1, int & y1, int & x2, int & y2, CellDir & d );

    EnvCell & Cell(int x, int y);

    QVector3D PlayerBackPos() const;
    QVector3D PlayerBackDir() const;
    int PlayerCurRoom() const;

    void ClipPlayerVelocity();

    void Update();
    void DrawGL();

private:   

    void DeleteDisplayList();

    bool CheckNineCollision(const int i, const int j);
    bool CheckNeighboursEmpty(const int i, const int j, const int roomind);

    const int maxroomperimetersize;
    int envsize;
    int entrancesize;

    EnvCell nullcell;

    QVector <QVector <EnvCell> > cells;
    QList <EnvRoom *> rooms;
    QList <EnvEntrance *> entrances;
    QList <EnvTitle *> roomtitles;
    EnvSkybox skybox;

    QSet <QString> alllinks;

    QVector3D player_backpos;
    QVector3D player_backdir;
    int player_curroom;

    GLuint tex_tile;

    int disp_list;
    bool rebuild_list;

    Player * player;

};

#endif // ENVIRONMENT_H
