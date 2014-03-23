#include "environment.h"

Environment::Environment(Player * p) :
    maxroomperimetersize(300)
{   

    envsize = 512;
    entrancesize = 3;

    nullcell.State(EMPTY);
    nullcell.Owner(-1);

    cells.resize(envsize);
    for (int i=0; i<cells.size(); ++i) {
        cells[i].resize(envsize);
    }

    for (int i=0; i<cells.size(); ++i) {
        for (int j=0; j<cells[i].size(); ++j) {

            cells[i][j].X(i);
            cells[i][j].Y(j);           

            EnvCell * left = NULL;
            EnvCell * right = NULL;
            EnvCell * up = NULL;
            EnvCell * down = NULL;

            EnvCell * upleft = NULL;
            EnvCell * upright = NULL;
            EnvCell * downleft = NULL;
            EnvCell * downright = NULL;

            //left's
            if (i > 0) {
                left = &cells[i-1][j];
                if (j > 0) {
                    upleft = &cells[i-1][j-1];
                }
                if (j < envsize-1) {
                    downleft = &cells[i-1][j+1];
                }
            }

            //right's
            if (i < envsize-1) {
                right = &cells[i+1][j];
                if (j > 0) {
                    upright = &cells[i+1][j-1];
                }
                if (j < envsize-1) {
                    downright = &cells[i+1][j+1];
                }
            }

            //up
            if (j > 0) {
                up = &cells[i][j-1];
            }

            //down
            if (j < envsize-1) {
                down = &cells[i][j+1];
            }

            cells[i][j].Neighbours(left, right, up, down, upleft, upright, downleft, downright);

        }

    }

    tex_tile = 0;

    player_curroom = -1;

    disp_list = 0;
    rebuild_list = true;

    player = p;
}

Environment::~Environment()
{

    if (tex_tile > 0) {

        glDeleteTextures(1, &tex_tile);
        tex_tile = 0;

    }

    DeleteDisplayList();

    qDeleteAll(rooms);
    qDeleteAll(entrances);
    qDeleteAll(roomtitles);

}

void Environment::LoadTextures()
{

    if (tex_tile == 0) {

        QImage tile_img("assets/tile.png");
        tex_tile = EnvObject::LoadTexture(tile_img);

    }

    skybox.LoadTextures();

}

int Environment::AddNewSpace(EnvEntrance * ent, int minarea, int minperim, int x1, int y1, int x2, int y2)
{   

    if (x1 > x2) {
        qSwap(x1, x2);
    }
    if (y1 > y2) {
        qSwap(y1, y2);
    }

    //--x1; --y1; ++x2; ++y2;
    if (ent != NULL) {
        if (ent->CDir() == LEFT) {
            --x1;
        }
        else if (ent->CDir() == RIGHT) {
            ++x2;
        }
        else if (ent->CDir() == UP) {
            --y1;
        }
        else if (ent->CDir() == DOWN) {
            ++y2;
        }
    }

    DoGrowRoom(minarea, minperim, x1, y1, x2, y2);

    const int roomind = rooms.size();

    QVector3D entrancemid;

    if (ent) {
        entrancemid = ent->BoundsMid();
    }
    else {
        entrancemid = QVector3D((x1+x2)/2, 0.0f, (y1+y2)/2);
    }

    for (int i=x1; i<=x2; ++i) {
        for (int j=y1; j<=y2; ++j) {

            if (CheckNeighboursEmpty(i, j, roomind)) {

                cells[i][j].State(EMPTY);
                cells[i][j].Owner(roomind);
                //qDebug() << "Setting cell" << i << j << "to state EMPTY and owner" << roomind << cells[i][j].Owner();
                //cells[i][j].Animate(true);
                //cells[i][j].TimeOffset((QVector3D(i,0,j) - entrancemid).length() * 0.5f);
                //cells[i][j].StartTime();

            }

        }
    }

    EnvRoom * newRoom = new EnvRoom(roomind);
    newRoom->SetBounds(x1, y1, x2, y2);

    rooms.push_back(newRoom);

    rebuild_list = true;

    return roomind;
}

