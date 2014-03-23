#ifndef GAME_H
#define GAME_H

#include <QtGui>

//#include "environment.h"
#include "environment2.h"
#include "player.h"
#include "urlentrywidget.h"
#include "soundmanager.h"

class Game
{

public:

    Game();

    void mouseMoveEvent(float x, float y);
    void mousePressEvent(QMouseEvent * e);
    void mouseReleaseEvent(QMouseEvent * e);
    void keyPressEvent(QKeyEvent * e);
    bool keyReleaseEvent(QKeyEvent * e);

    void Update();
    void DrawGL(const float half_ipd, const QVector3D & up, const QVector3D & forward);

    void initializeGL();

    void DrawKappas(const QList <float> & kappas);

    void Reset();

private:    

    void LoadBookmarks();

    Player player;
    const float player_height;
    const float player_fly_height;    

    //Environment env;
    Environment2 env2;
    URLEntryWidget urlentrywidget;

    QList <QString> bookmarks_list;

};

#endif // GAME_H
