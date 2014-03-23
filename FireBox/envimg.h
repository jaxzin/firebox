#ifndef ENVIMG_H
#define ENVIMG_H

#include <QtNetwork>

#include "envobject.h"
#include "htmlpage.h"
#include "spinanimation.h"

class EnvImg : public QObject, public EnvObject
{

    Q_OBJECT

public:

    EnvImg(const QImage & image); //load image locally
    EnvImg(const QUrl & url); //load image through a URL
    ~EnvImg();

    void DrawGL();   

public slots:

    void slotFinished();
    void slotError(QNetworkReply::NetworkError e);
    void slotSslErrors(QList <QSslError> e);

private:

    void DeleteTexture();

    QNetworkReply * reply;
    bool loaded;

    SpinAnimation * anim;

    QImage img;
    GLuint tex;

};

#endif // ENVIMG_H
