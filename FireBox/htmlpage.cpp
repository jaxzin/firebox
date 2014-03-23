#include "htmlpage.h"

QNetworkAccessManager * HTMLPage::manager = NULL;

HTMLPage::HTMLPage(QUrl u) :
    loaded(false),
    url(u)
{
}

void HTMLPage::Request()
{

    if (!manager)
        manager = new QNetworkAccessManager();

    QNetworkRequest request;
    request.setUrl(url);
    request.setRawHeader("User-Agent", "FireBox 1.0");

    reply = manager->get(request);

    connect(reply, SIGNAL(finished()), this, SLOT(slotFinished()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(slotError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(slotSslErrors(QList<QSslError>)));

    qDebug() << "Requesting" << url.toString();
}

bool HTMLPage::Loaded()
{
    return loaded;
}

QString HTMLPage::HTMLExtract(const QString & tag, const QString & html)
{

    QString returnStr;

    //extract a title
    int ind = html.indexOf(tag);

    if (ind >= 0) {
        returnStr = html.right(html.length() - (ind + tag.length()));
        returnStr = returnStr.left(returnStr.indexOf("<"));
    }

    return returnStr;
}

void HTMLPage::slotFinished()
{

    QByteArray ba = reply->readAll();
    reply->close();

    loaded = true;

    QString htmlstr(ba);
    QString titleTag("<title>");

    title = HTMLExtract(titleTag, htmlstr);

    QString s(ba);
    QStringList sl = s.split("<");

    //qDebug() << sl;

    datas.clear();

    QString urlstr = url.toString();
    QString urlprefix = urlstr.left(urlstr.lastIndexOf("/"));

    numimages = 0;
    numlinks = 0;
    numtexts = 0;

    for (int i=0; i<sl.size(); ++i) {

        if (sl[i].contains("img ", Qt::CaseInsensitive) &&
                sl[i].contains("src", Qt::CaseInsensitive)) {

            QStringList sl2 = sl[i].split("\"");

            for (int j=0; j<sl2.size()-1; ++j) {

                if (sl2[j].contains("src", Qt::CaseInsensitive) && sl2[j].contains("=", Qt::CaseInsensitive)) {

                    ++j;

                    HTMLData newdata;
                    newdata.type = IMAGE;

                    if (sl2[j].left(7).compare("http://", Qt::CaseInsensitive) == 0)
                        newdata.data = sl2[j];
                    else if (sl2[j].left(1).compare("/") == 0)
                        newdata.data = url.toString(QUrl::RemovePath) + sl2[j];
                    else
                        newdata.data = urlprefix + QString("/") + sl2[j];

                    datas.push_back(newdata);
                    ++numimages;

                }

            }

        }
        else if ((sl[i].contains("a ", Qt::CaseInsensitive) &&
                    sl[i].contains("href", Qt::CaseInsensitive)) ||
                    (sl[i].contains("meta ", Qt::CaseInsensitive) &&
                    sl[i].contains("url", Qt::CaseInsensitive))) {

            QStringList sl2 = sl[i].split("\"");

            for (int j=0; j<sl2.size()-1; ++j) {

                if (sl2[j].contains("href", Qt::CaseInsensitive) && sl2[j].contains("=", Qt::CaseInsensitive)) {

                    ++j;

                    if (sl2[j].right(5).contains(".") && !sl2[j].right(4).contains("htm"))
                        continue;

                    HTMLData newdata;
                    newdata.type = LINK;

                    if (sl2[j].left(7).compare("http://", Qt::CaseInsensitive) == 0)
                        newdata.data = sl2[j];
                    else if (sl2[j].left(1).compare("/") == 0)
                        newdata.data = url.toString(QUrl::RemovePath) + sl2[j];
                    else
                        newdata.data = urlprefix + QString("/") + sl2[j];

                    datas.push_back(newdata);
                    ++numlinks;

                }

            }
        }

        //extract any other text
        QString textpiece = sl[i].mid(sl[i].indexOf(">") + 1);
        textpiece = textpiece.trimmed();
        if (textpiece.length() > 0) {

            HTMLData newdata;
            newdata.type = TEXT;
            newdata.data = textpiece;
            datas.push_back(newdata);
            ++numtexts;

        }


    }

    qDebug() << title << numimages << "images," << numlinks << "links," << numtexts << " texts.";

}

void HTMLPage::slotError(QNetworkReply::NetworkError)
{

}

void HTMLPage::slotSslErrors(QList <QSslError>)
{

}

QString & HTMLPage::Title()
{
    return title;
}

const QList <HTMLData> & HTMLPage::Data()
{
    return datas;
}

int HTMLPage::NumImages()
{
    return numimages;
}

int HTMLPage::NumLinks()
{
    return numlinks;
}

int HTMLPage::NumTexts()
{
    return numtexts;
}

QNetworkAccessManager * HTMLPage::NetManager()
{
    return manager;
}