void Environment::AddNewEntrance(const int roomind, const QString & url)
{   

    int x1, y1, x2, y2;
    CellDir d;

    if (FindFreePerimeterPart(roomind, entrancesize+2, 0, x1, y1, x2, y2, d)) {

        if (d == UP) {
            y1 -= 1;
            //y2 -= 1;
            ++x1;
            --x2;
        }
        else if (d == DOWN) {
            //y1 += 1;
            y2 += 1;
            ++x1;
            --x2;
        }
        else if (d == LEFT) {
            x1 -= 1;
            //x2 -= 1;
            ++y1;
            --y2;
        }
        else if (d == RIGHT) {
            //x1 += 1;
            x2 += 1;
            ++y1;
            --y2;
        }

        for (int i=x1; i<=x2; ++i) {
            for (int j=y1; j<=y2; ++j) {

                cells[i][j].State(ENTRANCE);
                cells[i][j].Owner(roomind);

            }
        }

        if (d == UP) {
            cells[x1-1][y2].State(OCCUPIED);
            cells[x2+1][y2].State(OCCUPIED);
        }
        else if (d == DOWN) {
            cells[x1-1][y1].State(OCCUPIED);
            cells[x2+1][y1].State(OCCUPIED);
        }
        else if (d == LEFT) {
            cells[x2][y1-1].State(OCCUPIED);
            cells[x2][y2+1].State(OCCUPIED);
        }
        else if (d == RIGHT) {
            cells[x1][y1-1].State(OCCUPIED);
            cells[x1][y2+1].State(OCCUPIED);
        }

        /*
        if (d == UP) {
            //y1 -= 2;
            --x1;
            ++x2;
        }
        else if (d == DOWN) {
            //y2 += 2;
            --x1;
            ++x2;
        }
        else if (d == LEFT) {
            //x1 -= 2;
            --y1;
            ++y2;
        }
        else if (d == RIGHT) {
            //x2 += 2;
            --y1;
            ++y2;
        }
        */

        EnvEntrance * newenter = new EnvEntrance(url, entrancesize);        
        newenter->SetBounds(x1, y1, x2, y2);
        newenter->SetDir(EnvCell::CellDirToVector(d));
        newenter->SetCDir(d);
        entrances.push_back(newenter);

        qDebug() << "Adding entrance to" << url << "at" << x1 << y1 << EnvCell::CellDirToName(d);

        //room keeps list of entrances
        rooms[roomind]->AddChildEntrance(newenter);
        //entrance keeps this room as its parent
        newenter->ParentRoom(rooms[roomind]);

    }

    rebuild_list = true;

}

void Environment::AddNewEnvImage(const int roomind, const QImage & image)
{

    int x1, y1, x2, y2;
    CellDir d;

    if (FindFreePerimeterPart(roomind, 1, 0, x1, y1, x2, y2, d)) {

        for (int i=x1; i<=x2; ++i) {
            for (int j=y1; j<=y2; ++j) {
                cells[i][j].State(OCCUPIED);
            }
        }

        rooms[roomind]->AddEnvImage(float(x1+x2)/2.0f, float(y1+y2)/2.0f, d, image);

        //qDebug() << "Adding environment image" << url << "at" << x1 << y1 << EnvCell::CellDirToName(d);

    }

    rebuild_list = true;

}

void Environment::AddNewEnvImage(const int roomind, const QString & url)
{   

    int x1, y1, x2, y2;
    CellDir d;

    if (FindFreePerimeterPart(roomind, 1, 0, x1, y1, x2, y2, d)) {

        for (int i=x1; i<=x2; ++i) {
            for (int j=y1; j<=y2; ++j) {
                cells[i][j].State(OCCUPIED);
            }
        }

        rooms[roomind]->AddEnvImage(float(x1+x2)/2.0f, float(y1+y2)/2.0f, d, url);

        //qDebug() << "Adding environment image" << url << "at" << x1 << y1 << EnvCell::CellDirToName(d);

    }

    rebuild_list = true;

}

