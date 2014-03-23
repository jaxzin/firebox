#include "objfile.h"

ObjFile::ObjFile(const QString path)
{        

    maxx = -FLT_MAX;
    maxy = -FLT_MAX;

    QFile infile( path );

    if (!infile.open( QIODevice::ReadOnly | QIODevice::Text )) {
        QMessageBox::critical(NULL, QString("Can't open '") + path + QString("' for read."), QString("Sorry."));
        return;
    }

    //set base path (for loading resources in same dir)
    QFileInfo fi( path );
    m_basePath = fi.absolutePath();

    QTextStream ifs( &infile );

    int curMaterial = -1;

    while (!ifs.atEnd()) {

        QStringList line = ifs.readLine().split(" ");

        if (line.first().compare("mtllib") == 0) {

            QString mtlfilepath = m_basePath + "/" + line.last();
            LoadMaterials(mtlfilepath);

        }
        else if (line.first().compare("usemtl") == 0) {

            for (int i=0; i<m_mats.size(); ++i) {

                if (line.last().compare(m_mats[i].name) == 0) {
                    curMaterial = i;
                    break;
                }

            }

        }
        else if (line.first().compare("v") == 0) {

            const float x = line[1].toFloat();
            const float y = line[2].toFloat();
            const float z = line[3].toFloat();

            m_verts.push_back(QVector3D(x, y, z));

            maxx = (x > maxx) ? x : maxx;
            maxy = (y > maxy) ? y : maxy;

        }
        else if (line.first().compare("vn") == 0) {

            m_norms.push_back(QVector3D(line[1].toFloat(), line[2].toFloat(), line[3].toFloat()));

        }
        else if (line.first().compare("vt") == 0) {

            m_uvs.push_back(QVector2D(line[1].toFloat(), line[2].toFloat()));

        }
        else if (line.first().compare("f") == 0) {

            Face f;

            for (int i=1; i<line.size(); ++i) {

                QStringList eachv = line[i].split("/");

                for (int j=0; j<eachv.size(); ++j) {

                    if (!eachv[j].isEmpty()) {
                        const int ind = eachv[j].toInt() - 1;
                        if (j == 0)
                            f.vi.push_back(ind);
                        else if (j==1)
                            f.uvi.push_back(ind);
                        else
                            f.ni.push_back(ind);
                    }

                }

            }

            f.mati = curMaterial;

            m_faces.push_back(f);

        }

    }

    infile.close();

}

void ObjFile::SetUseMaterials(bool b)
{
    usemat = b;
}

void ObjFile::LoadMaterials(QString mtlpath)
{

    QFile mtlfile(mtlpath);
    if (!mtlfile.open( QIODevice::ReadOnly | QIODevice::Text )) {
        QMessageBox::critical(NULL, QString("Can't open '") + mtlpath + QString("' for read."), QString("Sorry."));
        return;
    }

    QTextStream mtlfs(&mtlfile);

    while (!mtlfs.atEnd()) {

        QStringList mtlline = mtlfs.readLine().split(" ");

        if (mtlline.first().compare("newmtl") == 0) {

            Material m;
            m_mats.push_back(m);

            m_mats.last().name = mtlline.last();

            qDebug() << "adding material with name " << m_mats.last().name;

        }
        else if (mtlline.first().compare("kd", Qt::CaseInsensitive) == 0) {

            m_mats.last().kd = new float[3];
            m_mats.last().kd[0] = mtlline[1].toFloat();
            m_mats.last().kd[1] = mtlline[2].toFloat();
            m_mats.last().kd[2] = mtlline[3].toFloat();

        }
        else if (mtlline.first().compare("ka", Qt::CaseInsensitive) == 0) {

            m_mats.last().ka = new float[3];
            m_mats.last().ka[0] = mtlline[1].toFloat();
            m_mats.last().ka[1] = mtlline[2].toFloat();
            m_mats.last().ka[2] = mtlline[3].toFloat();

        }
        else if (mtlline.first().compare("map_kd", Qt::CaseInsensitive) == 0) {

            m_mats.last().map_kd = mtlline.last();
            m_mats.last().texindex = LoadTexture(m_basePath + "/" + m_mats.last().map_kd);
            qDebug() << "texindex " << m_mats.last().texindex << " assigned to texture " << m_mats.last().map_kd;

        }

    }

    mtlfile.close();

}

