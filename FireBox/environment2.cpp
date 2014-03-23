#include "environment2.h"

Environment2::Environment2()
{
    player_curroom = 0;
    nTemplates = 6;
}

Environment2::~Environment2()
{

}

void Environment2::Reset()
{

    rooms.clear();
    portals.clear();

    EnvPortalRoom new_room;
    new_room.SetRoomTemplate(&room_templates[0]);    
    new_room.SetTitle("FireBox - Home and Bookmarks");
    new_room.SetURL("");
    rooms.push_back(new_room);        

    player_curroom = 0;

    alllinks.clear();

}


void Environment2::AddImage(const int room_index,const QImage & img)
{

    if (room_index < 0 || room_index >= rooms.size()) {
        qDebug() << "Environment2::AddPortal() - Warning Bad room index" << room_index;
        return;
    }

    if (rooms[room_index].NumMountPointsFree() <= 0) {
        qDebug() << "Environment2::AddPortal() - Warning room" << room_index << "has no more mountpoints";
        return;
    }

    rooms[room_index].AddEnvImage(img);

}

void Environment2::AddPortal(const int room_index, const QString & url)
{

    if (room_index < 0 || room_index >= rooms.size()) {
        qDebug() << "Environment2::AddPortal() - Warning Bad room index" << room_index;
        return;
    }

    if (rooms[room_index].NumMountPointsFree() <= 0) {
        qDebug() << "Environment2::AddPortal() - Warning room" << room_index << "has no more mountpoints";
        return;
    }

    //set up parent stuff for portal
    QVector3D parent_mount_pos;
    QVector3D parent_mount_dir;
    rooms[room_index].AddChildPortal(portals.size(), parent_mount_pos, parent_mount_dir);

    //set up child stuff for portal
    QVector3D child_mount_pos;
    QVector3D child_mount_dir;
    EnvPortalRoom child_room;
    child_room.SetRoomTemplate(&room_templates[rand() % nTemplates]); //TODO: make this dependent on the page's actual content
    //child_room.SetRoomTemplate(&room_templates[5]); //test specific rooms (for debugging)
    child_room.SetParentPortal(portals.size(), child_mount_pos, child_mount_dir);
    float newcol = float(portals.size() * 19  % 15) / 15.0f;
    QVector3D child_col = (QVector3D(2,2,2) + huecycle(newcol)) * 0.33f;
    child_room.SetColour(child_col);
    child_room.SetURL(url);

    EnvPortal new_portal;
    new_portal.SetParentRoom(room_index);
    new_portal.SetChildRoom(rooms.size());
    new_portal.SetURL(url);
    new_portal.SetParentPos(parent_mount_pos);
    new_portal.SetParentDir(parent_mount_dir);
    new_portal.SetChildPos(child_mount_pos);
    new_portal.SetChildDir(child_mount_dir);
    new_portal.SetParentColour(rooms[room_index].GetColour());
    new_portal.SetChildColour(child_col);
    new_portal.SetChildTitle(rooms[room_index].GetTitle());
    new_portal.SetChildURL(rooms[room_index].GetURL());

    portals.push_back(new_portal);
    rooms.push_back(child_room);

}

void Environment2::LoadRoomTemplates()
{

    //qDebug() << "Environment2::LoadRoomTemplates()";
    //TODO: make this actually go through the directory and figure out how many rooms there are, rather than hardcoding
    for (int i=0; i<nTemplates; ++i) {
        EnvRoomTemplate rt;
        room_templates.push_back(rt);
    }

    room_templates[0].Load("assets/rooms/room1.obj", "assets/rooms/room1.png", "assets/rooms/room1.txt");
    room_templates[1].Load("assets/rooms/room2.obj", "assets/rooms/room2.png", "assets/rooms/room2.txt");
    room_templates[2].Load("assets/rooms/room3.obj", "assets/rooms/room3.png", "assets/rooms/room3.txt");
    room_templates[3].Load("assets/rooms/room4.obj", "assets/rooms/room4.png", "assets/rooms/room4.txt");
    room_templates[4].Load("assets/rooms/room5.obj", "assets/rooms/room5.png", "assets/rooms/room5.txt");
    room_templates[5].Load("assets/rooms/room6.obj", "assets/rooms/room6.png", "assets/rooms/room6.txt");

}

