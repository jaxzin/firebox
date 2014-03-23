#include "game.h"

Game::Game() :
    player_height(2.25f),
    player_fly_height(40.0f),
    player_high(false)
{   

    player = new Player();
    player->Height(player_height);
    player->Pos(230.0f, player_height, 230.0f);

    QString instruct_text = QString("(Mouse left, right) - Rotate the view (about Y axis or 'yaw')");
    instruct_text += "(Tab) - Toggle visibility of URL entry widget";
    instruct_text += "(Enter) - Add URL as doorway in current room";
    instruct_text += "(Backspace) - Go back a webpage/room, or remove character when entering URL";
    instruct_text += "(W,A,S,D) - Translate (walk) around";
    instruct_text += "(F) - Toggle 'flying'' (lets you see the layout of the rooms from above, motion is fun in VR!)";
    instruct_text += "(G) - Toggle visibility of calibration grid and distortion parameters";
    instruct_text += "(1,2) - Adjust kappa_0";
    instruct_text += "(3,4) - Adjust kappa_1";
    instruct_text += "(5,6) - Adjust kappa_2";
    instruct_text += "(7,8) - Adjust kappa_3";
    instruct_text += "(F11) - Toggle fullscreen mode";

    //set up environment, add an initial space, and my homepage as a start entrance
    env = new Environment(player);
    int spaceind = env->AddNewSpace(NULL, 0, 0, 225, 225, 240, 240);

    env->AddNewEnvImage(spaceind, QImage("assets/instructions.png"));

    LoadBookmarks();
    for (int i=0; i<bookmarks_list.size(); ++i) {
        env->AddNewEntrance(spaceind, bookmarks_list[i]);
    }

    urlentrywidget = new URLEntryWidget(player);
    urlentrywidget->Hide();

}

void Game::Update()
{      

    player->Update();
    env->Update();

    env->ClipPlayerVelocity();

    //disable movement while widget is visible
    if (urlentrywidget->IsVisible()) {
        player->WalkForward(false);
        player->WalkBack(false);
        player->WalkLeft(false);
        player->WalkRight(false);
    }

}

void Game::DrawGL(const float half_ipd, const QVector3D & right, const QVector3D & up, const QVector3D & forward)
{

    //setup camera    
    player->SetViewGL(half_ipd, right, up, forward);

    //draw environment    
    env->DrawGL();   

    //draw "overlay" or vr widgets    
    urlentrywidget->DrawGL();

    //debugging stuff
    /*
    QVector3D pos = player->Pos();
    QVector3D dir = player->Dir();

    glPushMatrix();
    //glTranslatef(pos.x() + dir.x(), pos.y(), pos.z() + dir.z());
    glTranslatef(pos.x() + dir.x(), pos.y() - 1.0f, pos.z() + dir.z());
    glScalef(0.5f, 0.5, 0.5f);
    glBegin(GL_LINES);
    glColor3f(1, 0, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(right.x(), right.y(), right.z());
    glColor3f(0, 1, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(up.x(), up.y(), up.z());
    glColor3f(0, 0, 1);
    glVertex3f(0, 0, 0);
    glVertex3f(forward.x(), forward.y(), forward.z());
    glEnd();
    glPopMatrix();
    */

}

void Game::initializeGL()
{

    env->LoadTextures();

}

void Game::DrawKappas(const QList <float> & kappas)
{

    QString kappa_text;

    for (int i=0; i<4; ++i) {
        kappa_text += QString("kappa_") + QString::number(i) + ":" + QString::number(kappas[i]) + "\n";
    }

    /*
    EnvText env_text(kappa_text);

    env_text.Pos(player->Pos() + QVector3D(2, 0, 0));
    env_text.Dir(QVector3D(-1, 0, 0));

    env_text.DrawGL();
    */

}

void Game::mouseMoveEvent(float x, float)
{

    const float sensitivity = 0.1f;

    player->SpinView(x * sensitivity);    
    //player->TiltView(y * sensitivity);

}

void Game::mousePressEvent(QMouseEvent *)
{

}

void Game::mouseReleaseEvent(QMouseEvent *)
{

}

void Game::keyPressEvent(QKeyEvent * e)
{

    if (urlentrywidget->IsVisible()) {

        if (e->key() == Qt::Key_Tab) {
            urlentrywidget->Hide();
        }
        else if (e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return) {
            env->AddNewEntrance(env->PlayerCurRoom(), urlentrywidget->Text());
            urlentrywidget->Hide();
        }
        else if (e->key() == Qt::Key_Backspace) {
            urlentrywidget->RemoveLetter();
        }
        else {
            urlentrywidget->AddLetter(e->text());
        }

    }
    else {

        switch (e->key()) {

        case Qt::Key_W:
            player->WalkForward(true);
            break;

        case Qt::Key_S:
            player->WalkBack(true);
            break;

        case Qt::Key_A:
            player->WalkLeft(true);
            break;

        case Qt::Key_D:
            player->WalkRight(true);
            break;

        case Qt::Key_F:

        {
            player_high = !player_high;

            QVector3D pos(player->Pos().x(), 0, player->Pos().z());

            if (player_high) {
                player->FlyTo(pos + QVector3D(0, player_fly_height, 0), player->Dir(), 2000.0f);
            }
            else {
                player->FlyTo(pos + QVector3D(0, player_height, 0), player->Dir(), 2000.0f);
            }

            break;
        }

        case Qt::Key_Backspace:

            player->FlyTo(env->PlayerBackPos(), env->PlayerBackDir(), 2000.0f);
            break;

        case Qt::Key_Tab:

            urlentrywidget->Show();

            break;

        }

    }

}

bool Game::keyReleaseEvent(QKeyEvent * e)
{

    if (urlentrywidget->IsVisible()) {
        return true;
    }

    switch (e->key()) {

    case Qt::Key_W:
        player->WalkForward(false);
        break;

    case Qt::Key_S:
        player->WalkBack(false);
        break;

    case Qt::Key_A:
        player->WalkLeft(false);
        break;

    case Qt::Key_D:
        player->WalkRight(false);
        break;

    }

    return false;

}

void Game::LoadBookmarks()
{

    QString filename("bookmarks.txt");

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Game::LoadBookmarks(): File " << filename << " can't be loaded";
        return;
    }

    QTextStream ifs(&file);

    while (!ifs.atEnd()) {

        QString eachline = ifs.readLine();

        if (eachline[0] != '#') { //not a comment line
            bookmarks_list.push_back(eachline);
        }

    }

    file.close();

    qDebug() << "Game::LoadBookmarks() - Loaded" << bookmarks_list.size() << "bookmarks.";

}