void ObjFile::Draw()
{

    //glEnable(GL_LIGHTING);
    //glEnable(GL_LIGHT0);

    int curMaterial = -1;

    glBegin(GL_TRIANGLES);

    for (int i=0; i<m_faces.size(); ++i) {

        if (usemat && curMaterial != m_faces[i].mati) {

            glEnd();

            curMaterial = m_faces[i].mati;

            glMaterialfv(GL_FRONT, GL_AMBIENT, m_mats[curMaterial].ka);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, m_mats[curMaterial].kd);

            if (m_mats[curMaterial].map_kd.isEmpty()) {

                glDisable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, 0);

                //const Vector3 & kd = m_mats[curMaterial].kd;
                glColor3f(1, 1, 1);

            }
            else {

                glColor3f(1, 1, 1);
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, m_mats[curMaterial].texindex);

            }

            glBegin(GL_TRIANGLES);

        }

        //tri fan out this poly (works well in case more than 3 verts per face)
        for (int j=1; j+1 < m_faces[i].vi.size(); ++j) {

            if (!m_faces[i].ni.isEmpty()) {
                const QVector3D & n1 = m_norms[m_faces[i].ni[0]];
                glNormal3f(n1.x(), n1.y(), n1.z());
            }

            if (!m_faces[i].uvi.isEmpty()) {
                const QVector2D & uv1 = m_uvs[m_faces[i].uvi[0]];
                glTexCoord2f(uv1.x(), uv1.y());
            }

            const QVector3D & v1 = m_verts[m_faces[i].vi[0]];
            glVertex3f(v1.x(), v1.y(), v1.z());

            if (!m_faces[i].ni.isEmpty()) {
                const QVector3D & n2 = m_norms[m_faces[i].ni[j]];
                glNormal3f(n2.x(), n2.y(), n2.z());
            }

            if (!m_faces[i].uvi.isEmpty()) {
                const QVector2D & uv2 = m_uvs[m_faces[i].uvi[j]];
                glTexCoord2f(uv2.x(), uv2.y());
            }

            const QVector3D & v2 = m_verts[m_faces[i].vi[j]];
            glVertex3f(v2.x(), v2.y(), v2.z());

            if (!m_faces[i].ni.isEmpty()) {
                const QVector3D & n3 = m_norms[m_faces[i].ni[j+1]];
                glNormal3f(n3.x(), n3.y(), n3.z());
            }

            if (!m_faces[i].uvi.isEmpty()) {
                const QVector2D & uv3 = m_uvs[m_faces[i].uvi[j+1]];
                glTexCoord2f(uv3.x(), uv3.y());
            }

            const QVector3D & v3 = m_verts[m_faces[i].vi[j+1]];
            glVertex3f(v3.x(), v3.y(), v3.z());

        }

    }

    glEnd();

    glDisable(GL_TEXTURE_2D);
    if (usemat)
        glColor3f(1, 1, 1);

    //glDisable(GL_LIGHTING);
    //glDisable(GL_LIGHT0);

}

GLuint ObjFile::LoadTexture(QString file,  int * width, int * height)
{

    QImage tex1, buf;
    if (!buf.load(file))
        qDebug() << "Couldn't load file " << file;
    tex1 = QGLWidget::convertToGLFormat( buf );

    GLuint texInd;

    glGenTextures(1, &texInd);
    glBindTexture(GL_TEXTURE_2D, texInd);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, 4, tex1.width(), tex1.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, tex1.bits());

    glBindTexture(GL_TEXTURE_2D, 0);

    if (width) {
        *width = tex1.width();
    }

    if (height) {
        *height = tex1.height();
    }

    return texInd;

}

float ObjFile::MaxX()
{
    return maxx;
}

float ObjFile::MaxY()
{
    return maxy;
}