void Environment2::initializeGL()
{

    //qDebug() << "Environment2::initializeGL()";
    LoadRoomTemplates();
    skybox.LoadTextures();

}

void Environment2::DrawGL(Player & player) {

    const QList <int> & child_portals = rooms[player_curroom].GetChildPortals();    
    const int parent_portal = rooms[player_curroom].GetParentPortal();    

    //first compute a list of portals for culling (either not facing player direction, or player not within portal's front facing halfspace)
    int culled_portals = 0;
    QVector <bool> cull_portals = QVector<bool>(child_portals.size(), false);

    for (int i=0; i<child_portals.size(); ++i) {

        QVector3D dir_to_portal = portals[child_portals[i]].GetParentPos() - player.Pos();
        QVector3D portal_dir = portals[child_portals[i]].GetParentDir();
        //qDebug() << dir_to_portal << player.Dir() << MathUtil::GetAngleBetweenRadians(dir_to_portal, player.Dir()) ;

        if ((MathUtil::GetAngleBetweenRadians(dir_to_portal, player.Dir()) < MathUtil::_PI_OVER_2 &&
             MathUtil::GetAngleBetweenRadians(dir_to_portal, -portal_dir) < MathUtil::_PI_OVER_2)
                || fabsf(dir_to_portal.x()) < 1.0f || fabsf(dir_to_portal.z()) < 1.0f) {
            cull_portals[i] = false;
        }
        else {
            cull_portals[i] = true;
            ++culled_portals;
        }

    }    
    //qDebug() << "culled portals: " << culled_portals << "/" << cull_portals.size();

    //draw "backmost" skybox
    skybox.DrawGL();

    //draw child portals (and their skyboxes as seen through portal)
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    //1. draw the room to depth buffer (no colour)
    glClear(GL_DEPTH_BUFFER_BIT);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    rooms[player_curroom].DrawGL();

    //2. draw each portal stencil with it's unique index, writing to depth buffer, and taking index into account
    glClear(GL_STENCIL_BUFFER_BIT);
    for (int i=0; i<child_portals.size(); ++i) {
        if (cull_portals[i]) {
            continue;
        }

        glStencilFunc(GL_ALWAYS, i+1, 0xff);
        portals[child_portals[i]].DrawParentStencilGL();

    }
    if (parent_portal >= 0) {
        glStencilFunc(GL_ALWAYS, child_portals.size()+1, 0xff);
        portals[parent_portal].DrawChildStencilGL();
    }
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    //3.  for each stencil, draw within it the proper view
    for (int i=0; i<child_portals.size(); ++i) {
        if (cull_portals[i]) {
            continue;
        }

        glClear(GL_DEPTH_BUFFER_BIT);
        glStencilFunc(GL_EQUAL, i+1, 0xff);
        DrawRoomWithinPortalStencilGL(child_portals[i], true);

    }
    if (parent_portal >= 0) {
        glClear(GL_DEPTH_BUFFER_BIT);
        glStencilFunc(GL_EQUAL, child_portals.size()+1, 0xff);
        DrawRoomWithinPortalStencilGL(parent_portal, false);
    }

    glDisable(GL_STENCIL_TEST);

    //draw portal placeholders in current room
    glClear(GL_DEPTH_BUFFER_BIT);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    for (int i=0; i<child_portals.size(); ++i) {
        if (cull_portals[i]) {
            continue;
        }

        EnvPortal & portal = portals[child_portals[i]];
        portal.DrawParentStencilGL();
    }
    if (parent_portal >= 0) {
        EnvPortal & portal = portals[parent_portal];
        portal.DrawChildStencilGL();
    }
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    //finally draw room
    rooms[player_curroom].DrawGL();
    for (int i=0; i<child_portals.size(); ++i) {
        //if (cull_portals[i]) {
        //    continue;
        //}
        EnvPortal & portal = portals[child_portals[i]];
        portal.DrawParentGL();
    }
    if (parent_portal >= 0) {
        EnvPortal & portal = portals[parent_portal];
        portal.DrawChildGL();
    }

}