void Environment::AddNewEnvText(const int roomind, const QString & text)
{   

    int x1, y1, x2, y2;
    CellDir d;

    if (FindFreePerimeterPart(roomind, 0, 0, x1, y1, x2, y2, d)) {

        cells[x1][y1].State(OCCUPIED);
        rooms[roomind]->AddEnvText(x1, y1, d, text);

    }


    rebuild_list = true;

}

EnvCell & Environment::Cell(int x, int y) {

    if (x<0 || x>=envsize || y<0 || y>=envsize)
        return nullcell;
    else
        return cells[x][y];

}

bool Environment::DoGrowRoom(int minarea, int minperim, int & x1, int & y1, int & x2, int & y2)
{

    //int iter = 0;

    //enforce area and perimeter constraints
    while (EvaluatePerimeter(x1, y1, x2, y2) < minperim || EvaluateArea(x1, y1, x2, y2) < minarea) {

        //increment dimension maximally
        int cur = EvaluateRoom(minarea, minperim, x1, y1, x2, y2);
        int left = cur;
        int right = cur;
        int up = cur;
        int down = cur;

        //qDebug() << "iteration" << iter++ << cur << x1 << y1 << x2 << y2;

        //gradient-descent like approach
        if (x1 > 0) {
            left = EvaluateRoom(minarea, minperim, x1-1, y1, x2, y2);
        }
        if (x2 < envsize-1) {
            right = EvaluateRoom(minarea, minperim, x1, y1, x2+1, y2);
        }
        if (y1 > 0) {
            up = EvaluateRoom(minarea, minperim, x1, y1-1, x2, y2);
        }
        if (y2 < envsize-1) {
            down = EvaluateRoom(minarea, minperim, x1, y1, x2, y2+1);
        }

        if (left > right && left > up && left > down && left > cur) {
            --x1;
        }
        else if (right > up && right > down && right > cur) {
            ++x2;
        }
        else if (up > down && up > cur) {
            --y1;
        }
        else if (down > cur) {
            ++y2;
        }
        else {
            return false;
        }

        /*
        if (left > 0) {
            --x1;
        }
        if (right > 0) {
            ++x2;
        }
        if (up > 0) {
            --y1;
        }
        if (down > 0) {
            ++y2;
        }
        */

    }

    return true;

}

int Environment::EvaluateRoom(const int minarea, const int minperim, const int x1, const int y1, const int x2, const int y2)
{

    return qMin(minperim, EvaluatePerimeter(x1, y1, x2, y2)) + qMin(minarea, EvaluateArea(x1, y1, x2, y2));

}

int Environment::EvaluatePerimeter(int x1, int y1, int x2, int y2)
{

    int perim = 0;

    for (int i=x1; i<=x2; ++i) {

        if (Cell(i, y1).State() == EMPTY &&
            Cell(i, y1-1).State() == EMPTY &&
            Cell(i, y1+1).State() == EMPTY)
            ++perim;

        if (Cell(i, y2).State() == EMPTY &&
            Cell(i, y2-1).State() == EMPTY &&
            Cell(i, y2+1).State() == EMPTY)
            ++perim;

        /*
        if (Cell(i, y1).State() == EMPTY)  {
            ++perim;
        }
        if (Cell(i, y2).State() == EMPTY)  {
            ++perim;
        }
        */
    }

    for (int j=y1; j<=y2; ++j) {

        if (Cell(x1, j).State() == EMPTY &&
            Cell(x1-1, j).State() == EMPTY &&
            Cell(x1+1, j).State() == EMPTY)
            ++perim;

        if (Cell(x2, j).State() == EMPTY &&
            Cell(x2-1, j).State() == EMPTY &&
            Cell(x2+1, j).State() == EMPTY)
            ++perim;

        /*
        if (Cell(x1, j).State() == EMPTY)  {
            ++perim;
        }
        if (Cell(x2, j).State() == EMPTY)  {
            ++perim;
        }
        */

    }

    return perim;
}

