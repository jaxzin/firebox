#ifndef URLENTRYWIDGET_H
#define URLENTRYWIDGET_H

#include <QtOpenGL>

#include "envobject.h"
#include "textgeom.h"
#include "player.h"

class URLEntryWidget : public EnvObject
{

public:

    URLEntryWidget();

    void SetPlayer(Player * p);

    void AddLetter(const QString & c);
    void RemoveLetter();

    void Show();
    void Hide();
    bool IsVisible();

    QString Text() const;

    void DrawGL();

private:

    void UpdateTextGeom();

    QTime time;
    Player * player;

    QTime time_fadestart;
    bool hidden;

    QString curtext;

    TextGeom textgeom_instruct;
    TextGeom textgeom;

};

#endif // URLENTRYWIDGET_H
