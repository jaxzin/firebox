#include "envimg.h"

EnvImg::EnvImg(const QImage & image) :
    loaded(true),
    tex(0)
{

    img = image;

}

EnvImg::EnvImg(const QUrl & url) :
    loaded(false),
    tex(0)
{

    QNetworkRequest request;
    request.setUrl(url);
    request.setRawHeader("User-Agent", "FireBox 1.0");

    reply = HTMLPage::NetManager()->get(request);

    connect(reply, SIGNAL(finished()), this, SLOT(slotFinished()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(slotError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(slotSslErrors(QList<QSslError>)));

    anim = new SpinAnimation();
    anim->Animate(true);

    img_url_str = url.toString();

}

EnvImg::~EnvImg()
{

    delete anim;

    DeleteTexture();

}

void EnvImg::DrawGL()
{

    if (loaded && !tex) {
        tex = LoadTexture(img, true);
    }

    if (!loaded) {
        anim->Pos(pos);
        anim->Dir(dir);
        anim->Scale(0.25f);

        anim->DrawGL();
    }
    else {      

        glPushMatrix();

        MathUtil::FacePosDirGL(pos, dir);

        //glTranslatef(0, 0, -0.35f);
        glTranslatef(0, 2, 0.15f);

        float iw;
        float ih;

        if (img.width() > img.height()) {
            iw = 1.0f;
            ih = float(img.height()) / float(img.width());
        }
        else {
            iw = float(img.width()) / float(img.height());
            ih = 1.0f;
        }

        iw *= 0.9;
        ih *= 0.9f;

        glColor3f(1, 1, 1);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, tex);

        glBegin(GL_QUADS);

        glTexCoord2i(0, 0);
        glVertex2f(-iw, -ih);

        glTexCoord2i(1, 0);
        glVertex2f(iw, -ih);

        glTexCoord2i(1, 1);
        glVertex2f(iw, ih);

        glTexCoord2i(0, 1);
        glVertex2f(-iw, ih);

        glEnd();

        glDisable(GL_TEXTURE_2D);

        glPopMatrix();

    }

}

void EnvImg::slotFinished() {

    loaded = true;   

    QByteArray ba = reply->readAll();
    reply->close();

    bool succeed = img.loadFromData(ba);
    if (!succeed) {
        //qDebug() << "Unable to load image from data " << ba;
        qDebug() << "Unable to load image" << img_url_str;
    }

}

void EnvImg::slotError(QNetworkReply::NetworkError)
{

}

void EnvImg::slotSslErrors(QList <QSslError>)
{

}

void EnvImg::DeleteTexture()
{

    if (tex > 0) {
        glDeleteTextures(1, &tex);
    }
    tex = 0;

}