void Environment2::DrawRoomWithinPortalStencilGL(const int portal_index, const bool within_parent_room)
{

    EnvPortal & portal = portals[portal_index];

    glPushMatrix();

    //room needs to be drawn RELATIVE to the player's position/view within that world
    QVector3D d1 = portal.GetParentDir();
    QVector3D d2 = portal.GetChildDir();
    QVector3D p1 = portal.GetParentPos() + d1 * EnvPortal::GetPortalSpacing();
    QVector3D p2 = portal.GetChildPos() + d2 * EnvPortal::GetPortalSpacing();

    if (!within_parent_room) {
        qSwap(d1, d2);
        qSwap(p1, p2);
    }

    float angle = MathUtil::GetSignedAngleBetweenRadians(d2, -d1) * MathUtil::_180_OVER_PI;
    //qDebug() << p1 << d1 << p2 << d2 << angle;

    glPushMatrix();
    glRotatef(angle, 0, 1, 0);
    skybox.DrawGL();
    glPopMatrix();

    GLdouble plane_eqn[4];
    plane_eqn[0] = -d1.x();
    plane_eqn[1] = -d1.y();
    plane_eqn[2] = -d1.z();
    plane_eqn[3] = QVector3D::dotProduct(d1, p1);

    glClipPlane(GL_CLIP_PLANE0, plane_eqn);
    glEnable(GL_CLIP_PLANE0);

    glTranslatef(p1.x(), p1.y(), p1.z());
    glRotatef(angle, 0, 1, 0);
    glTranslatef(-p2.x(), -p2.y(), -p2.z());

    //draw the room itself
    const int draw_room_index = within_parent_room ? portal.GetChildRoom() : portal.GetParentRoom();
    EnvPortalRoom & draw_room = rooms[draw_room_index];
    draw_room.DrawGL();

    //draw stand-in portals for these rooms (just quads)    
    const QList <int> & child_portals = draw_room.GetChildPortals();
    const int parent_portal = draw_room.GetParentPortal();

    for (int i=0; i<child_portals.size(); ++i) {

        EnvPortalRoom & child_room = rooms[portals[child_portals[i]].GetChildRoom()];
        QVector3D room_colour = child_room.GetColour();

        if (portal_index != child_portals[i]) {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glColor4f(room_colour.x()-0.25f, room_colour.y()-0.25f, room_colour.z()-0.25f, 0.5f);
            portals[child_portals[i]].DrawParentStencilGL();
            glDisable(GL_BLEND);
        }

        portals[child_portals[i]].DrawParentGL();

    }
    if (parent_portal >= 0) {
        EnvPortalRoom & parent_room = rooms[portals[parent_portal].GetParentRoom()];
        QVector3D room_colour = parent_room.GetColour();

        if (portal_index != parent_portal) {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glColor4f(room_colour.x()-0.25f, room_colour.y()-0.25f, room_colour.z()-0.25f, 0.5f);
            portals[parent_portal].DrawChildStencilGL();
            glDisable(GL_BLEND);
        }

        portals[parent_portal].DrawChildGL();
    }

    glDisable(GL_CLIP_PLANE0);

    glPopMatrix();

}

