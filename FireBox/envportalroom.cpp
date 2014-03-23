#include "envportalroom.h"

EnvPortalRoom::EnvPortalRoom()
{
    cur_mount = 0;    
    parent_portal = -1;
    colour = QVector3D(1, 1, 1);
}

void EnvPortalRoom::SetRoomTemplate(EnvRoomTemplate * t)
{
    room_template = t;
}

void EnvPortalRoom::SetColour(const QVector3D & c)
{
    colour = c;
}

QVector3D EnvPortalRoom::GetColour() const
{
    return colour;
}

void EnvPortalRoom::SetTitle(const QString & s)
{
    title = s;
}

QString EnvPortalRoom::GetTitle() const
{
    return title;
}

void EnvPortalRoom::SetURL(const QString & s)
{
    url_str = s;
}

QString EnvPortalRoom::GetURL() const
{
    return url_str;
}

void EnvPortalRoom::AddEnvImage(const QImage & image)
{

    QVector3D mount_pt;
    QVector3D mount_dir;
    room_template->GetMount(cur_mount, mount_pt, mount_dir);
    ++cur_mount;

    EnvImg * newimg = new EnvImg(image);
    newimg->Pos(mount_pt);
    newimg->Dir(mount_dir);
    envimages.push_back(newimg);

    //qDebug() << "Added new image (local) at" << mount_pt << mount_dir;

}

void EnvPortalRoom::AddEnvImage(const QString & url)
{

    QVector3D mount_pt;
    QVector3D mount_dir;
    room_template->GetMount(cur_mount, mount_pt, mount_dir);
    ++cur_mount;

    QUrl theurl(url);

    EnvImg * newimg = new EnvImg(theurl);
    newimg->Pos(mount_pt);
    newimg->Dir(mount_dir);
    envimages.push_back(newimg);

    //qDebug() << "Added new image (URL) at" << mount_pt << mount_dir << url;

}

void EnvPortalRoom::AddEnvText(const QString & text)
{

    QVector3D mount_pt;
    QVector3D mount_dir;
    room_template->GetMount(cur_mount, mount_pt, mount_dir);
    ++cur_mount;

    EnvText * newtext = new EnvText(text);
    newtext->Pos(mount_pt);
    newtext->Dir(mount_dir);
    envtexts.push_back(newtext);

    //qDebug() << "Added new text at" << mount_pt << mount_dir;

}

void EnvPortalRoom::DrawGL()
{

    glColor3f(colour.x(), colour.y(), colour.z());
    room_template->DrawGL();

    glColor3f(1, 1, 1);
    for (int i=0; i<envimages.size(); ++i) {
        envimages[i]->DrawGL();
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (int i=0; i<envtexts.size(); ++i) {
        envtexts[i]->DrawGL();
    }

    glDisable(GL_BLEND);

}

int EnvPortalRoom::NumMountPointsFree()
{
    return room_template->GetNumMounts() - cur_mount;
}

void EnvPortalRoom::SetParentPortal(const int portal_index, QVector3D & pos, QVector3D & dir)
{
    parent_portal = portal_index;
    room_template->GetMount(cur_mount, pos, dir);
    ++cur_mount;
}

void EnvPortalRoom::AddChildPortal(const int portal_index, QVector3D & pos, QVector3D & dir)
{
    child_portals.push_back(portal_index);
    room_template->GetMount(cur_mount, pos, dir);
    ++cur_mount;
}

int EnvPortalRoom::GetParentPortal() const
{
    return parent_portal;
}

int EnvPortalRoom::GetNumChildPortals() const
{
    return child_portals.size();
}

QList <int> & EnvPortalRoom::GetChildPortals()
{
    return child_portals;
}

QVector3D EnvPortalRoom::ClipPlayerTrajectory(const QList <EnvPortal> & portals, const QVector3D & pos, const QVector3D & vel) const
{

    //determine if we are in any "portal regions"
    for (int i=0; i<child_portals.size(); ++i) {

        //within a parent portal
        if (portals[child_portals[i]].GetPlayerAtParent(pos+vel)) {
            return vel;
        }

    }

    if (parent_portal >= 0) {
        if (portals[parent_portal].GetPlayerAtChild(pos+vel)) {
            return vel;
        }
    }

    return room_template->ClipPlayerTrajectory(pos, vel);

}