int Environment::EvaluateArea(int x1, int y1, int x2, int y2)
{

    int area = 0;

    for (int i=x1; i<=x2; ++i) {
        for (int j=y1; j<=y2; ++j) {

            if (Cell(i, j).State() == EMPTY) {
                ++area;
            }

        }
    }

    return area;

}

QVector3D Environment::PlayerBackPos() const
{
    return player_backpos;
}

QVector3D Environment::PlayerBackDir() const
{
    return player_backdir;
}

int Environment::PlayerCurRoom() const
{
    return player_curroom;
}

void Environment::ClipPlayerVelocity()
{

    QVector3D pos = player->Pos();
    QVector3D vel = player->VelocityVector();

    int cell_x = pos.x();
    int cell_z = pos.z();
    int cell_vel_x = pos.x() + vel.x();
    int cell_vel_z = pos.z() + vel.z();

    if (CheckNineCollision(cell_vel_x, cell_vel_z)) {
        player->Pos(pos + vel);
    }
    else if (CheckNineCollision(cell_x, cell_vel_z)) {
        player->Pos(pos + QVector3D(0, 0, vel.z()));
    }
    else if (CheckNineCollision(cell_vel_x, cell_z)) {
        player->Pos(pos + QVector3D(vel.x(), 0, 0));
    }

}

void Environment::Update()
{   

    //update entrances, detect new openings
    for (int i=0; i<entrances.size(); ++i) {

        if (entrances[i]->Processed()) {
            continue;
        }

        if (!entrances[i]->StartedURLRequest()) { //detect opening

            if (entrances[i]->Near(player->Pos(), 2.0f)) {
                entrances[i]->StartURLRequest();
            }

        }
        else {

            if (entrances[i]->Loaded()) { //detect room is "opened" (we have the data back from http request)

                //time to build a room, based on contents here
                HTMLPage * page = entrances[i]->Page();

                //int minperim = (page->NumImages() + page->NumTexts()) * 1 + page->NumLinks() * (entrancesize + 4) + 8;
                int minperim = float(page->NumImages()) * 2.0f + page->NumLinks() * (entrancesize + 2) + 8;
                int minarea = 64;

                minperim = qMin(maxroomperimetersize, minperim);
                //qDebug() << "Building room with minimum perimeter score:" << minperim;

                int x1, y1, x2, y2;
                entrances[i]->GetBounds(x1, y1, x2, y2);

                int roomind = AddNewSpace(entrances[i], minarea, minperim, x1, y1, x2, y2);

                //inhabit the room with the page's data
                for (int j=0; j<page->Data().size(); ++j) {

                    if (page->Data()[j].type == LINK) {

                        //add link only if not already done
                        QUrl eachurl(page->Data()[j].data);
                        eachurl.setFragment(QString()); //this unsets fragment (i.e. stuff after #)
                        QString urldecode = eachurl.toString();

                        if (alllinks.contains(urldecode)) {
                            continue;
                        }

                        AddNewEntrance(roomind, page->Data()[j].data);
                        alllinks.insert(urldecode);

                    }
                    else if (page->Data()[j].type == IMAGE) {
                        AddNewEnvImage(roomind, page->Data()[j].data);
                    }
                    else if (page->Data()[j].type == TEXT) {

                        QString text = page->Data()[j].data.simplified();
                        if (EnvText::WorthyOfTextBox(text)) {
                            AddNewEnvText(roomind, text);
                        }

                    }

                }

                //entrance also keeps newly constructed room as its child
                entrances[i]->ChildRoom(rooms[roomind]);
                //room keeps generating entrance as its parent
                rooms[roomind]->ParentEntrance(entrances[i]);

                //with a parent/child relationship set, we now add "title" environment entities
                //on both sides of the doorway, using the names of each room

                EnvEntrance * pentrance = entrances[i]->ParentRoom()->ParentEntrance();

                EnvTitle * ptitle = new EnvTitle(entrancesize);
                EnvTitle * ctitle = new EnvTitle(entrancesize);

                if (pentrance) {
                    ptitle->Title(pentrance->Page()->Title());
                }
                else {
                    ptitle->Title(QString("about::blank"));
                }
                ptitle->Pos(entrances[i]->BoundsMid() + entrances[i]->Dir());
                ptitle->Dir(entrances[i]->Dir());
                ctitle->Title(entrances[i]->Page()->Title());
                ctitle->Pos(entrances[i]->BoundsMid());
                ctitle->Dir(entrances[i]->Dir() * (-1.0f));

                roomtitles.push_back(ptitle);
                roomtitles.push_back(ctitle);

                //mark as processed
                entrances[i]->Processed(true);

            }
        }

    }

    //determine from where user is stuff (i.e. where to "go back", etc.)
    if (player_curroom == -1 || !rooms[player_curroom]->Inside(player->Pos())) {

        player_backpos = player->Pos();
        player_backdir = player->Dir();

        //player is not in a room, or player just changed rooms, so we update
        for (int i=0; i<rooms.size(); ++i) {

            if (rooms[i]->Inside(player->Pos())) {

                player_curroom = i;

                if (rooms[i]->ParentEntrance()) { //if there's a "parent entrance", we can "go back one"

                    player_backpos = QVector3D(0, player->Height(), 0) + rooms[i]->ParentEntrance()->BoundsMid() - rooms[i]->ParentEntrance()->Dir() * 4.0f;
                    player_backdir = rooms[i]->ParentEntrance()->Dir();

                }

                break;

            }

        }

    }

}

