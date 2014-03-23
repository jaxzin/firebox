#ifndef HTMLPAGE_H
#define HTMLPAGE_H

#include <QtNetwork>
#include <QtGui>
#include <QtXml>

enum HTMLDataType {
    TEXT,
    IMAGE,
    LINK
};

struct HTMLData {

    HTMLDataType type;
    QString data;


};

class HTMLPage : public QObject, public QXmlDefaultHandler
{
    Q_OBJECT

public:

    HTMLPage(QUrl u);

    void Request();
    bool Loaded();

    static QNetworkAccessManager * NetManager();

    QString & Title();
    const QList <HTMLData> & Data();
    int NumImages();
    int NumLinks();
    int NumTexts();

public slots:

    void slotFinished();
    void slotError(QNetworkReply::NetworkError e);
    void slotSslErrors(QList <QSslError> e);

private:

    QString HTMLExtract(const QString & tag, const QString & html);

    static QNetworkAccessManager * manager;
    QNetworkReply * reply;

    QXmlSimpleReader reader;

    bool loaded;

    QUrl url;
    QString title;

    QList <HTMLData> datas;
    int numimages;
    int numlinks;
    int numtexts;

};

#endif // HTMLPAGE_H
