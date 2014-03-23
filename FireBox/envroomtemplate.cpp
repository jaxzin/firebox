#include "envroomtemplate.h"

EnvRoomTemplate::EnvRoomTemplate()
{
    disp_list = 0;
}

void EnvRoomTemplate::Load(const QString & obj_filename, const QString & tex_filename, const QString & data_filename)
{

    LoadObj(obj_filename);
    LoadTexture(tex_filename);
    LoadData(data_filename);

}


void EnvRoomTemplate::DrawGL()
{

    const float uvoffset = 0.0f;

    if (disp_list > 0) {

        glCallList(disp_list);

    }
    else {

        int num_quads = 0;

        disp_list = glGenLists(1);
        glNewList(disp_list, GL_COMPILE_AND_EXECUTE);

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture);

        glBegin(GL_TRIANGLES);
        for (int i=0; i<obj_face_verts.size(); i+=3) {

            for (int j=0; j<3; ++j) {

                const int ind_vt = obj_face_texcoords[i+j];
                const int ind_v = obj_face_verts[i+j];

                glTexCoord2f(obj_texcoords[ind_vt].x() + uvoffset, obj_texcoords[ind_vt].y() + uvoffset);
                glVertex3f(obj_verts[ind_v].x(), obj_verts[ind_v].y(), obj_verts[ind_v].z());

            }

        }
        glEnd();

        glDisable(GL_TEXTURE_2D);

        glEndList();

    }

}

void EnvRoomTemplate::LoadObj(const QString & filename)
{
    QFile file( filename );
    if (!file.open( QIODevice::ReadOnly | QIODevice::Text )) {
        return;
    }

    //set base path (for loading resources in same dir)
    QTextStream ifs(&file);

    while (!ifs.atEnd()) {

        const QStringList line = ifs.readLine().split(" ", QString::SkipEmptyParts);

        //skip any blank lines or lines without at least 4 entries (a command + 3 parameters)
        if (line.size() < 3) {
            continue;
        }

        if (QString::compare(line.first(), "v") == 0) {
            obj_verts.push_back(QVector3D(line[1].toFloat(), line[2].toFloat(), line[3].toFloat()));
        }
        else if (QString::compare(line.first(), "vt") == 0) {
            obj_texcoords.push_back(QVector2D(line[1].toFloat(), line[2].toFloat()));
        }
        else if (QString::compare(line.first(), "f") == 0) {

            //for a face line, triangle face entries come from 1-2-3, 1-3-4, 1-4-5, ...
            const QStringList each_v1 = line[1].split("/");

            for (int j=3; j<line.size(); ++j) {

                const QStringList each_v2 = line[j-1].split("/");
                const QStringList each_v3 = line[j].split("/");

                const int ind_v1 = each_v1.first().toInt()-1;
                const int ind_v2 = each_v2.first().toInt()-1;
                const int ind_v3 = each_v3.first().toInt()-1;

                const int ind_vt1 = each_v1.last().toInt()-1;
                const int ind_vt2 = each_v2.last().toInt()-1;
                const int ind_vt3 = each_v3.last().toInt()-1;

                obj_face_verts.push_back(ind_v1); //note: 1-indexed in file, but 0-indexed in program
                obj_face_verts.push_back(ind_v2);
                obj_face_verts.push_back(ind_v3);

                obj_face_texcoords.push_back(ind_vt1);
                obj_face_texcoords.push_back(ind_vt2);
                obj_face_texcoords.push_back(ind_vt3);

            }
        }

    }

    file.close();
}

void EnvRoomTemplate::LoadTexture(const QString & filename)
{

    QImage img(filename);
    texture = EnvObject::LoadTexture(img, false);

}

