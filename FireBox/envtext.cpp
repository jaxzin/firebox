#include "envtext.h"

EnvText::EnvText(const QString & t) :
    text(t),
    tex(0),
    tex_width(192),
    tex_height(384)
{

    img = QImage(tex_width, tex_height, QImage::Format_ARGB32);
    img.fill(0x00000000);

    scroll_time.start();

    /*
    for (int y = 0; y < image.height(); y++) {
        for (int x = 0; x < image.height(); x++) {
            QColor c = QColor::fromRgba(image.pixel(x, y));
            c.setHsv(c.hue(), c.saturation(), c.value());
            image.setPixel(x, y, c.rgba());
        }
    }
    */

    QTextOption option;
    option.setWrapMode(QTextOption::WordWrap);

    QPainter painter(&img);            
    //painter.fillRect(QRectF(0, 0, 128, 128), Qt::white);
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 12));
    //painter.drawLine(5, 5, 25, 25);
    painter.drawText(QRectF(4, 4, tex_width-4, tex_height-4), text, option);
    painter.end();

    QImage glimg;
    glimg = QGLWidget::convertToGLFormat( img );

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, 4, glimg.width(), glimg.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, glimg.bits());

    glBindTexture(GL_TEXTURE_2D, 0);

    //qDebug() << "Generating gl texture" << tex << "for text:" << text;

}

EnvText::~EnvText()
{

    if (tex != 0) {
        glDeleteTextures(1, &tex);
        tex = 0;
    }

}

void EnvText::DrawGL()
{

    if (tex) {

        //write all text to glimage
        //make opengl texture from glimage

        glPushMatrix();

        MathUtil::FacePosDirGL(pos, dir);

        glTranslatef(-0.5f, 0.0f, -0.385f);

        float iw = 0.9f;
        float ih = 0.9f;

        float scroll_val = -float(scroll_time.elapsed()) / 1000.0f / 10.0f;

        /*
        if (img.width() > img.height()) {
            iw = 1.0f;
            ih = float(img.height()) / float(img.width());
        }
        else {
            iw = float(img.width()) / float(img.height());
            ih = 1.0f;
        }
        */       

        glColor3f(0.5f, 0.5f, 0.5f);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, tex);

        glBegin(GL_QUADS);

        glTexCoord2f(0, 0 + scroll_val);
        glVertex2f(0, 0);

        glTexCoord2f(1, 0 + scroll_val);
        glVertex2f(iw, 0);

        glTexCoord2f(1, 0.5f + scroll_val);
        glVertex2f(iw, ih);

        glTexCoord2f(0, 0.5f + scroll_val);
        glVertex2f(0, ih);

        glEnd();

        glDisable(GL_TEXTURE_2D);

        glPopMatrix();

    }    

}

bool EnvText::WorthyOfTextBox(const QString & t)
{

    return t.simplified().length() > 50;

}
