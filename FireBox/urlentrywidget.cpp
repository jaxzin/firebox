#include "urlentrywidget.h"

URLEntryWidget::URLEntryWidget(const Player * p) :
    player(p)
{

    curtext = QString("www.dgp.toronto.edu/~mccrae/");

    textgeom.SetFixedSize(true, 0.01f);
    UpdateTextGeom();

    textgeom_instruct.SetFixedSize(true, 0.008f);
    textgeom_instruct.Text(QString("Enter URL (close - tab, enter - load):"));

    time.start();

    Show();

}

void URLEntryWidget::UpdateTextGeom()
{
    textgeom.Text(QString("http://") + curtext);
}

void URLEntryWidget::AddLetter(const QString & c)
{

    curtext.append(c);
    curtext = curtext.trimmed();
    UpdateTextGeom();

}

void URLEntryWidget::RemoveLetter()
{

    if (curtext.length() > 0) {
        curtext.chop(1);
    }

    UpdateTextGeom();

}

void URLEntryWidget::Show()
{
    time_fadestart.start();
    hidden = false;
}

void URLEntryWidget::Hide()
{
    time_fadestart.start();
    hidden = true;
}

bool URLEntryWidget::IsVisible()
{
    return !hidden;
}

QString URLEntryWidget::Text() const
{

    return textgeom.Text().toLower();

}

void URLEntryWidget::DrawGL()
{

    if (hidden && time_fadestart.elapsed() > 1000) {
        return;
    }

    const float alpha_val = hidden ? qMax(0.0f, 1.0f - float(time_fadestart.elapsed())/1000.0f) : qMin(1.0f, float(time_fadestart.elapsed())/1000.0f);

    QVector3D dir = player->Dir();
    dir.setY(0.0f);
    dir.normalize();

    const float len1 = textgeom.TextLength() * 1.1f + 0.05f;
    const float len2 = textgeom.TextLength() * 1.2f + 0.05f;

    //enable blending since this widget fades in and out
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glPushMatrix();

    MathUtil::FacePosDirGL(player->Pos() + dir, dir * (-1.0f));    

    //adds some depth motion over time
    glTranslatef(0, -0.25f, sinf(time.elapsed()/1000.0f)*0.05f);

    glBegin(GL_QUADS);

    glColor4f(0.6f, 0.6f, 1.0f, alpha_val);
    glVertex3f(-len2/2.0f, -0.05f, -0.05f);
    glVertex3f(len2/2.0f, -0.05f, -0.05f);
    glVertex3f(len2/2.0f, 0.1f, -0.05f);
    glVertex3f(-len2/2.0f, 0.1f, -0.05f);

    glColor4f(1, 1, 1, alpha_val);
    glVertex3f(-len1/2.0f, -0.04f, -0.02f);
    glVertex3f(len1/2.0f, -0.04f, -0.02f);
    glVertex3f(len1/2.0f, 0.08f, -0.02f);
    glVertex3f(-len1/2.0f, 0.08f, -0.02f);

    glEnd();

    glTranslatef(-textgeom.TextLength()/2.0f, 0, 0);

    glColor4f(0.3f, 0.3f, 0.3f, alpha_val);
    glTranslatef(0.0f, -0.02f, 0.0f);
    textgeom.DrawGL();

    glColor4f(0.5f, 0.5f, 0.5f, alpha_val);
    glTranslatef(0.05f, 0.06f, 0.0f);
    textgeom_instruct.DrawGL();

    glPopMatrix();

    glDisable(GL_BLEND);

}
