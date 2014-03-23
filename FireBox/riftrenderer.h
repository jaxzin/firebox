#ifndef RIFTRENDERER_H
#define RIFTRENDERER_H

#include <QtOpenGL>
#include <QGLFunctions>

#include <GL/glu.h>

extern "C" {
#include "../libovr_nsb/OVR.h"
}

class RiftRenderer
{

public:

    RiftRenderer();

    void initializeGL();

    void SetViewportSize(const int w, const int h);

    bool IsRiftDetected();

    void RenderClear();
    void RenderLeftEye();
    void RenderRightEye();
    void RenderToRift();

    void DrawCalibrationGrid(const int density) const;

    float GetEyeSeparation() const;
    void SetEyeSeparation(const float f);

    float GetKappa(const int i) const;
    QList <float> GetKappas() const;
    QString GetKappasString() const;
    void SetKappa(const int i, const float f);

    void GetCurrentDeviceRotation(double rot[16]);

    int FPS();

private:

    GLuint framebuffer;
    GLuint colorbuffer;
    GLuint depthbuffer;

    int view_w;
    int view_h;

    QGLFunctions glFuncs;

    QGLShaderProgram shader;
    int frameBufLocation;
    int leftEyeLocation;
    int kappa1Location;
    int kappa2Location;
    int kappa3Location;
    int kappa4Location;
    int lensCentreLocation;
    int scaleInLocation;
    int scaleLocation;

    float eye_fov;
    float eye_separation;
    QList <float> kappa;

    Device *dev; //oculus rift device pointer

    QTime framerate_time;
    int counted_frames;
    int fps;

};

#endif // RIFTRENDERER_H
