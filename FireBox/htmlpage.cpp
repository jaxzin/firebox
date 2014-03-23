#include "htmlpage.h"

QNetworkAccessManager * HTMLPage::manager = NULL;

HTMLPage::HTMLPage() :
    loaded(false)
{
}

void HTMLPage::SetURL(const QUrl & u) {
    url = u;
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

void HTMLPage::AddImage(const QString & img_url_str)
{

    HTMLData newdata;
    newdata.type = IMAGE;

    if (img_url_str.left(7).compare("http://", Qt::CaseInsensitive) == 0)
        newdata.data = img_url_str;
    else if (img_url_str.left(1).compare("/") == 0)
        newdata.data = url.toString(QUrl::RemovePath) + img_url_str;
    else
        newdata.data = url_prefix + QString("/") + img_url_str;

    datas.push_back(newdata);
    ++numimages;

}

void HTMLPage::AddLink(const QString & link_url_str)
{

    HTMLData newdata;
    newdata.type = LINK;

    if (link_url_str.left(7).compare("http://", Qt::CaseInsensitive) == 0) {
        newdata.data = link_url_str;
    }
    else if (link_url_str.left(1).compare("/") == 0) {
        newdata.data = url.toString(QUrl::RemovePath) + link_url_str;
    }
    else {
        newdata.data = url_prefix + QString("/") + link_url_str;
    }

    datas.push_back(newdata);
    ++numlinks;

}

QStringList HTMLPage::GetStringsBetween(const QString & s1, const QString & s2, const QString & str)
{

    QStringList return_str_list;

    QString process_str = str;
    while (process_str.length() > 0) {

        //1.  cut up to the first string s1
        const int i1 = process_str.indexOf(s1);
        if (i1 == -1) {
            break;
        }
        process_str = process_str.right(process_str.length() - i1 - s1.length());

        //2. cut before the second string s2
        const int i2 = process_str.indexOf(s2);
        if (i2 == -1) {
            break;
        }
        return_str_list.push_back(process_str.left(i2));

        process_str = process_str.right(process_str.length() - i2 - s2.length());

    }

    return return_str_list;
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

    url_str = url.toString();
    url_prefix = url_str.left(url_str.lastIndexOf("/"));

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

                    AddImage(sl2[j]);

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

                    if (sl2[j].right(5).contains(".") && !sl2[j].right(4).contains("htm")) {
                        continue;
                    }

                    AddLink(sl2[j]);


                }
                else if (sl2[j].contains("data-href", Qt::CaseInsensitive) && sl2[j].contains("=", Qt::CaseInsensitive)) {

                    ++j;

                    AddLink(sl2[j]);

                }

            }
        }
        else if (sl[i].contains("div ", Qt::CaseInsensitive)) {

            //get this case working:
            //<div title="Google" align="left" id="hplogo" onload="window.lol&amp;&amp;lol()" style="background:url(/images/srpr/logo11w.png) no-repeat;background-size:269px 95px;height:95px;width:269px"><div nowrap="" style="color:#777;font-size:16px;font-weight:bold;position:relative;left:218px;top:70px">Canada</div></div>

            HTMLData newdata;
            newdata.type = DIV;
            newdata.data = sl[i];
            datas.push_back(newdata);

            //QStringList sl2 = sl[i].split("\"");
            QStringList sl2 = GetStringsBetween("\"", "\"", sl[i]);

            for (int j=0; j<sl2.size(); ++j) {

                QStringList sl3 = GetStringsBetween("(", ")", sl2[j]);

                for (int k=0; k<sl3.size(); ++k) {

                    QString extension = sl3[k].right(4);

                    if (extension.compare(".png", Qt::CaseInsensitive) == 0 ||
                            extension.compare(".jpg", Qt::CaseInsensitive) == 0||
                            extension.compare(".gif", Qt::CaseInsensitive) == 0) {

                        AddImage(sl3[k]);

                    }

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
