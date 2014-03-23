#include "riftrenderer.h"

RiftRenderer::RiftRenderer() :
    eye_fov(90.0f),
    eye_separation(0.04f)
{

    framerate_time.start();
    counted_frames = 0;
    fps = 0;

    kappa.push_back(1.0);    
    kappa.push_back(1.7);
    kappa.push_back(0.7);
    kappa.push_back(15.0);

    /*
    kappa.push_back(1.0);
    kappa.push_back(0.22);
    kappa.push_back(0.24);
    kappa.push_back(0.0);
    */


    dev = openRift(0, 0);

    if (!dev)
    {
        qDebug() << "Could not locate Rift\n";
        qDebug() << "Be sure you have read/write permission to the proper /dev/hidrawX device\n";

    }
    else {

        qDebug() << "Device Info:";
        qDebug() << "\tname:     " << dev->name;
        qDebug() << "\tlocation: " << dev->location;
        qDebug() << "\tvendor:   " << dev->vendorId;
        qDebug() << "\tproduct:  " << dev->productId;

        // Fire up Sensor update thread
        runSensorUpdateThread(dev);

    }

}

void RiftRenderer::initializeGL()
{   

    //glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

    QString supported_extensions((char *)glGetString(GL_EXTENSIONS));

    if (!supported_extensions.contains("GL_EXT_framebuffer_object")) {
        qDebug() << "Warning - framebuffer objects not supported.";
    }  

    glFuncs = QGLFunctions(QGLContext::currentContext());

    if (framebuffer > 0) {
        //delete framebuffers if already existing - we are reallocating new ones
        //with a resolution that matches the viewport/window size
        glFuncs.glDeleteFramebuffers(1, &framebuffer);
        glDeleteTextures(1, &colorbuffer);
        glFuncs.glDeleteRenderbuffers(1, &depthbuffer);
    }

    //generate offscreen framebuffer to render to
    glFuncs.glGenFramebuffers(1, &framebuffer);
    glGenTextures(1, &colorbuffer);
    glFuncs.glGenRenderbuffers(1, &depthbuffer);

    glFuncs.glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    glBindTexture(GL_TEXTURE_2D, colorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, view_w, view_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFuncs.glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorbuffer, 0);

    glFuncs.glBindRenderbuffer(GL_RENDERBUFFER, depthbuffer);
    glFuncs.glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, view_w, view_h);
    glFuncs.glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthbuffer);

    GLenum status = glFuncs.glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status == GL_FRAMEBUFFER_COMPLETE) {
        qDebug() << "Framebuffer set up:" << framebuffer << depthbuffer << colorbuffer << "Resolution:" << view_w << view_h;
    }
    else {
        qDebug() << "Problem setting up framebuffer";
    }


    glFuncs.glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glFuncs.glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    //set up the shader that inverts the optics
    if (shader.shaders().empty()) {

        shader.addShaderFromSourceFile(QGLShader::Vertex, "./assets/rift_vert.txt");
        shader.addShaderFromSourceFile(QGLShader::Fragment, "./assets/rift_frag.txt");

        if (!shader.link()) {
            qDebug() << "Linking shader failed: " << shader.log();
        }

        frameBufLocation = shader.uniformLocation("framebuf");
        leftEyeLocation = shader.uniformLocation("left_eye");
        kappa1Location = shader.uniformLocation("kappa_0");
        kappa2Location = shader.uniformLocation("kappa_1");
        kappa3Location = shader.uniformLocation("kappa_2");
        kappa4Location = shader.uniformLocation("kappa_3");
        lensCentreLocation = shader.uniformLocation("LensCenter");
        scaleLocation = shader.uniformLocation("Scale");
        scaleInLocation = shader.uniformLocation("ScaleIn");

    }
    //qDebug() << frameBufLocation << leftEyeLocation;

}

void RiftRenderer::SetViewportSize(const int w, const int h)
{

    view_w = w;
    view_h = h;

}

bool RiftRenderer::IsRiftDetected()
{
    return (dev != NULL);
}

void RiftRenderer::RenderClear()
{

    glFuncs.glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}

void RiftRenderer::RenderLeftEye()
{

    //bind rendering to left eye framebuffer
    //glFuncs.glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glViewport(0, 0, 800, 800);
    //glViewport(0, 0, 640, 800);
    glViewport(0, 0, view_w/2, view_h);
    //glViewport(0, 0, view_h, view_h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(eye_fov, 1.0f, 0.5f, 500.0f);

    glMatrixMode(GL_MODELVIEW);

}

void RiftRenderer::RenderRightEye()
{

    //bind rendering to right eye framebuffer
    //glFuncs.glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glViewport(0, 0, 800, 800);
    //glViewport(0, 0, 640, 800);
    glViewport(view_w/2, 0, view_w/2, view_h);
    //glViewport(0, 0, view_h, view_h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(eye_fov, 1.0f, 0.5f, 500.0f);

    glMatrixMode(GL_MODELVIEW);

}