bool Environment::FindFreePerimeterPart(const int roomind, const int len, const int space, int & x1, int & y1, int & x2, int & y2, CellDir & d)
{

    int bx1, by1, bx2, by2;
    rooms[roomind]->GetBounds(bx1, by1, bx2, by2);   

    QList <int> x0;
    QList <int> y0;
    QList <int> xsize;
    QList <int> ysize;
    QList <CellDir> xd;

    for (int i=by1; i<=by2; ++i) { //right
        x0.push_back(bx2);
        y0.push_back(i-space);
        xsize.push_back(0);
        ysize.push_back(len+space);
        xd.push_back(RIGHT);
    }
    for (int i=bx2; i>=bx1; --i) { //down
        x0.push_back(i-space);
        y0.push_back(by2);
        xsize.push_back(len+space);
        ysize.push_back(0);
        xd.push_back(DOWN);
    }
    for (int i=by2; i>=by1; --i) { //left
        x0.push_back(bx1);
        y0.push_back(i-space);
        xsize.push_back(0);
        ysize.push_back(len+space);
        xd.push_back(LEFT);
    }
    for (int i=bx1; i<=bx2; ++i) { //up
        x0.push_back(i-space);
        y0.push_back(by1);
        xsize.push_back(len+space);
        ysize.push_back(0);
        xd.push_back(UP);
    }

    //iterate through each cell of perimeter
    for (int c=0; c<x0.size(); ++c) {

        //qDebug() << "Searching for unblocked at position" << c;
        bool blocked = false;

        //for (int i=x0[c]-space; i<=x0[c]+xsize[c]+space; ++i) {
            //for (int j=y0[c]-space; j<=y0[c]+ysize[c]+space; ++j) {
        for (int i=x0[c]; i<=x0[c]+xsize[c]; ++i) {
            for (int j=y0[c]; j<=y0[c]+ysize[c]; ++j) {

                if (blocked) {
                    continue;
                }

                //cell is blocked if it's not empty or not mine
                bool cellnotempty = (Cell(i, j).State() != EMPTY);
                bool cellnotmine = (Cell(i, j).Owner() != roomind);

                if (cellnotempty || cellnotmine) {
                    //qDebug() << "blocked due to" << cellnotempty << cellnotmine << i << j << Cell(i, j).Owner() << roomind;
                    blocked = true;
                }
            }            
        }

        if (!blocked) {

            if (xd[c] == LEFT || xd[c] == RIGHT) {
                y0[c] += space;
                ysize[c] -= space;
            }
            else if (xd[c] == UP || xd[c] == DOWN) {
                x0[c] += space;
                xsize[c] -= space;
            }

            x1 = x0[c];
            x2 = x0[c]+xsize[c];
            y1 = y0[c];
            y2 = y0[c]+ysize[c];
            d = xd[c];
            //qDebug() << "FOUND UNBLOCKED PART ON WALL at index" << c;
            return true;
        }

    }

    return false;

}