void Environment2::MovePlayer(const int portal_index, const bool go_parent_to_child, Player & player)
{

    EnvPortal & portal = portals[portal_index];

    //update player's active room
    player_curroom = go_parent_to_child ? portal.GetChildRoom() : portal.GetParentRoom();

    //also update player's position, etc., we have to transform them seamlessly to a new world coordinate
    QVector3D d1 = portal.GetParentDir();
    QVector3D d2 = portal.GetChildDir();
    QVector3D p1 = portal.GetParentPos() + d1 * EnvPortal::GetPortalSpacing();
    QVector3D p2 = portal.GetChildPos() + d2 * EnvPortal::GetPortalSpacing();

    if (!go_parent_to_child) {
        qSwap(d1, d2);
        qSwap(p1, p2);
    }

    const float angle = MathUtil::GetSignedAngleBetweenRadians(-d2, d1); //note: d1 and d2 swapped

    const QVector3D x1 = player.Pos() + player.Velocity();
    QVector3D x2 = x1;
    x2 -= p1;
    x2 = MathUtil::GetRotatedAxis(-angle, x2, QVector3D(0, 1, 0));
    x2 += p2;

    player.Pos(x2);
    player.SpinView(angle * MathUtil::_180_OVER_PI);

}

bool Environment2::Update_CrossPortals(Player & player)
{

    //detect if player crosses a portal
    const QList <int> & child_portals = rooms[player_curroom].GetChildPortals();
    for (int i=0; i<child_portals.size(); ++i) {

        EnvPortal & each_portal = portals[child_portals[i]];

        if (each_portal.GetPlayerCrossedParent(player.Pos(), player.Velocity())) {

            //qDebug() << "detected crossing with portal" << child_portals[i] << " parent travelling to" << each_portal.GetChildRoom();
            MovePlayer(child_portals[i], true, player);

            return true;

        }
    }

    const int parent_portal = rooms[player_curroom].GetParentPortal();
    if (parent_portal >= 0) {
        const EnvPortal & each_portal = portals[parent_portal];

        if (each_portal.GetPlayerCrossedChild(player.Pos(), player.Velocity())) {

            //qDebug() << "detected crossing with portal" << parent_portal << " child travelling to" << each_portal.GetParentRoom();
            MovePlayer(parent_portal, false, player);

            return true;

        }
    }

    return false;

}

void Environment2::Update(Player & player)
{   

    //clip player's velocity (do collision detection to modify velocity vector)
    const QVector3D new_vel = ClipPlayerTrajectory(player.Pos(), player.Velocity());
    player.Velocity(new_vel);

    //detect if player crosses a portal
    const QList <int> & child_portals = rooms[player_curroom].GetChildPortals();
    for (int i=0; i<child_portals.size(); ++i) {

        EnvPortal & each_portal = portals[child_portals[i]];

        //detect if player nears a portal
        if (!each_portal.StartedURLRequest() &&
                each_portal.GetPlayerNearParent(player.Pos()) &&
                each_portal.GetPlayerFacingParent(player.Dir()) &&
                !player.Flying() &&
                !player.FlightMode()) { //detect opening

            SoundManager::Play(3, false);
            each_portal.StartURLRequest();

        }

    }

    for (int i=0; i<portals.size(); ++i) {

        EnvPortal & each_portal = portals[i];

        if (!each_portal.Processed() && each_portal.Loaded()) { //detect room is "opened" (we have the data back from http request)

            //mark processed           
            each_portal.Processed(true);

            //time to build a room, based on contents here
            HTMLPage * page = each_portal.Page();

            //update portal with page title
            each_portal.SetParentTitle(page->Title());
            rooms[each_portal.GetChildRoom()].SetTitle(page->Title());

            //play a custom sound effect for the page
            //SoundManager::Stop(3);
            if (page->NumLinks() > 10 && page->NumImages() > 10) {
                SoundManager::Play(13, false);
            }
            else if (page->NumImages() > 10) {
                SoundManager::Play(10, false);
            }
            else if (page->NumLinks() > 10) {
                SoundManager::Play(12, false);
            }
            else if (page->NumTexts() > 10) {
                SoundManager::Play(11, false);
            }
            else if (page->NumLinks() == 1) {
                SoundManager::Play(9, false);
            }
            else if (page->NumLinks() == 0) {
                SoundManager::Play(8, false);
            }
            else {
                SoundManager::Play(14, false);
            }

            const int roomind = each_portal.GetChildRoom();

            //inhabit the room with the page's data
            for (int j=0; j<page->Data().size(); ++j) {

                //qDebug() << page->Data()[j].type << page->Data()[j].data;

                //but stop when there's no more space to slot stuff
                if (rooms[roomind].NumMountPointsFree() <= 0) {
                    break;
                }

                if (page->Data()[j].type == LINK) {

                    //add link only if not already done
                    QUrl eachurl(page->Data()[j].data);
                    eachurl.setFragment(QString()); //this unsets fragment (i.e. stuff after #)
                    QString urldecode = eachurl.toString();

                    if (alllinks.contains(urldecode)) {
                        continue;
                    }

                    //AddNewEntrance(roomind, page->Data()[j].data);
                    AddPortal(roomind, urldecode);

                    alllinks.insert(urldecode);

                }
                else if (page->Data()[j].type == IMAGE) {

                    //attempt to sanitize the url for the image
                    QString img_url = page->Data()[j].data;
                    while (img_url.contains("//")) {
                        img_url = img_url.right(img_url.length()-img_url.indexOf("//")-2);
                    }

                    QString img_url_extension = img_url.right(4);
                    if (!img_url.contains("pixel")) {
                        //qDebug() << "adding image with" << img_url;
                        rooms[roomind].AddEnvImage(QString("http://")+img_url);
                    }
                }
                else if (page->Data()[j].type == TEXT) {

                    QString text = page->Data()[j].data.simplified();
                    if (EnvText::WorthyOfTextBox(text)) {
                        //AddNewEnvText(roomind, text);
                        rooms[roomind].AddEnvText(page->Data()[j].data);
                    }

                }

            }

        }

    }

    if (!player.Flying() && !player.FlightMode()) {

        bool crossed_portal = Update_CrossPortals(player);

        if (!crossed_portal) {
            //update player's position
            player.Pos(player.Pos() + new_vel);
        }

    }
    else {
        player.Velocity(QVector3D(0, 0, 0));
    }

}