void RiftRenderer::RenderToRift()
{   

    //keeping track of fps
    ++counted_frames;
    if (framerate_time.elapsed() > 1000) {
        qDebug() << "FPS:" << counted_frames;
        framerate_time.restart();
        fps = counted_frames;
        counted_frames = 0;
    }
    const float scaleFactor = 0.9f; //in Oculus SDK example it's "1.0f/Distortion.Scale"
    const float as = float(view_w / 2.0f) / float(view_h); //rift's "half screen aspect ratio";

    GLfloat lensCentreLeft[2] = {0.25, 0.5};
    GLfloat lensCentreRight[2] = {0.75, 0.5};
    //GLfloat scale[2] = {view_w / 2.0f * scaleFactor, view_h / 2.0f * scaleFactor * as};
    //GLfloat scaleIn[2] = {2.0f / float(view_w), 2.0f / float(view_h) / as};
    GLfloat scale[2] = {0.5f, as};
    GLfloat scaleIn[2] = {2.0f * scaleFactor, 1.0f / as * scaleFactor};

    //qDebug() << "scale" << scale[0] << scale[1] << "Scalein" << scaleIn[0] << scaleIn[1] << "view" << view_w << view_h << "as" << as;

    //render to back buffer
    glFuncs.glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glViewport(0, 0, 1280, 800);
    glViewport(0, 0, view_w, view_h);

    //set colour to white
    glColor3f(1, 1, 1);

    //orthographic projection onto the image plane
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, 1, 0, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    //setup the left eye texture sampling
    glBindTexture(GL_TEXTURE_2D, colorbuffer);
    shader.setUniformValue(frameBufLocation, colorbuffer);
    shader.bind();
    shader.setUniformValue(leftEyeLocation, 1);
    shader.setUniformValue(kappa1Location, kappa[0]);
    shader.setUniformValue(kappa2Location, kappa[1]);
    shader.setUniformValue(kappa3Location, kappa[2]);
    shader.setUniformValue(kappa4Location, kappa[3]);
    shader.setUniformValue(scaleLocation, scale[0], scale[1]);
    shader.setUniformValue(scaleInLocation, scaleIn[0], scaleIn[1]);
    //shader.setUniformValue(scaleLocation, 1.0f, 1.0f);
    //shader.setUniformValue(scaleInLocation, 1.0f, 1.0f);

    shader.setUniformValue(lensCentreLocation, lensCentreLeft[0], lensCentreLeft[1]);

    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex2f(0.0f + eye_separation, 0);

    glTexCoord2f(0, 1);
    glVertex2f(0.0f + eye_separation, 1);

    glTexCoord2f(0.5, 1);
    //glTexCoord2f(1, 1);
    glVertex2f(0.5f + eye_separation, 1);

    glTexCoord2f(0.5, 0);
    //glTexCoord2f(1, 0);
    glVertex2f(0.5f + eye_separation, 0);
    glEnd();

    shader.setUniformValue(leftEyeLocation, 0);
    shader.setUniformValue(lensCentreLocation, lensCentreRight[0], lensCentreRight[1]);

    glBegin(GL_QUADS);
    glTexCoord2f(0.5, 0);
    //glTexCoord2f(0, 0);
    glVertex2f(0.5f - eye_separation, 0);

    glTexCoord2f(0.5, 1);
    //glTexCoord2f(0, 1);
    glVertex2f(0.5f - eye_separation, 1);

    glTexCoord2f(1, 1);
    glVertex2f(1.0f - eye_separation, 1);

    glTexCoord2f(1, 0);
    glVertex2f(1.0f - eye_separation, 0);
    glEnd();

    shader.release();

    glBindTexture( GL_TEXTURE_2D, 0);



    //report gl errors
    /*
    GLenum errCode = glGetError();
    if (errCode != GL_NO_ERROR) {
        qDebug() << (char *)gluErrorString(errCode);
    }
    */


}

void RiftRenderer::DrawCalibrationGrid(const int density) const
{

    //set colour to white
    glColor3f(1, 0, 0);

    //orthographic projection onto the image plane
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, 1, 0, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glBegin(GL_LINES);
    for (int i=-density; i<=density; ++i) {

        const float x = float(i) / float(density);

        glVertex2f(x, 0);
        glVertex2f(x, 1);

        glVertex2f(0, x);
        glVertex2f(1, x);

    }
    glEnd();

}

float RiftRenderer::GetEyeSeparation() const
{

    return eye_separation;

}

void RiftRenderer::SetEyeSeparation(const float f)
{

    eye_separation = f;
    qDebug() << "Eye_separation:" << eye_separation;

}

float RiftRenderer::GetKappa(const int i) const
{

    if (i < 0 || i >= kappa.size()) {
        qDebug() << "RiftRenderer::GetKappa() - warning, index" << i << "should be between 0 and 3";
        return 1.0f;
    }

    return kappa[i];

}

QList <float> RiftRenderer::GetKappas() const
{
    return kappa;
}

QString RiftRenderer::GetKappasString() const
{

    QString kappa_text;

    for (int i=0; i<kappa.size(); ++i) {
        kappa_text += QString("kappa_") + QString::number(i) + ":" + QString::number(kappa[i]) + "  ";
    }

    return kappa_text;

}

void RiftRenderer::SetKappa(const int i, const float f)
{

    if (i < 0 || i >= kappa.size()) {
        qDebug() << "RiftRenderer::SetKappa() - warning, index" << i << "should be between 0 and 3";
        return;
    }

    kappa[i] = f;

}

void RiftRenderer::GetCurrentDeviceRotation(double rot[16]) {

    if (dev) { //if we opened the rift device

        double m4[16];
        //quat_toMat4(dev->Q, m4);
        quat_toMat4(dev->QP, m4);
        mat4_toRotationMat(m4, rot);       

    }
    else { //otherwise we apply no extra rotation (identity)

        rot[0] = 1.0f; rot[4] = 0.0f; rot[8] = 0.0f; rot[12] = 0.0f;
        rot[1] = 0.0f; rot[5] = 0.0f; rot[9] = -1.0f; rot[13] = 0.0f;
        rot[2] = 0.0f; rot[6] = 1.0f; rot[10] = 0.0f; rot[14] = 0.0f;
        rot[3] = 0.0f; rot[7] = 0.0f; rot[11] = 0.0f; rot[15] = 1.0f;

    }

}

int RiftRenderer::FPS()
{
    return fps;
}