void Environment::DrawGL()
{


    skybox.DrawGL();

    if (!rebuild_list) {

        glCallList(disp_list);

    }
    else {

        int num_quads = 0;

        rebuild_list = false;
        DeleteDisplayList();

        disp_list = glGenLists(1);
        glNewList(disp_list, GL_COMPILE_AND_EXECUTE);       

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, tex_tile);


        glBegin(GL_QUADS);
        for (int i=0; i<cells.size(); ++i) {
            for (int j=0; j<cells[i].size(); ++j) {
                cells[i][j].DrawGL();

                if (cells[i][j].Owner() >= 0) {
                    num_quads += 5;
                }
            }
        }
        glEnd();               

        glDisable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);

        for (int i=0; i<roomtitles.size(); ++i) {
            roomtitles[i]->DrawGL();
        }        

        glEndList();

        qDebug() << "Estimated num quads:" << num_quads;

    }


    for (int i=0; i<entrances.size(); ++i) {

        //if player's in the room, we draw the entrance stuff
        if (player_curroom == entrances[i]->ParentRoom()->RoomIndex() &&
                entrances[i]->Near(player->Pos(), 15.0f)) {

            entrances[i]->DrawGL();

        }

    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (int i=0; i<rooms.size(); ++i) {
        rooms[i]->DrawGL();
    }

    glDisable(GL_BLEND);



}

void Environment::DeleteDisplayList()
{

    if (disp_list > 0) {
        glDeleteLists(disp_list, 1);
    }
    disp_list = 0;

}

bool Environment::CheckNineCollision(const int i, const int j)
{

    if (!(i > 0 && i < cells.size()-1 && j > 0 && j < cells.first().size()-1)) {
        return false;
    }

    QList <EnvCell *> testcells;

    testcells.push_back(&cells[i-1][j-1]);
    testcells.push_back(&cells[i][j-1]);
    testcells.push_back(&cells[i+1][j-1]);

    testcells.push_back(&cells[i-1][j]);
    testcells.push_back(&cells[i][j]);
    testcells.push_back(&cells[i+1][j]);

    testcells.push_back(&cells[i-1][j+1]);
    testcells.push_back(&cells[i][j+1]);
    testcells.push_back(&cells[i+1][j+1]);

    for (int c=0; c<testcells.size(); ++c) {

        //if (testcells[c]->State() != ENTRANCE && testcells[c]->State() != OCCUPIED) {
        if (testcells[c]->Owner() == -1) {

            return false;

        }

    }

    return true;

}

bool Environment::CheckNeighboursEmpty(const int i, const int j, const int roomind)
{

    if (!(i > 0 && i < cells.size()-1 && j > 0 && j < cells.first().size()-1)) {
        return false;
    } 

    if (cells[i][j].State() != EMPTY) {
        return false;
    }

    QList <EnvCell *> testcells;

    testcells.push_back(&cells[i-1][j-1]);
    testcells.push_back(&cells[i][j-1]);
    testcells.push_back(&cells[i+1][j-1]);

    testcells.push_back(&cells[i-1][j]);
    testcells.push_back(&cells[i][j]);
    testcells.push_back(&cells[i+1][j]);

    testcells.push_back(&cells[i-1][j+1]);
    testcells.push_back(&cells[i][j+1]);
    testcells.push_back(&cells[i+1][j+1]);

    for (int c=0; c<testcells.size(); ++c) {

        if (testcells[c]->Owner() != roomind && testcells[c]->Owner() != -1 && testcells[c]->State() != ENTRANCE) {

            return false;

        }

    }

    return true;

}
