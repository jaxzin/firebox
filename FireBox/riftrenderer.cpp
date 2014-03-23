#include "riftrenderer.h"

using namespace OVR;

RiftRenderer::RiftRenderer()
{

    oculus_detected = false;

    kappa.push_back(1.0);
    kappa.push_back(1.63);
    kappa.push_back(7.8);
    kappa.push_back(0.0);

    counted_frames = 0;
    fps = 0;

    //using Oculus Rift SDK
    System::Init(Log::ConfigureDefaultLog(LogMask_All));

    pManager = *DeviceManager::Create();

    if (pManager == NULL) {
        qDebug() << "Warning: OVR::DeviceManager::Create() returned NULL ";
        return;
    }

    qDebug() << "Created pManager: pointer address" << pManager;
    //DeviceEnumerator<HMDDevice> devEnum = pManager->EnumerateDevices<OVR::HMDDevice>();

    //qDebug() << "Device type" << devEnum.GetType();
    pHMD = *pManager->EnumerateDevices<HMDDevice>().CreateDevice();

    if (pHMD == NULL) {
        qDebug() << "Warning: pManager->EnumerateDevices<>().CreateDevice() returned NULL ";
        //QMessageBox::critical(NULL, QString("Error"), QString("Could not detect Oculus Rift device."));
        return;
    }

    if (!pHMD->GetDeviceInfo(&hmd)) {
        qDebug() << "Warning: pHMD->GetDeviceInfo() returned NULL ";
        //QMessageBox::critical(NULL, QString("Error"), QString("Could not get information for Oculus Rift device."));
        return;
    }

    //by this point, the hmd data is set
    rift_MonitorName = QString(hmd.DisplayDeviceName);
    rift_EyeDistance = hmd.InterpupillaryDistance;
    rift_DistortionK[0] = hmd.DistortionK[0];
    rift_DistortionK[1] = hmd.DistortionK[1];
    rift_DistortionK[2] = hmd.DistortionK[2];
    rift_DistortionK[3] = hmd.DistortionK[3];

    pSensor = *pHMD->GetSensor();

    if (!pSensor) {
        qDebug() << "Warning: pHMD->GetSensor() returned NULL ";
        //QMessageBox::critical(NULL, QString("Error"), QString("Could not communicate with Oculus Rift sensor."));
        return;
    }

    SFusion = new OVR::SensorFusion();
    SFusion->AttachToSensor(pSensor);

    oculus_detected = true;

}

void RiftRenderer::initializeGL()
{   

    SetupLeftRightProjectionMatrices();

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
    //glFuncs.glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, view_w, view_h);
    //glFuncs.glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, view_w, view_h);
    glFuncs.glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, view_w, view_h);
    //glFuncs.glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthbuffer);
    glFuncs.glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthbuffer);

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

    framerate_time.start();

}

void RiftRenderer::SetViewportSize(const int w, const int h)
{

    view_w = w;
    view_h = h;

}

void RiftRenderer::RenderClear()
{

    glFuncs.glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}

void RiftRenderer::RenderLeftEye()
{

    glViewport(0, 0, view_w/2, view_h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glLoadMatrixf(left_projection_matrix);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

}

void RiftRenderer::RenderRightEye()
{

    //Rift SDK way    
    glViewport(view_w/2, 0, view_w/2, view_h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glLoadMatrixf(right_projection_matrix);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

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

    const float halfLensSeparation = hmd.LensSeparationDistance / hmd.HScreenSize / 2.0f;

    GLfloat lensCentreLeft[2] = {0.25, 0.5};
    GLfloat lensCentreRight[2] = {0.75, 0.5};

    const float scaleFactor = 0.8f; //in Oculus SDK example it's "1.0f/Distortion.Scale"
    const float as = float(view_w / 2.0f) / float(view_h); //rift's "half screen aspect ratio";
    GLfloat scale[2] = {0.5f, as}; //scale by 0.5, since viewport is half horizontally
    GLfloat scaleIn[2] = {2.0f * scaleFactor, 1.0f / as * scaleFactor};

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

    //My method is to keep the distortion symmetric in the shader, and bring the centres of projection inward to the lens centres, by translating textured quads
    //I really believe this is to be a more correct method than performing an asymmetric distortion within the shader.
    const float eye_separation = 0.25f - halfLensSeparation;

    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex2f(0.0f + eye_separation, 0);
    glTexCoord2f(0.5, 0);
    glVertex2f(0.5f + eye_separation, 0);
    glTexCoord2f(0.5, 1);
    glVertex2f(0.5f + eye_separation, 1);
    glTexCoord2f(0, 1);
    glVertex2f(0.0f + eye_separation, 1);
    glEnd();

    shader.setUniformValue(leftEyeLocation, 0);
    shader.setUniformValue(lensCentreLocation, lensCentreRight[0], lensCentreRight[1]);

    glBegin(GL_QUADS);
    glTexCoord2f(0.5, 0);
    glVertex2f(0.5f - eye_separation, 0);    
    glTexCoord2f(1, 0);
    glVertex2f(1.0f - eye_separation, 0);
    glTexCoord2f(1, 1);
    glVertex2f(1.0f - eye_separation, 1);
    glTexCoord2f(0.5, 1);
    glVertex2f(0.5f - eye_separation, 1);
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

 bool RiftRenderer::IsRiftDetected() const
 {     
     return oculus_detected;
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

    qDebug() << kappa[0] << kappa[1] << kappa[2] << kappa[3];

}

void RiftRenderer::UpdateDeviceRotation()
{

    if (pSensor) {

        OVR::Quatf hmdOrient = SFusion->GetOrientation();

        OVR::Vector3f x_vec = hmdOrient.Rotate(OVR::Vector3f(1, 0, 0));
        OVR::Vector3f y_vec = hmdOrient.Rotate(OVR::Vector3f(0, 1, 0));
        OVR::Vector3f z_vec = hmdOrient.Rotate(OVR::Vector3f(0, 0, -1)); //OpenGL - forward is along negative z-axis

        right = QVector3D(x_vec.x, x_vec.y, x_vec.z);
        up = QVector3D(y_vec.x, y_vec.y, y_vec.z);
        forward = QVector3D(z_vec.x, z_vec.y, z_vec.z);

    }    
    else { //otherwise we apply no extra rotation (identity)

        right = QVector3D(1, 0, 0);
        up = QVector3D(0, 1, 0);
        forward = QVector3D(0, 0, -1);

    }


}

void RiftRenderer::GetOrientation(QVector3D & right, QVector3D & up, QVector3D & forward)
{

    UpdateDeviceRotation();

    right = this->right;
    up = this->up;
    forward = this->forward;

}

int RiftRenderer::FPS()
{
    return fps;
}

void RiftRenderer::SetupLeftRightProjectionMatrices()
{

    float aspectRatio = float(hmd.HResolution * 0.5f) / float(hmd.VResolution);
    float yfov = MathUtil::RadToDeg(2.0f * atan((hmd.VScreenSize / 2) / hmd.EyeToScreenDistance));

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(yfov, aspectRatio, 0.005f, 100.0f);

    glGetFloatv(GL_PROJECTION_MATRIX, left_projection_matrix);
    glGetFloatv(GL_PROJECTION_MATRIX, right_projection_matrix);

    glMatrixMode(GL_MODELVIEW);

}
