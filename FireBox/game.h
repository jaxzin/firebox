#ifndef GAME_H
#define GAME_H

#include <QtGui>

#include "environment.h"
#include "player.h"
#include "urlentrywidget.h"

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
    void DrawGL(const float half_ipd, const double rot[16]);

    void initializeGL();

    void DrawKappas(const QList <float> & kappas);

private:    

    const float player_height;
    const float player_fly_height;
    Player * player;
    bool player_high;

    Environment * env;    
    URLEntryWidget * urlentrywidget;

};

#endif // GAME_H
