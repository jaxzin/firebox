#ifndef RIFTRENDERER_H
#define RIFTRENDERER_H

#include <QtOpenGL>
#include <QtGui>
#include <QGLFunctions>

#include <pthread.h>
#include <GL/glu.h>

#include <OVR.h>

#include "mathutil.h"

/*
extern "C" {
#include <libovr_nsb/OVR.h>
}
*/

class RiftRenderer
{

public:

    RiftRenderer();

    void initializeGL();

    void SetViewportSize(const int w, const int h);   

    void RenderClear();
    void RenderLeftEye();
    void RenderRightEye();
    void RenderToRift();

    void GetOrientation(QVector3D & right, QVector3D & up, QVector3D & forward);

    void DrawCalibrationGrid(const int density) const;

    bool IsRiftDetected() const;
    float GetKappa(const int i) const;
    QList <float> GetKappas() const;
    QString GetKappasString() const;
    void SetKappa(const int i, const float f);   

    int FPS();

    // Thread funcs
    //void runSensorUpdateThread( Device *dev );
    //static void *threadFunc( void *data );

private:

    void SetupLeftRightProjectionMatrices();

    void UpdateDeviceRotation();

    GLuint framebuffer;
    GLuint colorbuffer;
    GLuint depthbuffer;

    GLfloat left_projection_matrix[16];
    GLfloat right_projection_matrix[16];

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

    QList <float> kappa;

    //Device *dev; //oculus rift device pointer
    OVR::Ptr<OVR::DeviceManager> pManager;
    OVR::Ptr<OVR::HMDDevice> pHMD;
    OVR::Ptr<OVR::SensorDevice> pSensor;
    OVR::SensorFusion *SFusion;

    OVR::HMDInfo hmd;

    QString rift_MonitorName;
    float rift_EyeDistance;
    float rift_DistortionK[4];

    QTime framerate_time;
    int counted_frames;
    int fps;

    bool oculus_detected;

    //these members are updated each time updateOrientation() is called via GetOrientation()
    QVector3D right;
    QVector3D up;
    QVector3D forward;

};

#endif // RIFTRENDERER_H