QVector3D Environment2::ClipPlayerTrajectory(const QVector3D & pos, const QVector3D & vel) const
{

    if (player_curroom >= 0 && player_curroom < rooms.size()) {
        return rooms[player_curroom].ClipPlayerTrajectory(portals, pos, vel);
    }
    else {
        return QVector3D(0, 0, 0);
    }

}

int Environment2::PlayerCurRoom() const
{
    return player_curroom;
}

bool Environment2::PlayerCanGoBack() const
{
    const int parent_portal = rooms[player_curroom].GetParentPortal();
    return (parent_portal >= 0);
}

QVector3D Environment2::PlayerBackPos() const
{
    const int parent_portal = rooms[player_curroom].GetParentPortal();

    if (parent_portal >= 0) {
        const EnvPortal & portal = portals[parent_portal];
        return portal.GetChildPos() + portal.GetChildDir();
    }

    return QVector3D(0, 0, 0);
}

QVector3D Environment2::PlayerBackDir() const
{
    const int parent_portal = rooms[player_curroom].GetParentPortal();

    if (parent_portal >= 0) {
        const EnvPortal & portal = portals[parent_portal];
        return portal.GetChildDir();
    }

    return QVector3D(0, 0, 1);
}

QVector3D Environment2::huecycle(double val)
{

    val *= 6;

    if (0 <= val && val < 1) {
        return QVector3D(0, val, 1.0);
    }
    else if (1 <= val && val < 2) {
        return QVector3D(0, 1, 1 - (val-1));
    }
    else if (2 <= val && val < 3) {
        return QVector3D(val-2, 1, 0);
    }
    else if (3 <= val && val < 4) {
        return QVector3D(1, 1 - (val-3), 0);
    }
    else if (4 <= val && val < 5) {
        return QVector3D(1, 0, val-4);
    }
    else if (5 <= val && val < 6) {
        return QVector3D(1 - (val-5), 0, 1);
    }
    else {
        return QVector3D(1, 1, 1);
    }

}