void EnvRoomTemplate::LoadData(const QString & filename)
{

    QFile file( filename );
    if (!file.open( QIODevice::ReadOnly | QIODevice::Text )) {
        return;
    }

    //set base path (for loading resources in same dir)
    QTextStream ifs(&file);

    //get num colliders
    QStringList eachline = ifs.readLine().split(" ");
    const int nColliders = eachline.last().toInt();

    //read rectangular colliders
    for (int i=0; i<nColliders; ++i) {

        eachline = ifs.readLine().split(" ");

        if (eachline[0].compare("CIRCLE") == 0) {

            CircCollider c;

            c.pos = QPointF(eachline[1].toFloat(), eachline[2].toFloat());
            c.rad = eachline[3].toFloat();
            if (eachline[4].compare("IN") == 0) {
                c.rad -= 1.0f;
                c.stay_inside = true;
            }
            else {
                c.rad += 1.0f;
                c.stay_inside = false;
            }

            circ_colliders.push_back(c);

        }
        else {

            const float x1 = eachline[0].toFloat();
            const float y1 = eachline[1].toFloat();
            const float x2 = eachline[2].toFloat();
            const float y2 = eachline[3].toFloat();

            //NOTE: colliders are padded by 1 unit to enforce player's size and prevent viewplane clipping
            QRectF c;
            c.setBottomLeft(QPointF(qMin(x1, x2) - 1, qMin(y1, y2) - 1));
            c.setTopRight(QPointF(qMax(x1, x2) + 1, qMax(y1, y2) + 1));
            colliders.push_back(c);

        }

    }

    //read mount points
    eachline = ifs.readLine().split(" ");
    const int nMounts = eachline.last().toInt();

    for (int i=0; i<nMounts; ++i) {

        eachline = ifs.readLine().split(" ");

        const float px = eachline[0].toFloat();
        const float py = eachline[1].toFloat();
        const float pz = eachline[2].toFloat();

        const float dx = eachline[3].toFloat();
        const float dy = eachline[4].toFloat();
        const float dz = eachline[5].toFloat();

        mount_pts.push_back(QVector3D(px, py, pz));
        mount_dirs.push_back(QVector3D(dx, dy, dz));

    }

    file.close();
}

QVector3D EnvRoomTemplate::ClipPlayerTrajectory(const QVector3D & pos, const QVector3D & vel) const
{

    QVector3D new_vel;

    bool x_collide = false;
    bool z_collide = false;

    QPointF move(pos.x() + vel.x(), pos.z() + vel.z());
    QPointF move_x(pos.x() + vel.x(), pos.z());
    QPointF move_z(pos.x(), pos.z() + vel.z());

    for (int i=0; i<colliders.size(); ++i) {

        if (colliders[i].contains(move)) {

            if (colliders[i].contains(move_x)) {
                x_collide = true;
            }

            if (colliders[i].contains(move_z)) {
                z_collide = true;
            }

        }

    }

    if (!x_collide && z_collide) {
        new_vel = QVector3D(vel.x(), 0, 0);
    }
    else if (!z_collide && x_collide) {
        new_vel = QVector3D(0, 0, vel.z());
    }
    else if (x_collide && z_collide) {
        new_vel = QVector3D(0, 0, 0);
    }
    else {
        new_vel = vel;
    }

    move = QPointF(pos.x() + new_vel.x(), pos.z() + new_vel.z());
    for (int i=0; i<circ_colliders.size(); ++i) {

        QVector3D cent_dir = QVector3D(circ_colliders[i].pos.x() - move.x(), 0, circ_colliders[i].pos.y() - move.y());
        const float cent_dist = cent_dir.length();

        if ((circ_colliders[i].stay_inside && cent_dist > circ_colliders[i].rad) ||
             (!circ_colliders[i].stay_inside && cent_dist < circ_colliders[i].rad)) {
            new_vel += cent_dir.normalized() * (cent_dist - circ_colliders[i].rad);
        }

    }

    /*
    qDebug() << "the mountpoints:";
    for (int angle=0; angle<=360; angle+=18) {

        if (angle % 36 == 0) {
            qDebug() << sinf(float(angle)* MathUtil::_PI_OVER_180) * 20.0f << "0" << cosf(float(angle)* MathUtil::_PI_OVER_180) * 20.0f
                     << -sinf(float(angle)* MathUtil::_PI_OVER_180) << "0" << -cosf(float(angle)* MathUtil::_PI_OVER_180);
        }
        else {
            qDebug() << sinf(float(angle)* MathUtil::_PI_OVER_180) * 12.1f << "0" << cosf(float(angle)* MathUtil::_PI_OVER_180) * 12.1f
                        << sinf(float(angle)* MathUtil::_PI_OVER_180) << "0" << cosf(float(angle)* MathUtil::_PI_OVER_180);
        }
    }
    */

    return new_vel;

}

void EnvRoomTemplate::GetMount(const int index, QVector3D & pt, QVector3D & dir) const
{

    if (index < 0 || index >= mount_pts.size()) {
        qDebug() << "EnvRoomTemplate::GetMount - Warning, index was " << index << "but only " << mount_pts.size() << "mountpoints.";
        return;
    }

    pt = mount_pts[index];
    dir = mount_dirs[index];
}

int EnvRoomTemplate::GetNumMounts() const
{
    return mount_pts.size();
}
