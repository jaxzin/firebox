#include "game.h"

Game::Game() :
    player_height(2.25f),
    player_fly_height(40.0f)
{   

    //player = new Player();
    //player->Height(player_height);
    player.Height(player_height);

    //env = new Environment();
    //env.SetPlayer(&player);

    QString instruct_text = QString("(Mouse left, right) - Rotate the view (about Y axis or 'yaw')");
    instruct_text += "(Tab) - Toggle visibility of URL entry widget";
    instruct_text += "(Enter) - Add URL as doorway in current room";
    instruct_text += "(Backspace) - Navigate back (or backspace when entering URL)";
    instruct_text += "(W,A,S,D) - Walk around";
    instruct_text += "(F) - Flight mode (see the layout of the rooms from above)";
    instruct_text += "(G) - Toggle visibility of calibration grid and distortion parameters";
    instruct_text += "(R) - Reset environment";
    instruct_text += "(1,2) - Adjust kappa_0";
    instruct_text += "(3,4) - Adjust kappa_1";
    instruct_text += "(5,6) - Adjust kappa_2";
    instruct_text += "(7,8) - Adjust kappa_3";
    instruct_text += "(F11) - Toggle fullscreen mode";

    //urlentrywidget = new URLEntryWidget(&player);
    urlentrywidget.SetPlayer(&player);

    LoadBookmarks();

}

void Game::Update()
{      

    player.Update();
    //env.Update();
    env2.Update(player);

    //disable movement while widget is visible
    if (urlentrywidget.IsVisible()) {
        player.WalkForward(false);
        player.WalkBack(false);
        player.WalkLeft(false);
        player.WalkRight(false);
    }

}

void Game::DrawGL(const float half_ipd, const QVector3D & up, const QVector3D & forward)
{

    //setup camera    
    player.SetViewGL(half_ipd, up, forward);

    //draw environment    
    //env.DrawGL();
    env2.DrawGL(player);

    //draw "overlay" or vr widgets    
    urlentrywidget.DrawGL();

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

    //env.LoadTextures();
    env2.initializeGL();

    Reset();

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

    player.SpinView(x * sensitivity);
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

    if (urlentrywidget.IsVisible()) {

        if (e->key() == Qt::Key_Tab) {
            urlentrywidget.Hide();
        }
        else if (e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return) {
            //env.AddNewEntrance(env.PlayerCurRoom(), urlentrywidget.Text());
            env2.AddPortal(env2.PlayerCurRoom(), urlentrywidget.Text());
            SoundManager::Play(5, false);
            urlentrywidget.Hide();
        }
        else if (e->key() == Qt::Key_Backspace) {
            urlentrywidget.RemoveLetter();
        }
        else {
            urlentrywidget.AddLetter(e->text());
        }

    }
    else {

        switch (e->key()) {

        case Qt::Key_W:
            player.WalkForward(true);
            break;

        case Qt::Key_S:
            player.WalkBack(true);
            break;

        case Qt::Key_A:
            player.WalkLeft(true);
            break;

        case Qt::Key_D:
            player.WalkRight(true);
            break;

        case Qt::Key_F:

        {

            player.FlightMode(!player.FlightMode());

            QVector3D pos(player.Pos().x(), 0, player.Pos().z());

            if (player.FlightMode()) {
                SoundManager::Play(6, false);
                player.FlyTo(pos + QVector3D(0, player_fly_height, 0), player.Dir(), 2000.0f);
            }
            else {
                SoundManager::Play(7, false);
                player.FlyTo(pos + QVector3D(0, player_height, 0), player.Dir(), 2000.0f);
            }

            break;
        }

        case Qt::Key_R:

            SoundManager::Play(15, false);
            Reset();
            break;

        case Qt::Key_Backspace:

            if (env2.PlayerCanGoBack()) {
                SoundManager::Play(16, false);

                QVector3D new_pos = env2.PlayerBackPos();
                QVector3D new_dir = env2.PlayerBackDir();
                player.FlyTo(new_pos + QVector3D(0, player_height, 0), new_dir, 2000.0f);
            }
            break;

        case Qt::Key_Tab:

            SoundManager::Play(4, false);
            urlentrywidget.Show();

            break;

        }

    }

}

bool Game::keyReleaseEvent(QKeyEvent * e)
{

    if (urlentrywidget.IsVisible()) {
        return true;
    }

    switch (e->key()) {

    case Qt::Key_W:
        player.WalkForward(false);
        break;

    case Qt::Key_S:
        player.WalkBack(false);
        break;

    case Qt::Key_A:
        player.WalkLeft(false);
        break;

    case Qt::Key_D:
        player.WalkRight(false);
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

        QString eachline = ifs.readLine().simplified();
        //qDebug() << eachline;

        if (eachline[0] != '#' || eachline.length() <= 0) { //not a comment or blank line
            bookmarks_list.push_back(eachline);
        }

    }

    file.close();

    qDebug() << "Game::LoadBookmarks() - Loaded" << bookmarks_list.size() << "bookmarks.";

}

void Game::Reset()
{

    //player.Pos(230.0f, player_height, 230.0f);
    player.Pos(0.0f, player_height, 0.0f);
    player.Dir(QVector3D(0, 0, 14));
    player.FlightMode(false);
    player.Flying(false);

    env2.Reset();

    env2.AddImage(0, QImage("assets/instructions.png"));

    for (int i=0; i<bookmarks_list.size(); ++i) {
        env2.AddPortal(0, bookmarks_list[i]);
        //if (i == 0) {
        //    env.AddNewEnvImage(spaceind, QImage("assets/instructions.png"));
        //}
    }

    //set up environment, add an initial space, and my homepage as a start entrance
    /*
    env.Clear();
    int spaceind = env.AddNewSpace(NULL, 0, 0, 225, 225, 240, 240);

    for (int i=0; i<bookmarks_list.size(); ++i) {
        env.AddNewEntrance(spaceind, bookmarks_list[i]);
        if (i == 0) {
            env.AddNewEnvImage(spaceind, QImage("assets/instructions.png"));
        }
    }
    */

    urlentrywidget.Hide();

}
